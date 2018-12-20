#include "stdafx.h"
#include "Monitor.h"
#include "StrategyData.h"
#include "GraphDoc.h"
#include "GraphView.h"
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
#include <boost/date_time/posix_time/posix_time.hpp>
#include "GraphFrm.h"
#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include "MainFrm.h"
#include <condition_variable>
using namespace StrategyData;
using namespace boost::property_tree;
using namespace boost::posix_time;
using namespace boost::gregorian;
#define FIRST_BUTTON_NID 2048
#define CUSTOM_BUTTON_HEIGTH 25
#define BUTTON_WIDTH 40
#define START_OR_STOP_BUTTON_WIDTH 10
#ifdef _DEBUG
#define new DEBUG_NEW
#include <stdint.h>
void CGraphView::AssertValid() const
{
    CView::AssertValid();
}

void CGraphView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CGraphDoc* CGraphView::GetDocument() const // 非调试版本是内联的
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGraphDoc)));
    return (CGraphDoc*)m_pDocument;
}
#endif
void Communicate(const char * address, unsigned int port, const std::stringstream & in, std::stringstream & out);

IMPLEMENT_DYNCREATE(CGraphView, CView)
BEGIN_MESSAGE_MAP(CGraphView, CView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_COMMAND_RANGE(FIRST_BUTTON_NID, FIRST_BUTTON_NID+30, &CGraphView::OnCustomButtonClick)
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_MESSAGE(MSG_GRAPH_VIEW_UPDATE, &CGraphView::OnMsgGraphViewUpdate)
    ON_COMMAND(ID_BUTTON_RESET_PROBE_GRAPH, &CGraphView::OnButtonResetProbeGraph)
    ON_MESSAGE(MSG_RESET_PROBE_GRAPH, &CGraphView::OnMsgResetProbeGraph)
    ON_MESSAGE(MSG_INIT_TREE_HAS_FETCHED, &CGraphView::OnMsgInitTreeHasFetched)
    ON_MESSAGE(MSG_SHOW_HIDE_CUSTOM_BUTTON, &CGraphView::OnMsgShowHideCustomButton)
END_MESSAGE_MAP()
#define MAX_POINT_COUNT 36000
#define TIMER_PERIOD_LENGTH 1000
void CGraphView::Ptime2TM(const ptime & CurrentTime, TM & _tmXData)
{
    _tmXData.nYear = CurrentTime.date().year() - 1900;
    _tmXData.nMonth = CurrentTime.date().month();
    _tmXData.nDay = CurrentTime.date().day();
    _tmXData.nHour = CurrentTime.time_of_day().hours();
    _tmXData.nMinute = CurrentTime.time_of_day().minutes();
    _tmXData.nSecond = CurrentTime.time_of_day().seconds();
    _tmXData.nMilliSecond = (int)CurrentTime.time_of_day().fractional_seconds() / 1000;
}
void CGraphView::UintLeast642TM(const uint_least64_t & CurrentTime, TM & _tmXData)
{
    _tmXData.nYear = (CurrentTime/ (uint_least64_t)10000000000000) % 10000 - 1900;
    _tmXData.nMonth = (CurrentTime/ (uint_least64_t)100000000000) % 100;
    _tmXData.nDay = (CurrentTime/ (uint_least64_t)1000000000) % 100;
    _tmXData.nHour = (CurrentTime/ (uint_least64_t)10000000) % 100;
    _tmXData.nMinute = (CurrentTime/ (uint_least64_t)100000) % 100;
    _tmXData.nSecond = (CurrentTime/ (uint_least64_t)1000) % 100;
    _tmXData.nMilliSecond = (CurrentTime) % 3;
}

CGraphView::CGraphView()
{
    auto mainf = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
    mainf->AddRequiryDataCallback(this);
}

CGraphView::~CGraphView()
{

}

BOOL CGraphView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: 在此处通过修改
    //  CREATESTRUCT cs 来修改窗口类或样式

    return CView::PreCreateWindow(cs);
}

void CGraphView::ShowMessage(string msg)
{
    CString * result = new CString(CA2W(msg.c_str()));
    GetParent()->SendMessage(MSG_C_MAIN_FRAME_SHOW_MESSAGE, (WPARAM)result);
}

bool CGraphView::InitChartCtrl(ptree & Graph)
{
    RECT ClientRect;
    this->GetClientRect(&ClientRect);
    
    if (m_vecButtons.size() != 0 && get<0>(*m_vecButtons.begin())->IsWindowVisible())
        ClientRect.bottom-= CUSTOM_BUTTON_HEIGTH;
    
    {
        if (m_hPE)
        {
            PEdestroy(m_hPE);
            m_hPE = NULL;
        }
        m_hPE = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &ClientRect, this->m_hWnd, 1001);
        if (Graph.empty())
            return 0;
        m_strSerialNames.clear();
        unsigned int OverAllSerialCount = 0;
        unsigned int OverAllGraphCount = Graph.size();
        for (auto & Subgraph : Graph)
        {
            OverAllSerialCount += Subgraph.second.find("serials")->second.size();
            for (auto & serial : Subgraph.second.find("serials")->second)
                m_strSerialNames.push_back(wstring(CA2W(serial.first.c_str())));
        }

        m_intGraphRightLimit = 0;



#pragma region 十字心开启
        // Cursor prompting in top left corner //
        PEnset(m_hPE, PEP_bCURSORPROMPTTRACKING, TRUE);
        PEnset(m_hPE, PEP_nCURSORPROMPTSTYLE, PECPS_YVALUE);
        PEnset(m_hPE, PEP_nCURSORPROMPTLOCATION, PECPL_TOP_RIGHT);
#pragma endregion

#pragma region 窗口分割属性
        {
            PEnset(m_hPE, PEP_nSUBSETS, OverAllSerialCount);
            PEnset(m_hPE, PEP_nPOINTS, MAX_POINT_COUNT);
            std::unique_ptr<int[]> nMAS(new int[OverAllGraphCount]);
            unsigned int i = 0;
            for (auto &subset : Graph)
            {
                auto SerialCount = subset.second.find("serials")->second.size();
                nMAS[i] = SerialCount;
                i++;
            }
            PEvset(m_hPE, PEP_naMULTIAXESSUBSETS, nMAS.get(), OverAllGraphCount);


            unsigned int Subset_i = 0;
            for (auto &subset : Graph)
            {

                auto SerialsNode = subset.second.find("serials");
                unsigned int SerialInSubset_i = 0;
                for (auto & serial : SerialsNode->second)
                {
                    float fY = 0.0;
                    PEvsetcellEx(m_hPE, PEP_faYDATA, Subset_i, SerialInSubset_i, &fY);
                    SerialInSubset_i++;
                }
                Subset_i++;
            }
        }
        
#pragma endregion

#pragma region 设置坐标轴的属性
        {
            unsigned int i = 0;
            for (auto & subset : Graph)
            {
                PEnset(m_hPE, PEP_nWORKINGAXIS, i);
                auto Sytle = subset.second.find("style")->second.data();
                if ("Line" == Sytle)
                    PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
                else if ("Bar" == Sytle)
                    PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
                else
                    PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
                //PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);// PESA_ALL
                i++;
            }
        }
#pragma endregion

#pragma region 设置子集的颜色的风格 并且清空默认值
        {
            std::unique_ptr<DWORD[]> dwColor(new DWORD[OverAllSerialCount]);
            std::unique_ptr<DWORD[]> dwStyle(new DWORD[OverAllSerialCount]);

            unsigned int Serial_i = 0;
            for (auto &subset : Graph)
            {
                auto SerialsNode = subset.second.find("serials");
                for (auto & serial : SerialsNode->second)
                {
                    auto Color = serial.second.find("color");
                    dwColor[Serial_i] = m_mapColorMap.find(Color->second.data())->second;
                    dwStyle[Serial_i] = PELT_THINSOLID;
                    Serial_i++;
                }
            }
            PEvset(m_hPE, PEP_dwaSUBSETCOLORS, dwColor.get(), OverAllSerialCount);
            PEvset(m_hPE, PEP_naSUBSETLINETYPES, dwStyle.get(), OverAllSerialCount);
        }

#pragma endregion

#pragma region 创建序列数值提示窗口
        {
            DWORD AnnotationColor = PERGB(155, 42, 42, 142);
            int nHS = 1;
            PEnset(m_hPE, PEP_nWORKINGTABLE, 1);
            PEnset(m_hPE, PEP_nTAROWS, OverAllSerialCount);
            PEnset(m_hPE, PEP_nTACOLUMNS, 2);

            for (unsigned int i = 0;i < OverAllSerialCount;i++)
            {
                PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, i, 0, &nHS);
                PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, i, 0, &AnnotationColor);
            }
            int nTACW = 20;
            PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, 0, &nTACW);
            PEnset(m_hPE, PEP_bSHOWTABLEANNOTATION, TRUE);
            PEnset(m_hPE, PEP_nTABORDER, PETAB_DROP_SHADOW);
            PEnset(m_hPE, PEP_dwTABACKCOLOR, PERGB(255, 255, 255, 255));
            PEnset(m_hPE, PEP_nTATEXTSIZE, 100);
            PEnset(m_hPE, PEP_nTALOCATION, PETAL_INSIDE_PIXEL_UNITS);
            PEnset(m_hPE, PEP_nTAX, 50); // initial setting, we set again further down in code.
            PEnset(m_hPE, PEP_nTAY, 50);
            PEnset(m_hPE, PEP_nTAMOVEABLE, PETAM_FULL);
            PEnset(m_hPE, PEP_nTABORDER, PETAB_INSET);
            PEnset(m_hPE, PEP_nTABEVELSTYLE, PEBS_MEDIUM_SMOOTH);
        }

#pragma endregion



#pragma region 曲线名称
        {
            unsigned int Serial_i = 0;
            for (auto &subset : Graph)
            {
                auto SerialsNode = subset.second.find("serials");
                for (auto & serial : SerialsNode->second)
                {
                    auto NameNode = serial.second.find("name");
                    string nname = NameNode->second.data();
                    wstring wname(nname.begin(), nname.end());
                    PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, Serial_i, (void*)wname.c_str());
                    Serial_i++;
                }
            }
        }
#pragma endregion    

    }

    {
        PEnset(m_hPE, PEP_nMULTIAXESSIZING, TRUE);
        PEnset(m_hPE, PEP_bZOOMWINDOW, TRUE);
        PEnset(m_hPE, PEP_bAUTOSCALEDATA, TRUE);
        //PEnset(m_hPE, PEP_bNULLDATAGAPS, TRUE);//这个属性设定了不要把null点连起来
#pragma region 其他属性
        PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
        PEnset(m_hPE, PEP_bMOUSEDRAGGINGX, TRUE);
        PEnset(m_hPE, PEP_bMOUSEDRAGGINGY, TRUE);
        PEnset(m_hPE, PEP_nCURSORMODE, PECM_FLOATINGXY); // 启动标尺模式
        PEnset(m_hPE, PEP_bMOUSECURSORCONTROL, TRUE);
        PEnset(m_hPE, PEP_nFONTSIZE, PEFS_SMALL);
        PEnset(m_hPE, PEP_nPOINTSIZE, PEPS_SMALL);
        PEszset(m_hPE, PEP_szMAINTITLE, TEXT(""));
        PEszset(m_hPE, PEP_szSUBTITLE, TEXT(""));
        PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
        PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
        PEnset(m_hPE, PEP_bSCROLLINGSCALECONTROL, TRUE);
        PEnset(m_hPE, PEP_bALLOWDATAHOTSPOTS, TRUE);
#pragma endregion
        
#pragma region 坐标轴属性
        PEnset(m_hPE, PEP_nDATETIMEMODE, PEDTM_VB);    // Date/Time Mode
        PEnset(m_hPE, PEP_bUSINGXDATAII, TRUE);     // Using Double Precision XDataII
                                                    //PEnset(m_hPE, PEP_bDATETIMESHOWSECONDS, TRUE);
        //PEnset(m_hPE, PEP_bDATETIMEMILLISECONDS, TRUE);
        PEnset(m_hPE, PEP_nDAYLABELTYPE, PEDLT_3_CHAR);
        PEnset(m_hPE, PEP_nMONTHLABELTYPE, PEDLT_3_CHAR);
        PEnset(m_hPE, PEP_nYEARMONTHDAYPROMPT, PEDP_INSIDE_TOP);
#pragma endregion

#pragma region Set Various Other Properties
        PEnset(m_hPE, PEP_nDATAPRECISION, 8);
        PEnset(m_hPE, PEP_nMULTIAXISSTYLE, PEMAS_GROUP_ALL_AXES);
        PEnset(m_hPE, PEP_nMULTIAXESSIZING, TRUE);
        PEnset(m_hPE, PEP_bYAXISONRIGHT, TRUE);
        PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_NONE);
        PEnset(m_hPE, PEP_nALLOWZOOMING, PEAZ_HORZANDVERT);
        PEnset(m_hPE, PEP_nZOOMSTYLE, PEZS_RO2_NOT);

        PEnset(m_hPE, PEP_bMOUSECURSORCONTROL, TRUE);
        PEnset(m_hPE, PEP_bALLOWDATAHOTSPOTS, TRUE);
        PEnset(m_hPE, PEP_nHOTSPOTSIZE, PEHSS_LARGE);
        PEnset(m_hPE, PEP_nSHOWXAXIS, PESA_GRIDNUMBERS);
        PEnset(m_hPE, PEP_bALLOWMAXIMIZATION, FALSE);
        PEnset(m_hPE, PEP_nGRIDLINECONTROL, PEGLC_NONE);
        PEnset(m_hPE, PEP_bSIMPLELINELEGEND, TRUE);
        PEnset(m_hPE, PEP_bSIMPLEPOINTLEGEND, TRUE);
        PEnset(m_hPE, PEP_nLEGENDSTYLE, PELS_1_LINE_TOP_OF_AXIS);
        PEnset(m_hPE, PEP_nAUTOMINMAXPADDING, 1);
        PEnset(m_hPE, PEP_nGRAPHPLUSTABLEMENU, PEMC_HIDE);
        PEnset(m_hPE, PEP_nTABLEWHATMENU, PEMC_HIDE);
        PEnset(m_hPE, PEP_nMULTIAXISSTYLEMENU, PEMC_SHOW);
        PEnset(m_hPE, PEP_nLEGENDLOCATIONMENU, PEMC_SHOW);
        PEnset(m_hPE, PEP_nSHOWTABLEANNOTATIONSMENU, PEMC_SHOW);
        PEnset(m_hPE, PEP_bALLOWANNOTATIONCONTROL, TRUE);
        PEnset(m_hPE, PEP_bSHOWANNOTATIONS, TRUE);
#pragma endregion

#pragma region Create Background Gradient

        PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
        PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_LIGHT_NO_BORDER);//
        PEnset(m_hPE, PEP_dwTEXTCOLOR, PERGB(255, 255, 255, 255));
        PEnset(m_hPE, PEP_dwGRAPHFORECOLOR, PERGB(255, 255, 255, 255));

        PEnset(m_hPE, PEP_dwDESKGRADIENTSTART, PERGB(255, 255, 255, 255));
        PEnset(m_hPE, PEP_dwDESKGRADIENTEND, PERGB(255, 0, 0, 0));
        PEnset(m_hPE, PEP_nDESKGRADIENTSTYLE, PEGS_RADIAL_TOP_RIGHT);
        PEnset(m_hPE, PEP_nDESKBMPSTYLE, 0);
        PEnset(m_hPE, PEP_nGRAPHGRADIENTSTYLE, 0);
#pragma endregion

#pragma region RenderEngine
        PEnset(m_hPE, PEP_nRENDERENGINE, PERE_GDIPLUS);
#pragma endregion

#pragma region 其他
        PEnset(m_hPE, PEP_nGRAPHANNOTMOVEABLE, 1);
        PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
        PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
        PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
        PEnset(m_hPE, PEP_bLABELBOLD, TRUE);
        PEnset(m_hPE, PEP_bLINESHADOWS, TRUE);
        PEnset(m_hPE, PEP_nDATASHADOWS, PEDS_SHADOWS);
        PEnset(m_hPE, PEP_bSHOWANNOTATIONS, TRUE);
        
#pragma endregion

        PEreinitialize(m_hPE);
        PEresetimage(m_hPE, 0, 0);
        ::InvalidateRect(m_hPE, 0, 0);
    }


    return TRUE;
}

void CGraphView::OnQuiry(ptree & Graph,TCustomParam Counter)
{
    if(-1 != Counter)
    {
        std::unique_lock <std::mutex> lck(m_mtxCallbackMtx, std::defer_lock);
        lck.lock();
        auto RawNode = Graph.find("rawdatetime");
        if (RawNode == Graph.not_found())
            return;
        auto RawDatetime = atoll(RawNode->second.data().c_str());
        if (0 == RawDatetime)
        {
            ptime Current = microsec_clock::local_time();
            Ptime2TM(Current, m_tmXData);
        }
        else
            UintLeast642TM(atoll(RawNode->second.data().c_str()), m_tmXData);
        unsigned int Subset_i = 0;
        auto GraphNode = Graph.find("graph");
        if (GraphNode == Graph.not_found())
            return;
        for (auto &subset : GraphNode->second)
        {
            auto SerialsNode = subset.second.find("serials");
            int SerialCount = SerialsNode->second.size();
            for (auto & serial : SerialsNode->second)
            {
                auto ValueNode = serial.second.find("value");
                float newy = NULLDATAVALUE;
                if ("NULL" != ValueNode->second.data())
                    newy = atof(ValueNode->second.data().c_str());
                m_YData.get()[Subset_i] = newy;
                Subset_i++;
            }
        }
        lck.unlock();
        PostMessage(MSG_GRAPH_VIEW_UPDATE, NULL, NULL);
    }else
    {
        m_treeInitTree = Graph;
        PostMessage(MSG_INIT_TREE_HAS_FETCHED);
    }
}

void CGraphView::OnDraw(CDC* /*pDC*/)
{
    CGraphDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // TODO: 在此处为本机数据添加绘制代码
}

int CGraphView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;
    unsigned int _strategyid=0;
    std::tie(m_strIp, m_uPort, m_strName, m_listStrategies)
        = dynamic_cast<CMainFrame*>(AfxGetMainWnd())->GetNextDocumentParam();
    
    RECT ClientRect;
    this->GetClientRect(&ClientRect);
     bool HasCustomButton = true;
    {
        auto StrategyName = get<2>(m_listStrategies.front());
        string fullpath = boost::filesystem::initial_path<boost::filesystem::path>().string();
        string m_strConfigFileName = fullpath + "\\" + StrategyName + ".json";
        boost::property_tree::ptree tree;
        try {
            boost::property_tree::read_json(ifstream(m_strConfigFileName), tree);
        }
        catch (std::exception & err)
        {
            HasCustomButton = false;
        };


        if (HasCustomButton)
        {
            auto CustonButtonNode = tree.find("CustonButton");
            if (tree.not_found() != CustonButtonNode)
            {
                for (auto & buttonCfg : CustonButtonNode->second)
                {

                    m_vecButtons.push_back(
                        make_tuple(
                            std::unique_ptr<CButton>(new CButton()),
                            CString(CA2W(buttonCfg.first.c_str())),
                            buttonCfg.second.data()
                            )
                        );
                }
            }
            unsigned int _Custom_Button_width = (ClientRect.right - ClientRect.left) / m_vecButtons.size();
            unsigned int nID = FIRST_BUTTON_NID;
            unsigned int xpos = 0;
            for (auto & button : m_vecButtons)
            {
                RECT rect;
                rect.left = xpos;
                rect.top = ClientRect.bottom - CUSTOM_BUTTON_HEIGTH;
                rect.right = xpos + _Custom_Button_width;
                rect.bottom = ClientRect.bottom;
                get<0>(button)->Create(
                    get<1>(button),
                    WS_CHILD | WS_BORDER | WS_VISIBLE | BS_PUSHBUTTON,
                    rect,
                    this,
                    nID);
                nID++;
                xpos += _Custom_Button_width;
            }
        }
    }
    
    PostMessage(MSG_RESET_PROBE_GRAPH);
    return 0;
}

void CGraphView::ResizeViewLayout(int cx, int cy)
{
    LONG GraphButtom = cy;
    if (m_vecButtons.size() != 0 && get<0>(*m_vecButtons.begin())->IsWindowVisible())
    {
        GraphButtom -= CUSTOM_BUTTON_HEIGTH;
        unsigned int _Custom_Button_width = cx / m_vecButtons.size();
        unsigned int xpos = 0;
        for (auto &button : m_vecButtons)
        {
            if (IsWindow(*get<0>(button)))
            {
                get<0>(button)->SetWindowPos(NULL,
                    xpos,
                    cy - CUSTOM_BUTTON_HEIGTH,
                    _Custom_Button_width,
                    CUSTOM_BUTTON_HEIGTH,
                    SWP_NOACTIVATE | SWP_NOZORDER);//SWP_NOMOVE |
            }
            xpos += _Custom_Button_width;
        }
    }

    if (m_hPE)
        ::MoveWindow(m_hPE, 0, 0, cx, GraphButtom, FALSE);
}

void CGraphView::OnSize(UINT nType, int cx, int cy)
{
    
    ResizeViewLayout(cx, cy);
    CView::OnSize(nType, cx, cy);
}

void CGraphView::OnCustomButtonClick(UINT uID)
{
    if (get<2>(m_vecButtons[uID - FIRST_BUTTON_NID])[0] != '*')
    {
        if (::MessageBox(NULL, L"确定要发送这个命令么", get<1>(m_vecButtons[uID - FIRST_BUTTON_NID]), MB_YESNO) != IDYES)
            return;
    }
    string command = get<2>(m_vecButtons[uID - FIRST_BUTTON_NID]);
    ptree RequireMeddleFormat, ResultMeddleFormat;
    std::stringstream in, out;
    RequireMeddleFormat.put("type", "reqmeddle");
    RequireMeddleFormat.put("strategyid", get<0>(m_listStrategies.front()));
    RequireMeddleFormat.put("command", command);
    try {
        boost::property_tree::write_json(in, RequireMeddleFormat);
        Communicate(m_strIp.c_str(), m_uPort, in, out);
        boost::property_tree::read_json(out, ResultMeddleFormat);
    }
    catch (std::exception & err)
    {
        ShowMessage(err.what());
    }
    auto ResultNode = ResultMeddleFormat.find("result");
    if (ResultNode != ResultMeddleFormat.not_found())
        ShowMessage(ResultNode->second.data());
    else
        ShowMessage("Can not find <result>");
}

void CGraphView::OnDestroy()
{
    
    KillTimer(0);
    CView::OnDestroy();
}

void CGraphView::SendReqGetProbe(TCustomParam cus)
{
    try {
        ptree RequireGraphFormat;
        std::stringstream in, out;
        boost::property_tree::ptree result;
        RequireGraphFormat.put("type", "reqgetprobe");
        RequireGraphFormat.put("strategyid", get<0>(m_listStrategies.front()));
        boost::property_tree::write_json(in, RequireGraphFormat);
        auto mainf = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
        std::unique_lock<std::mutex> lck(mainf->m_mtxTaskQueMtx, std::defer_lock);
        lck.lock();
        mainf->m_deqTaskQue.push_back(make_tuple(m_strIp, m_uPort, RequireGraphFormat, this, cus));
        lck.unlock();
        mainf->m_cvTaskQueCv.notify_one();
    }
    catch (...) {}
}

void CGraphView::OnTimer(UINT_PTR nIDEvent)
{
    auto curTime = microsec_clock::universal_time().time_of_day();
    if (
        ((curTime>time_duration( 1, 0, 0, 0)) && (curTime<time_duration( 3, 30, 0, 0)))
        ||
        ((curTime>time_duration( 5, 0, 0, 0)) && (curTime<time_duration( 7, 15, 0, 0)))
        ||
        ((curTime>time_duration(13, 0, 0, 0)) && (curTime<time_duration(18, 30, 0, 0)))
        )
    {
        SendReqGetProbe(m_intRealTimeCounter);
    }
    
    CView::OnTimer(nIDEvent);
}

afx_msg LRESULT CGraphView::OnMsgGraphViewUpdate(WPARAM wParam, LPARAM lParam)
{
    double newx2;
    float newy;
    std::unique_lock <std::mutex> lck(m_mtxCallbackMtx,std::defer_lock);
    lck.try_lock();
    if (lck.owns_lock())
    {
        PEcreateserialdate(&newx2, &m_tmXData, PEDTM_VB);
        PEnset(m_hPE, PEP_nWORKINGTABLE, 1);
        TCHAR buffer[128];
        TCHAR szDate[58];
        for (unsigned int Subset_i = 0;Subset_i < m_uSerialCnt;Subset_i++)
        {
            newy = m_YData.get()[Subset_i];
            PEvsetcellEx(m_hPE, PEP_faXDATAII, Subset_i, m_intRealTimeCounter, &newx2);
            PEvsetcellEx(m_hPE, PEP_faYDATA, Subset_i, m_intRealTimeCounter, &newy);


            if (abs(newy - NULLDATAVALUE)<1e-9)
                _stprintf_s(buffer, TEXT("-"));
            else
                _stprintf_s(buffer, TEXT("%.5f"), newy);
            PEvsetcellEx(m_hPE, PEP_szaTATEXT, Subset_i, 0, (void*)m_strSerialNames[Subset_i].c_str());
            PEvsetcellEx(m_hPE, PEP_szaTATEXT, Subset_i, 1, buffer);
        }
        m_intRealTimeCounter++;
        PEdrawtable(m_hPE, 1, NULL);
        PEreinitialize(m_hPE);
        PEresetimage(m_hPE, 0, 0);
        ::InvalidateRect(m_hPE, NULL, FALSE);
        lck.unlock();    
        m_intGraphRightLimit++;
    }
    if (m_intRealTimeCounter>MAX_POINT_COUNT)
        PostMessage(MSG_RESET_PROBE_GRAPH);
    return 0;
}

void CGraphView::OnButtonResetProbeGraph()
{
    PostMessage(MSG_RESET_PROBE_GRAPH);
}

afx_msg LRESULT CGraphView::OnMsgResetProbeGraph(WPARAM wParam, LPARAM lParam)
{
    KillTimer(0);
    SendReqGetProbe(-1);
    return 0;
}

afx_msg LRESULT CGraphView::OnMsgInitTreeHasFetched(WPARAM wParam, LPARAM lParam)
{
    auto TypeNode = m_treeInitTree.find("type");
    if (TypeNode != m_treeInitTree.not_found() && "error" == TypeNode->second.data())
    {
        AfxMessageBox(L"该策略不存在，可能已经被其他登陆端删除");
        return 0;
    }
    auto GraphNode = m_treeInitTree.find("graph");
    if (GraphNode == m_treeInitTree.not_found())
        return 0;
    m_uSerialCnt = 0;
    for (auto &subset : GraphNode->second)
    {
        auto SerialsNode = subset.second.find("serials");
        m_uSerialCnt += SerialsNode->second.size();
    }
    m_YData = std::shared_ptr<float>(new float[m_uSerialCnt], [](float * p) {if (p) delete[]p;});
    memset(m_YData.get(), float(NULLDATAVALUE), m_uSerialCnt);
    InitChartCtrl(GraphNode->second);
    m_intRealTimeCounter = 0;
    SetTimer(0, TIMER_PERIOD_LENGTH, nullptr);
    return 0;
}

afx_msg LRESULT CGraphView::OnMsgShowHideCustomButton(WPARAM wParam, LPARAM lParam)
{
    bool IsShow = static_cast<bool>(wParam);
    if (IsShow)
    {
        for(auto & Button: m_vecButtons)
        {
            get<0>(Button)->ShowWindow(SW_SHOW);
        }
    }
    else
    {
        for (auto & Button : m_vecButtons)
        {
            get<0>(Button)->ShowWindow(SW_HIDE);
        }
    }
    RECT rect;
    this->GetClientRect(&rect);
    ResizeViewLayout(rect.right, rect.bottom);
    return 0;
}


