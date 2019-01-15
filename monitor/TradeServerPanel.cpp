#include "stdafx.h"
#include "TradeServerPanel.h"
#include "MainFrm.h"
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
#include <sstream>
#include <tuple>
#include <memory>
#include "Resource.h"
#include "DlgAddSource.h"
#include "DlgDeployNewStrategy.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include "GraphDoc.h"
#include "MainFrm.h"
#include <exception>
#include "DlgUpdateStrategyBin.h"
#include "DlgSetShareValue.h"
#include "DlgUpdateOrderTicket.h"
#include <unordered_map>
#pragma region boost::ptime
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;
#pragma endregion

using namespace std;


void Communicate(const char * address, unsigned int port, const std::stringstream & in, std::stringstream & out);
BEGIN_MESSAGE_MAP(CMyCMFCPropertyGridCtrl, CMFCPropertyGridCtrl)
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CMyCMFCListCtrl, CListCtrl)
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CTradeServerPanel, CWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_MESSAGE(MSG_CTRADESERVERPANEL_TAB_CHANGED, &CTradeServerPanel::OnMsgTabChanged)
    ON_MESSAGE(MSG_PROP_ITEM_R_CLICK, &CTradeServerPanel::OnMsgPropItemRClick)
    ON_COMMAND(ID_SOURCEBLACKRCLICK_ADD_MD_SOURCE, &CTradeServerPanel::OnSourceblackrclickAddMdSource)
    ON_COMMAND(ID_SOURCEITEMRCLICK_DEL_MD_SOURCE, &CTradeServerPanel::OnSourceitemrclickDelMdSource)
    ON_COMMAND(ID_SOURCEBLACKRCLICK_ADD_TD_SOURCE, &CTradeServerPanel::OnSourceblackrclickAddTdSource)
    ON_COMMAND(ID_SOURCEITEMRCLICK_DEL_TD_SOURCE, &CTradeServerPanel::OnSourceitemrclickDelTdSource)
    ON_COMMAND(ID_STRATEGYBLACK_DEPLOY_NEW_STRATEGY, &CTradeServerPanel::OnStrategyblackDeployNewStrategy)
    ON_COMMAND(ID_STRATEGYITEM_CANCEL_STRATEGY, &CTradeServerPanel::OnStrategyitemCancelStrategy)
    ON_COMMAND(ID_STRATEGYITEM_STRATEGY_SUPERVISORY, &CTradeServerPanel::OnStrategyitemStrategySupervisory)
    ON_COMMAND(ID_STRATEGYITEM_STRATEGY_SAVE_CONFIG_JSON, &CTradeServerPanel::OnStrategyitemStrategySaveConfigJson)
    ON_COMMAND(ID_STRATEGYITEM_STRATEGY_UPDATE_BIN, &CTradeServerPanel::OnStrategyitemStrategyUpdateBin)
    ON_COMMAND(ID_STRATEGYITEM_CANCEL_STRATEGY_AND_SAVE, &CTradeServerPanel::OnStrategyitemCancelStrategyAndSave)
    ON_COMMAND(ID_SETSHAREDVALUE, &CTradeServerPanel::OnSetsharedvalue)
    ON_COMMAND(ID_STRATEGYITEM_STRATEGY_UPDATE_ORDER_TICKET, &CTradeServerPanel::OnStrategyitemStrategyUpdateOrderTicket)
END_MESSAGE_MAP()

void CMyCMFCPropertyGridCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
    CPoint clientPoint = point;
    this->ScreenToClient(&clientPoint);

    auto hItem = this->HitTest(clientPoint);
    UINT uFlags;
    auto tuple = new std::tuple<CMFCPropertyGridProperty*,int, CPoint>(hItem,0, point);
    m_pwndMessageSink->SendMessage(MSG_PROP_ITEM_R_CLICK, (WPARAM)tuple, (LPARAM)m_uTabID);
}

void CMyCMFCListCtrl::OnContextMenu(CWnd *, CPoint point)
{
    CPoint clientPoint = point;
    this->ScreenToClient(&clientPoint);

    auto hItem = this->HitTest(clientPoint);
    UINT uFlags;
    auto tuple = new std::tuple<CMFCPropertyGridProperty*, int, CPoint>(nullptr,hItem, point);
    m_pwndMessageSink->SendMessage(MSG_PROP_ITEM_R_CLICK, (WPARAM)tuple, (LPARAM)m_uTabID);
}

CTradeServerPanel::CTradeServerPanel(string _Name, string _Ip, unsigned int _Port):
    m_strName(_Name),
    m_strIp(_Ip),
    m_uPort(_Port),
    m_lstStrategy(this,0),
    m_wndSharedValue(this,1),
    m_wndMarketDataSource(this,2),
    m_wndTradeSource(this,3)
{
}

CTradeServerPanel::~CTradeServerPanel()
{
}

bool CTradeServerPanel::GetSelectedStrategyIds(list<tuple< string, string, string> > & strategy_ids)
{
    POSITION pos = m_lstStrategy.GetFirstSelectedItemPosition(); //pos选中的首行位置
    if (pos == NULL)
        return false;
    
    while (pos) //如果选择多行
    {
        int nIdx = m_lstStrategy.GetNextSelectedItem(pos);

        if (nIdx >= 0 && nIdx<m_lstStrategy.GetItemCount())
            strategy_ids.push_back(make_tuple(
                string(CW2A(m_lstStrategy.GetItemText(nIdx, TStrategyColumes::_id).GetString())),
                string(CW2A(m_lstStrategy.GetItemText(nIdx, TStrategyColumes::_comment).GetString())),
                string(CW2A(m_lstStrategy.GetItemText(nIdx, TStrategyColumes::_bin).GetString()))
                )
                );
    }

    return true;
}

void CTradeServerPanel::RemoteGetAndShowResult(const ptree & root)
{
    std::stringstream in, out;
    boost::property_tree::ptree out_root;
    try {
        boost::property_tree::write_json(in, root);
        Communicate(m_strIp.c_str(), m_uPort, in, out);
        boost::property_tree::read_json(out, out_root);
    }
    catch (std::exception & err)
    {
        ShowMessage(err.what());
        return;
    }
    ShowResult(out_root);
}

void CTradeServerPanel::RemoteGetResult(const ptree & root,ptree & out_root)
{
    std::stringstream in, out;
    
    try {
        boost::property_tree::write_json(in, root);
        Communicate(m_strIp.c_str(), m_uPort, in, out);
        boost::property_tree::read_json(out, out_root);
    }
    catch (std::exception & err)
    {
        ShowMessage(err.what());
        return;
    }
}

void CTradeServerPanel::ShowResult(const ptree & out_root)
{
    auto Result = out_root.find("result");
    if (Result != out_root.not_found())
    {
        AfxGetApp()->GetMainWnd()->SendMessage(MSG_C_MAIN_FRAME_SHOW_MESSAGE, (WPARAM)(new CString(CA2W(Result->second.data().c_str()))));

    }
    else
    {
        CString * output = new CString;
        for (auto & keyval : out_root)
        {
            *output += CA2W(keyval.first.c_str());
            *output += L":";
            *output += CA2W(keyval.second.data().c_str());
            *output += L";";
        }

        AfxGetApp()->GetMainWnd()->SendMessage(MSG_C_MAIN_FRAME_SHOW_MESSAGE, (WPARAM)(output));
    }
}

void CTradeServerPanel::ShowMessage(const char * fmt, ...)
{
    char buf[2048];
    buf[0] = 0;
    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buf, 2048, fmt, arg);
    va_end(arg);
    string msg = buf;
    CString * Message = new CString(CA2W(buf));
    AfxGetApp()->GetMainWnd()->SendMessage(MSG_C_MAIN_FRAME_SHOW_MESSAGE, (WPARAM)Message);

}

void CTradeServerPanel::SaveJsonToFile(const ptree & tree)
{
    
    ofstream saveFile(to_iso_string(microsec_clock::local_time()) + ".json");
    if (saveFile.is_open())
    {
        try {
            write_json(saveFile, tree);
        }
        catch (...)
        {
            AfxMessageBox(L"无法保存刚才的操作到json文件");
        }
        saveFile.close();
    }
}

int CTradeServerPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    CMFCTabCtrl::Style sty = CMFCTabCtrl::STYLE_3D_ONENOTE;
    try
    {
        boost::property_tree::ptree root;
        auto config = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetConfigFile();
        if (config == NULL)
            throw std::exception();
        boost::property_tree::read_json(config, root);
        auto items = root.get_child("UIStyle");
        auto styleStr = items.get_child("OutLookBarTabStyle").data();
        if ("STYLE_3D" == styleStr)
            sty = CMFCTabCtrl::STYLE_3D;
        else if ("STYLE_3D_ONENOTE" == styleStr)
            sty = CMFCTabCtrl::STYLE_3D_ONENOTE;
        else if ("STYLE_3D_ROUNDED" == styleStr)
            sty = CMFCTabCtrl::STYLE_3D_ROUNDED;
        else if ("STYLE_3D_ROUNDED_SCROLL" == styleStr)
            sty = CMFCTabCtrl::STYLE_3D_ROUNDED_SCROLL;
        else if ("STYLE_3D_SCROLLED" == styleStr)
            sty = CMFCTabCtrl::STYLE_3D_SCROLLED;
        else if ("STYLE_3D_VS2005" == styleStr)
            sty = CMFCTabCtrl::STYLE_3D_VS2005;
        else if ("STYLE_FLAT" == styleStr)
            sty = CMFCTabCtrl::STYLE_FLAT;
        else if ("STYLE_FLAT_SHARED_HORZ_SCROLL" == styleStr)
            sty = CMFCTabCtrl::STYLE_FLAT_SHARED_HORZ_SCROLL;
        else
            throw std::exception();

    }
    catch (...)
    {
    }
    CRect rectDummy;
    rectDummy.SetRectEmpty();
    if (!m_wndTabs.Create(sty, rectDummy, this, 1))
    {
        TRACE0("未能创建输出选项卡窗口\n");
        return -1;      // 未能创建
    }
    const DWORD dwStyle = WS_CHILD | WS_BORDER | WS_VISIBLE;
    if (!m_lstStrategy.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
        !m_wndSharedValue.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
        !m_wndMarketDataSource.Create(dwStyle, rectDummy, &m_wndTabs, 4)||
        !m_wndTradeSource.Create(dwStyle, rectDummy, &m_wndTabs, 5)
        )
    {
        TRACE0("未能创建输出窗口\n");
        return -1;      // 未能创建
    }
    {
        m_lstStrategy.InsertColumn(TStrategyColumes::_id, _T("ID"), LVCFMT_LEFT, 70);// 插入列 
        m_lstStrategy.InsertColumn(TStrategyColumes::_position, _T("持仓"), LVCFMT_LEFT, 70);// 插入列 
        m_lstStrategy.InsertColumn(TStrategyColumes::_comment, _T("备注"), LVCFMT_LEFT, 70);// 插入列
        m_lstStrategy.InsertColumn(TStrategyColumes::_custom, _T("自定义"), LVCFMT_LEFT, 70);// 插入列 
        m_lstStrategy.InsertColumn(TStrategyColumes::_maxticket, _T("最大门票"), LVCFMT_LEFT, 70);// 插入列 
        m_lstStrategy.InsertColumn(TStrategyColumes::_remainticket, _T("剩余门票"), LVCFMT_LEFT, 70);// 插入列 
        m_lstStrategy.InsertColumn(TStrategyColumes::_bin, _T("策略"), LVCFMT_LEFT, 90);// 插入列 
        LONG lStyle;
        lStyle = GetWindowLong(m_lstStrategy.m_hWnd, GWL_STYLE);// 获取当前窗口style 
        lStyle &= ~LVS_TYPEMASK; // 清除显示方式位 
        lStyle |= LVS_REPORT; // 设置style 
        SetWindowLong(m_lstStrategy.m_hWnd, GWL_STYLE, lStyle);// 设置style 
        DWORD dwStyle2 = m_lstStrategy.GetExtendedStyle();
        dwStyle2 |= LVS_EX_FULLROWSELECT;// 选中某行使整行高亮（只适用与report 风格的listctrl ） 
        dwStyle2 |= LVS_EX_GRIDLINES;// 网格线（只适用与report 风格的listctrl ） 
        dwStyle2 |= LVS_EX_CHECKBOXES;//item 前生成checkbox 控件 
        m_lstStrategy.SetExtendedStyle(dwStyle2); // 设置扩展风格 
    }
    

    m_wndSharedValue.EnableHeaderCtrl(FALSE);  //使用表头   
    m_wndSharedValue.SetVSDotNetLook();  //使用样式   
    m_wndSharedValue.MarkModifiedProperties(); //着重显示更改过的部分 

    m_wndMarketDataSource.EnableHeaderCtrl(FALSE);  //使用表头   
    m_wndMarketDataSource.SetVSDotNetLook();  //使用样式   
    m_wndMarketDataSource.MarkModifiedProperties(); //着重显示更改过的部分 

    m_wndTradeSource.EnableHeaderCtrl(FALSE);  //使用表头   
    m_wndTradeSource.SetVSDotNetLook();  //使用样式   
    m_wndTradeSource.MarkModifiedProperties(); //着重显示更改过的部分 

    UpdateFonts();


    // 将列表窗口附加到选项卡: 
    m_wndTabs.AddTab(&m_lstStrategy, L"策略", (UINT)0);
    m_wndTabs.AddTab(&m_wndSharedValue, L"共享值", (UINT)1);
    m_wndTabs.AddTab(&m_wndMarketDataSource, L"行情源", (UINT)2);
    m_wndTabs.AddTab(&m_wndTradeSource, L"交易源", (UINT)3);
    // 使用一些虚拟文本填写输出选项卡(无需复杂数据)
    /*FillBuildWindow();
    FillDebugWindow();
    FillFindWindow();*/

    return 0;

}

void CTradeServerPanel::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    m_wndTabs.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
    // TODO: 在此处添加消息处理程序代码
}

void CTradeServerPanel::UpdateFonts()
{
    m_lstStrategy.SetFont(&afxGlobalData.fontRegular);
    m_wndSharedValue.SetFont(&afxGlobalData.fontRegular);
    m_wndMarketDataSource.SetFont(&afxGlobalData.fontRegular);
    m_wndTradeSource.SetFont(&afxGlobalData.fontRegular);
    
}

BOOL CMyCMFCTabCtrl::SetActiveTab(int iTab)
{
    GetParent()->SendMessage(MSG_CTRADESERVERPANEL_TAB_CHANGED,(WPARAM)iTab,NULL);
    return CMFCTabCtrl::SetActiveTab(iTab);
}

afx_msg LRESULT CTradeServerPanel::OnMsgTabChanged(WPARAM wParam, LPARAM lParam)
{
    std::stringstream in, out;
    boost::property_tree::ptree root, result;
    switch (static_cast<int>(wParam))
    {
    case 0:root.put("type", "reqgetallrunningstrategies");
        break;
    case 1:root.put("type", "reqallsharedvalue");
        break;
    case 2:root.put("type", "reqgetallmarketdatasource");
        break;
    case 3:root.put("type", "reqgetalltradesource");
        break;
    default:return 0;
    }
    try {
        boost::property_tree::write_json(in, root);
        Communicate(m_strIp.c_str(), m_uPort, in, out);
        boost::property_tree::read_json(out, result);
    }
    catch (std::exception & err)
    {
        ShowMessage("Communicate with remote thunder-trader[%s:%u] fails. Please check your network and try again.[%s]", 
            m_strIp.c_str(), m_uPort, err.what());
        return 0;
    }
    auto TypeNode = result.find("type");
    if (TypeNode != result.not_found() && "error" == TypeNode->second.data())
    {
        ShowResult(result);
        return 0;
    }
    switch (static_cast<int>(wParam))
    {
    case 0: 
    {
        string a = out.str();
        m_lstStrategy.DeleteAllItems();
        int counter = 0;
        for (auto & sid_Node : result)
        {
            CString temp= CA2W(sid_Node.first.c_str());
            int nRow = m_lstStrategy.InsertItem(counter++, temp);
            for (auto & attr_Node : sid_Node.second)
            {
                auto ColNode = m_mapStrategyColumeMap.find(attr_Node.first);
                if (ColNode != m_mapStrategyColumeMap.end())
                {
                    temp = CA2W(attr_Node.second.data().c_str());
                    m_lstStrategy.SetItemText(nRow, ColNode->second, temp);
                }
            }
            
        }
        
    }
        break;
    case 1:
    {
        string a = out.str();
        m_wndSharedValue.RemoveAll();
        auto Values = result.find("values");
        if (Values!= result.not_found())
        {
            for (auto & sharedvalue_Node : Values->second)
            {
                auto AttrProp = new CMFCPropertyGridProperty(CString(CA2W((sharedvalue_Node.first + " " + sharedvalue_Node.second.data()).c_str())));
                m_wndSharedValue.AddProperty(AttrProp);
            }
            m_wndSharedValue.AdjustLayout();
            m_wndSharedValue.ExpandAll(FALSE);
        }
    }
        break;
    case 2:
    {
        m_wndMarketDataSource.RemoveAll();
        for (auto & node : result)
        {
            auto prop = new CMFCPropertyGridProperty(CString(CA2W(node.first.c_str())));
            for (auto col : node.second)
                prop->AddSubItem(new CMFCPropertyGridProperty(CString(CA2W(col.first.c_str())), CString(CA2W(col.second.data().c_str()))));
            m_wndMarketDataSource.AddProperty(prop);
        }
        m_wndMarketDataSource.AdjustLayout();
    }
        break;
    case 3:
    {
        m_wndTradeSource.RemoveAll();
        for (auto & node : result)
        {
            auto prop = new CMFCPropertyGridProperty(CString(CA2W(node.first.c_str())));
            for (auto col : node.second)
            {
                if (col.first == "cancelamount")
                {
                    auto cancelamount_prop = new CMFCPropertyGridProperty(L"cancelamount");
                    for(auto & ins: col.second)
                        cancelamount_prop->AddSubItem(new CMFCPropertyGridProperty(CString(CA2W(ins.first.c_str())), CString(CA2W(ins.second.data().c_str()))));
                    prop->AddSubItem(cancelamount_prop);
                }
                else
                    prop->AddSubItem(new CMFCPropertyGridProperty(CString(CA2W(col.first.c_str())), CString(CA2W(col.second.data().c_str()))));

            }
            m_wndTradeSource.AddProperty(prop);
        }
        m_wndTradeSource.AdjustLayout();
    }
        break;
    }

    return 0;
}

afx_msg LRESULT CTradeServerPanel::OnMsgPropItemRClick(WPARAM wParam, LPARAM lParam)
{
    typedef std::tuple<CMFCPropertyGridProperty*, int, CPoint> ClickMsgType;
    CMFCPropertyGridProperty * CurrentItem_PropertyGrid = nullptr;
    int CurrentItem_ListCtrl = -1;
    CPoint point;
    std::unique_ptr<ClickMsgType> tup(reinterpret_cast<ClickMsgType*>(wParam));
    tie(CurrentItem_PropertyGrid, CurrentItem_ListCtrl, point) = *tup;

    unsigned int TabID = static_cast<unsigned int>(lParam);
    auto pop_menu = [this,&point](int n) {
        CMenu m_Menu;
        m_Menu.LoadMenu(IDR_MENU_R_CLICK);//编辑好的菜单资源  
        CMenu *m_SubMenu = m_Menu.GetSubMenu(n);
        m_SubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    };
    switch (TabID)
    {
        case 0: 
        {
            if (CurrentItem_ListCtrl < 0) 
                pop_menu(4);
            else 
                pop_menu(5);
        };
        break;
        case 1:
        {
            if (nullptr == CurrentItem_PropertyGrid || CurrentItem_PropertyGrid->GetParent() == nullptr)
                pop_menu(8);
        };break;
        case 2:
        {
            if (nullptr == CurrentItem_PropertyGrid)
                pop_menu(0);
            else if(CurrentItem_PropertyGrid->GetParent() == nullptr)
                pop_menu(1);
        };
        break;
        case 3:
        {
            if (nullptr == CurrentItem_PropertyGrid)
                pop_menu(2);
            else if (CurrentItem_PropertyGrid->GetParent() == nullptr)
                pop_menu(3);
        };
        break;
    }
    return 0;
}
//添加行情源
void CTradeServerPanel::OnSourceblackrclickAddMdSource()
{
    std::stringstream in, out;
    boost::property_tree::ptree root, out_root;

    root.put("type", "reqgetsupportedmdtypes");
    try {
        boost::property_tree::write_json(in, root);
        Communicate(m_strIp.c_str(), m_uPort, in, out);
        boost::property_tree::read_json(out, out_root);
    }
    catch (std::exception & err)
    {
        ShowMessage(err.what());
        return;
    }
    map<string, string> keyvalue;
    for (auto & keyval : out_root)
    {
        if ("type" == keyval.first)
            continue;
        keyvalue[keyval.first] = keyval.second.data();
    }

    CDlgAddSource dlg(keyvalue);
    if (IDOK == dlg.DoModal())
    {
        std::stringstream in, out;
        boost::property_tree::ptree root, Rsproot;
        root.put("type", "reqaddmarketdatasource");
        root.put("sourcetype", CDlgAddSource::s_strSelectedType);
        for (auto & keyval : CDlgAddSource::s_mapPropValues)
            root.put(keyval.first, keyval.second);
        try {
            boost::property_tree::write_json(in, root);
            Communicate(m_strIp.c_str(), m_uPort, in, out);
            boost::property_tree::read_json(out, Rsproot);
        }
        catch (std::exception & err)
        {
            ShowMessage(err.what());
            return;
        }
        SaveJsonToFile(root);
        ShowResult(Rsproot);
    }
}
//删除行情源
void CTradeServerPanel::OnSourceitemrclickDelMdSource()
{
    auto cur = m_wndMarketDataSource.GetCurSel();
    string keyword=CW2A(cur->GetName());
    boost::property_tree::ptree root;
    root.put("type", "reqdelmarketdatasource");
    root.put("keyword", keyword);
    RemoteGetAndShowResult(root);
}
//添加交易源
void CTradeServerPanel::OnSourceblackrclickAddTdSource()
{
    std::stringstream in, out;
    boost::property_tree::ptree root, out_root;
    root.put("type", "reqgetsupportedtdtypes");
    try {
        boost::property_tree::write_json(in, root);
        Communicate(m_strIp.c_str(), m_uPort, in, out);
        boost::property_tree::read_json(out, out_root);
    }
    catch (std::exception & err)
    {
        ShowMessage(err.what());
        return;
    }
    map<string, string> keyvalue;
    for (auto & keyval : out_root)
    {
        if ("type" == keyval.first)
            continue;
        keyvalue[keyval.first] = keyval.second.data();
    }
    CDlgAddSource dlg(keyvalue);
    if (dlg.DoModal() == IDOK)
    {
        std::stringstream in, out;
        boost::property_tree::ptree root, Rsproot;
        root.put("type", "reqaddtradesource");
        root.put("sourcetype", CDlgAddSource::s_strSelectedType);
        for (auto & keyval : CDlgAddSource::s_mapPropValues)
            root.put(keyval.first, keyval.second);
        try {
            boost::property_tree::write_json(in, root);
            Communicate(m_strIp.c_str(), m_uPort, in, out);
            boost::property_tree::read_json(out, Rsproot);
        }
        catch (std::exception & err)
        {
            ShowMessage(err.what());
            return;
        }
        SaveJsonToFile(root);
        ShowResult(Rsproot);
    }
}
//删除交易源
void CTradeServerPanel::OnSourceitemrclickDelTdSource()
{
    auto cur = m_wndTradeSource.GetCurSel();
    string keyword = CW2A(cur->GetName());
    
    boost::property_tree::ptree root;
    root.put("type", "reqdeltradesource");
    root.put("keyword", keyword);
    RemoteGetAndShowResult(root);
    
}
//添加新策略(通过图形界面)
void CTradeServerPanel::OnStrategyblackDeployNewStrategy()
{
    CDlgDeployNewStrategy dlg(m_strIp, m_uPort);
    if (dlg.DoModal() == IDOK)
    {
        if (CDlgDeployNewStrategy::s_vecParams.empty())
        {
            SaveJsonToFile(CDlgDeployNewStrategy::s_Result);
            RemoteGetAndShowResult(CDlgDeployNewStrategy::s_Result);
        }
        else
        {
            for (size_t i = 0;i < CDlgDeployNewStrategy::s_vecParams.size();i++)
            {
                CDlgDeployNewStrategy::s_Result.find("comment")->second.data() = CDlgDeployNewStrategy::s_vecParams[i].first;
                auto ParamNode = CDlgDeployNewStrategy::s_Result.find("param");
                try {
                    for (auto & t : CDlgDeployNewStrategy::s_vecParams[i].second)
                    {
                        auto this_param_node = ParamNode->second.find(t.first);
                        if (this_param_node != ParamNode->second.not_found())
                            this_param_node->second.data() = t.second;
                        else
                            throw exception(t.first.c_str());
                    }
                }
                catch (std::exception & err)
                {
                    ShowMessage("can not find param \'%s\'", err.what());
                    continue;
                }
                RemoteGetAndShowResult(CDlgDeployNewStrategy::s_Result);
            }
        }
        
    }
}
//删除策略
void CTradeServerPanel::OnStrategyitemCancelStrategy()
{
    list< tuple< string, string, string> > strategy_ids;
    if (GetSelectedStrategyIds(strategy_ids)==false)
        return;
    CString msg;
    msg.Format(L"确定要撤销这%d个策略吗？", strategy_ids.size());
    if (IDOK == MessageBox(msg, _T("！再次确认"), MB_OKCANCEL | MB_ICONWARNING))
    {
        boost::property_tree::ptree root;
        root.put("type", "reqcancelrunningstrategies");
        root.put("strategyid", "");
        for (auto & sid : strategy_ids)
        {
            root.find("strategyid")->second.data() = get<TStrategyColumes::_id>(sid);
            RemoteGetAndShowResult(root);
        }
    }
}
//删除策略并且保存
void CTradeServerPanel::OnStrategyitemCancelStrategyAndSave()
{
    wchar_t szPath[MAX_PATH];     //存放选择的目录路径 
    CString str;
    ZeroMemory(szPath, sizeof(szPath));
    BROWSEINFO bi;
    bi.hwndOwner = m_hWnd;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = szPath;
    bi.lpszTitle = L"请选择保存目录：";
    bi.ulFlags = 0;
    bi.lpfn = NULL;
    bi.lParam = 0;
    bi.iImage = 0;
    //弹出选择目录对话框
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);
    if (lp && SHGetPathFromIDList(lp, szPath))
    {
        string nstrPath=CW2A(szPath);
        TStrategiesListType strategy_ids;
        if (GetSelectedStrategyIds(strategy_ids) == false)
            return;
        
        boost::property_tree::ptree root,result;
        root.put("type", "reqcancelrunningstrategies");
        root.put("strategyid", "");
        for (auto & id : strategy_ids)
        {
            root.find("strategyid")->second.data() = get<TStrategyColumes::_id>(id);
            RemoteGetResult(root, result);
            auto RedeployNode = result.find("redeploy");
            if (RedeployNode != result.not_found())
            {
                string SaveName = to_iso_string(microsec_clock::local_time()) + ".json";
                auto CommentNode = RedeployNode->second.find("comment");
                if (CommentNode != RedeployNode->second.not_found())
                    SaveName = CommentNode->second.data() + ".json";
                ofstream save(nstrPath + "\\" + SaveName);
                if (save.is_open())
                {
                    try {
                        write_json(save, RedeployNode->second);
                    }
                    catch (std::exception &err)
                    {
                        AfxMessageBox(CA2W(err.what()));
                    }
                    save.close();
                }
                else
                    AfxMessageBox(L"无法打开文件");
            }
            else
                AfxMessageBox(L"返回信息不带有ReDeploy信息!");
        }
    }
}
//监控策略
void CTradeServerPanel::OnStrategyitemStrategySupervisory()
{
    TStrategiesListType strategy_ids;
    if (GetSelectedStrategyIds(strategy_ids) == false)
        return;
    auto strategyid = atoi(get<TStrategyColumes::_id>(strategy_ids.front()).c_str());
    auto comment = get<TStrategyColumes::_comment>(strategy_ids.front());
    auto pos=AfxGetApp()->GetFirstDocTemplatePosition();
    dynamic_cast<CMainFrame*>(AfxGetMainWnd())->SetNextDocumentParam(m_strIp, m_uPort,m_strName, strategy_ids);
    AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_NEW);
}
//保存策略配置
void CTradeServerPanel::OnStrategyitemStrategySaveConfigJson()
{
    wchar_t szPath[MAX_PATH];     //存放选择的目录路径 
    CString str;
    ZeroMemory(szPath, sizeof(szPath));
    BROWSEINFO bi;
    bi.hwndOwner = m_hWnd;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = szPath;
    bi.lpszTitle = L"请选择保存目录：";
    bi.ulFlags = 0;
    bi.lpfn = NULL;
    bi.lParam = 0;
    bi.iImage = 0;
    //弹出选择目录对话框
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);
    if (lp && SHGetPathFromIDList(lp, szPath))
    {
        TStrategiesListType strategy_ids;
        if (GetSelectedStrategyIds(strategy_ids) == false)
            return;
        boost::property_tree::ptree root, result;
        root.put("type", "reqstrategyconfigjson");
        root.put("strategyid", "");
        for (auto & str : strategy_ids)
        {
            root.find("strategyid")->second.data() = get<TStrategyColumes::_id>(str);
            RemoteGetResult(root, result);

            auto ResultNode = result.find("result");
            if (ResultNode != result.not_found())
            {
                CString filename = szPath;
                filename += CString(L"\\") + CA2W((get<TStrategyColumes::_id>(str) +"-"+ get<TStrategyColumes::_comment>(str)).c_str())+L".json";
                
                ofstream save(CW2A(filename.GetString()));
                if (save.is_open())
                {
                    try {
                        write_json(save, ResultNode->second);
                    }
                    catch (std::exception &err)
                    {
                        AfxMessageBox(L"保存错误");
                    }
                    save.close();
                }
            }
        }
    }
}
//升级策略bin文件
void CTradeServerPanel::OnStrategyitemStrategyUpdateBin()
{
    TStrategiesListType strategy_ids;
    if (GetSelectedStrategyIds(strategy_ids) == false)
        return;
    if (strategy_ids.size() > 1)
    {
        AfxMessageBox(L"无法同时升级多个策略，请选择一个策略!");
        return;
    }
    vector<string> bins;
    {//获取所有策略bin文件
        std::stringstream in, out;
        boost::property_tree::ptree root, result;
        root.put("type", "reqallstrategybin");
        try {
            RemoteGetResult(root, result);
            for (auto & node : result)
                bins.push_back(node.second.data());
        }
        catch (std::exception & err)
        {
        }
    }
    
    CDlgUpdateStrategyBin dlg(bins);
    if (dlg.DoModal() == IDOK)
    {
        string res = dlg.s_strResult;
        if (res.empty() == false)
        {
            
            auto str = strategy_ids.front();
            string strategyid = get<TStrategyColumes::_id>(str);
            boost::property_tree::ptree root;
            root.put("type", "requpdatestrategybin");
            root.put("strategyid", strategyid);
            root.put("newbin", res);
            RemoteGetAndShowResult(root);
        }
    }
}
//设置共享值
void CTradeServerPanel::OnSetsharedvalue()
{
    CDlgSetShareValue dlg;
    if (dlg.DoModal() == IDOK)
    {
        boost::property_tree::ptree root;
        root.put("type", "reqmodifysharedvalue");
        root.put("valueid", CDlgSetShareValue::s_intIndex);
        root.put("newvalue", CDlgSetShareValue::s_dbNewValue);
        RemoteGetAndShowResult(root);
    }
}
//设置策略下单门票
void CTradeServerPanel::OnStrategyitemStrategyUpdateOrderTicket()
{
    list< tuple< string, string, string> > strategy_ids;
    if (GetSelectedStrategyIds(strategy_ids) == false)
        return;
    
    
    CDlgUpdateOrderTicket dlg;
    if (dlg.DoModal() == IDOK)
    {
        boost::property_tree::ptree RequireFormat, ResultFormat;
        std::stringstream in, out;
        RequireFormat.put("type", "reqsetordertickets");
        RequireFormat.put("strategyid", "");
        RequireFormat.put("maxticket", CDlgUpdateOrderTicket::s_intMaxTickets);
        RequireFormat.put("remainticket", CDlgUpdateOrderTicket::s_intRemainTickets);
        for (auto & str : strategy_ids)
        {
            boost::property_tree::ptree ResultFormat;
            RequireFormat.find("strategyid")->second.data() = get<TStrategyColumes::_id>(str);
            RemoteGetResult(RequireFormat, ResultFormat);
        }
    }
}

