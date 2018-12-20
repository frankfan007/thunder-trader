#include "stdafx.h"
#include "Monitor.h"
#include "StrategyData.h"
#include "FloatingProfitDoc.h"
#include "FloatingProfitView.h"
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
#include <boost/date_time/posix_time/posix_time.hpp>
#include "FloatingProfitFrm.h"
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
void CFloatingProfitView::AssertValid() const
{
    CView::AssertValid();
}

void CFloatingProfitView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CFloatingProfitDoc* CFloatingProfitView::GetDocument() const // 非调试版本是内联的
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFloatingProfitDoc)));
    return (CFloatingProfitDoc*)m_pDocument;
}
#endif
void Communicate(const char * address, unsigned int port, const std::stringstream & in, std::stringstream & out);

IMPLEMENT_DYNCREATE(CFloatingProfitView, CView)
BEGIN_MESSAGE_MAP(CFloatingProfitView, CView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_MESSAGE(MSG_GRAPH_VIEW_UPDATE, &CFloatingProfitView::OnMsgFloatingProfitViewUpdate)
END_MESSAGE_MAP()
#define MAX_POINT_COUNT 36000
#define TIMER_PERIOD_LENGTH 1000

CFloatingProfitView::CFloatingProfitView()
{
    auto mainf = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
    mainf->AddRequiryDataCallback(this);
}

CFloatingProfitView::~CFloatingProfitView()
{

}

BOOL CFloatingProfitView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: 在此处通过修改
    //  CREATESTRUCT cs 来修改窗口类或样式

    return CView::PreCreateWindow(cs);
}

void CFloatingProfitView::ShowMessage(string msg)
{
    CString * result = new CString(CA2W(msg.c_str()));
    GetParent()->SendMessage(MSG_C_MAIN_FRAME_SHOW_MESSAGE, (WPARAM)result);
}

bool CFloatingProfitView::InitChartCtrl()
{
    RECT ClientRect;
    this->GetClientRect(&ClientRect);
    {
        if (m_hPE)
        {
            PEdestroy(m_hPE);
            m_hPE = NULL;
        }
        m_hPE = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &ClientRect, this->m_hWnd, 1001);




#pragma region 十字心开启
        // Cursor prompting in top left corner //
        PEnset(m_hPE, PEP_bCURSORPROMPTTRACKING, TRUE);
        PEnset(m_hPE, PEP_nCURSORPROMPTSTYLE, PECPS_YVALUE);
        PEnset(m_hPE, PEP_nCURSORPROMPTLOCATION, PECPL_TOP_RIGHT);
#pragma endregion

#pragma region 窗口分割属性
        {
            PEnset(m_hPE, PEP_nSUBSETS, 1);
            PEnset(m_hPE, PEP_nPOINTS, MAX_POINT_COUNT);
            int nMAS[1] = { 1 };
            nMAS[0] = 1;
            PEvset(m_hPE, PEP_naMULTIAXESSUBSETS, nMAS, 1);
            float fY = 0.0;
            PEvsetcellEx(m_hPE, PEP_faYDATA, 0, 0, &fY);


#pragma endregion

#pragma region 设置坐标轴的属性

            PEnset(m_hPE, PEP_nWORKINGAXIS, 0);
            PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);

#pragma endregion

#pragma region 设置子集的颜色的风格 并且清空默认值

            DWORD dwColor[1] = { PERGB(255, 255, 0, 0) };
            DWORD dwStyle[1] = { PELT_MEDIUMSOLID };
            PEvset(m_hPE, PEP_dwaSUBSETCOLORS, dwColor, 1);
            PEvset(m_hPE, PEP_naSUBSETLINETYPES, dwStyle, 1);

#pragma endregion

#pragma region 创建序列数值提示窗口
            {
                DWORD AnnotationColor = PERGB(155, 42, 42, 142);
                int nHS = 1;
                PEnset(m_hPE, PEP_nWORKINGTABLE, 1);
                PEnset(m_hPE, PEP_nTAROWS, 1);
                PEnset(m_hPE, PEP_nTACOLUMNS, 2);

                PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, 0, 0, &nHS);
                PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, 0, 0, &AnnotationColor);

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
            PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, 0, (void*)L"FloatProfit");
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
}

void CFloatingProfitView::OnQuiry(ptree & Graph,TCustomParam Counter)
{
    if(-1 != Counter)
    {
        float  * R =new float();
        std::unique_lock <std::mutex> lck(m_mtxCallbackMtx, std::defer_lock);
        lck.lock();
        auto TypeNode = Graph.find("type");
        if (TypeNode != Graph.not_found()&& TypeNode->second.data()=="error")
            return;
        else
        {
            auto ResultNode = Graph.find("result");
            if (ResultNode != Graph.not_found() && ResultNode->second.data() != "unknown")
                *R = atof(ResultNode->second.data().c_str());
        }
        lck.unlock();
        
        PostMessage(MSG_GRAPH_VIEW_UPDATE, reinterpret_cast<WPARAM>(R), NULL);
    }
}

void CFloatingProfitView::OnDraw(CDC* /*pDC*/)
{
    CFloatingProfitDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // TODO: 在此处为本机数据添加绘制代码
}

int CFloatingProfitView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;
    unsigned int _strategyid=0;
    std::tie(m_strIp, m_uPort, m_strName, m_listStrategies)
        = dynamic_cast<CMainFrame*>(AfxGetMainWnd())->GetNextDocumentParam();
    
    RECT ClientRect;
    this->GetClientRect(&ClientRect);
    InitChartCtrl();
    SetTimer(0, TIMER_PERIOD_LENGTH, nullptr);
    return 0;
}

void CFloatingProfitView::OnSize(UINT nType, int cx, int cy)
{
    
    ::MoveWindow(m_hPE, 0, 0, cx, cy, FALSE);
    CView::OnSize(nType, cx, cy);
}

void CFloatingProfitView::OnDestroy()
{
    
    KillTimer(0);
    CView::OnDestroy();
}

void CFloatingProfitView::SendReqGetFloatingProfit()
{
    try {
        ptree RequireFloatingProfitFormat;
        std::stringstream in, out;
        boost::property_tree::ptree result;
        RequireFloatingProfitFormat.put("type", "getfloatingprofit");
        stringstream ss;
        for (auto & str : m_listStrategies)
            ss << get<0>(str) << ",";
        RequireFloatingProfitFormat.put("strategyid", ss.str());
        boost::property_tree::write_json(in, RequireFloatingProfitFormat);
        auto mainf = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
        std::unique_lock<std::mutex> lck(mainf->m_mtxTaskQueMtx, std::defer_lock);
        lck.lock();
        mainf->m_deqTaskQue.push_back(make_tuple(m_strIp, m_uPort, RequireFloatingProfitFormat, this, 0));
        lck.unlock();
        mainf->m_cvTaskQueCv.notify_one();
    }
    catch (...) {}
}

void CFloatingProfitView::OnTimer(UINT_PTR nIDEvent)
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
        SendReqGetFloatingProfit();
    }
    
    CView::OnTimer(nIDEvent);
}

afx_msg LRESULT CFloatingProfitView::OnMsgFloatingProfitViewUpdate(WPARAM wParam, LPARAM lParam)
{
    double newx2;
    float newy;
    std::unique_lock <std::mutex> lck(m_mtxCallbackMtx,std::defer_lock);
    lck.try_lock();
    if (lck.owns_lock())
    {
        
        PEnset(m_hPE, PEP_nWORKINGTABLE, 1);
        TCHAR buffer[128];
        TCHAR szDate[58];
        
        newy = *reinterpret_cast<float*>(wParam);
        delete reinterpret_cast<float*>(wParam);
        TM tm;
        {
            ptime CurrentTime = second_clock::local_time();;
            tm.nYear = CurrentTime.date().year() - 1900;
            tm.nMonth = CurrentTime.date().month();
            tm.nDay = CurrentTime.date().day();
            tm.nHour = CurrentTime.time_of_day().hours();
            tm.nMinute = CurrentTime.time_of_day().minutes();
            tm.nSecond = CurrentTime.time_of_day().seconds();
            tm.nMilliSecond = (int)CurrentTime.time_of_day().fractional_seconds() / 1000;
            PEcreateserialdate(&newx2, &tm, PEDTM_VB);
        }
        //PEvset(m_hPE, PEP_faXDATAII, &newx2, 1);
        PEvsetcellEx(m_hPE, PEP_faXDATAII, 0, m_intRealTimeCounter, &newx2);
        PEvsetcellEx(m_hPE, PEP_faYDATA, 0, m_intRealTimeCounter, &newy);

        
        if (abs(newy - NULLDATAVALUE)<1e-9)
            _stprintf_s(buffer, TEXT("-"));
        else
            _stprintf_s(buffer, TEXT("%.5f"), newy);
        PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 0, (void*)L"FloatingProfit");
        PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 1, buffer);
        
        m_intRealTimeCounter++;
        PEdrawtable(m_hPE, 1, NULL);
        PEreinitialize(m_hPE);
        PEresetimage(m_hPE, 0, 0);
        ::InvalidateRect(m_hPE, NULL, FALSE);
        lck.unlock();    
        m_intFloatingProfitRightLimit++;
    }
    if (m_intRealTimeCounter>MAX_POINT_COUNT)
        PostMessage(MSG_RESET_PROBE_GRAPH);
    return 0;
}

