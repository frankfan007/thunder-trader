
#include "stdafx.h"

#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include "Monitor.h"
#include <fstream>
#include "MainFrm.h"
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
#include <map>
#include <tuple>
#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <set>
#include "RequiryDataCallbackInterface.h"
using namespace std;

void Communicate(const char * address, unsigned int port, const std::stringstream & in, std::stringstream & out);
#define _IPAddress 0
#define _Port 1
#define _DataTree 2
#define _Callback 3
#define _Custom 4

#pragma region COutlookBar
BEGIN_MESSAGE_MAP(COutlookBar, CMFCOutlookBar)
    ON_WM_DROPFILES()
    ON_WM_CREATE()
END_MESSAGE_MAP()

void COutlookBar::OnDropFiles(HDROP hDropInfo)
{
    CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)GetUnderlyingWindow();
    int CurrentTabIndex = pOutlookBar->GetActiveTab();
    UINT count;
    wchar_t filePath[200];
    tuple<int, set<string> > * result = new tuple<int, set<string> >(CurrentTabIndex, set<string>());
    count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
    if (count>0)
    {
        for (UINT i = 0; i<count; i++)
        {
            int pathLen = DragQueryFile(hDropInfo, i, filePath, sizeof(filePath));
            wstring wfilename = filePath;
            string nfilename(wfilename.begin(), wfilename.end());
            get<1>(*result).insert(nfilename);
        }
    }
    DragFinish(hDropInfo);
    GetParent()->SendMessage(MSG_DRAG_JSON_FILE, (WPARAM)result);
    CMFCOutlookBar::OnDropFiles(hDropInfo);
}

int COutlookBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMFCOutlookBar::OnCreate(lpCreateStruct) == -1)
        return -1;

    DragAcceptFiles(TRUE);

    return 0;
}
#pragma endregion

#pragma region CMainFrame
IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
    ON_WM_CREATE()
    ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
    ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
    ON_WM_SETTINGCHANGE()
    ON_MESSAGE(MSG_C_MAIN_FRAME_SHOW_MESSAGE, &CMainFrame::OnMsgCMainFrameShowMessage)
    ON_MESSAGE(MSG_DRAG_JSON_FILE, &CMainFrame::OnMsgDragJsonFile)
    ON_COMMAND(ID_WINDOW_LAYOUT_FOCUS, &CMainFrame::OnWindowLayoutFocus)
    ON_UPDATE_COMMAND_UI(ID_WINDOW_LAYOUT_FOCUS, &CMainFrame::OnUpdateWindowLayoutFocus)
    ON_COMMAND(ID_WINDOW_LAYOUT_MEAN, &CMainFrame::OnWindowLayoutMean)
    ON_UPDATE_COMMAND_UI(ID_WINDOW_LAYOUT_MEAN, &CMainFrame::OnUpdateWindowLayoutMean)
    ON_COMMAND(ID_WINDOW_LAYOUT_HORIZ_FIRST, &CMainFrame::OnWindowLayoutHorizFirst)
    ON_UPDATE_COMMAND_UI(ID_WINDOW_LAYOUT_HORIZ_FIRST, &CMainFrame::OnUpdateWindowLayoutHorizFirst)
    ON_COMMAND(ID_WINDOW_LAYOUT_FREEZN, &CMainFrame::OnWindowLayoutFreezn)
    ON_UPDATE_COMMAND_UI(ID_WINDOW_LAYOUT_FREEZN, &CMainFrame::OnUpdateWindowLayoutFreezn)
    ON_COMMAND(ID_BUTTON_RESET_ALL_PROBE_GRAPH, &CMainFrame::OnButtonResetAllProbeGraph)
    ON_COMMAND(ID_BUTTON_SHOW_HIDE_CUSTOM_BUTTON, &CMainFrame::OnButtonShowHideCustomButton)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

void CMainFrame::RelayoutAllWindows()
{
    POSITION pos = theApp.GetFirstDocTemplatePosition();
    if (!pos)
        return;
    CMultiDocTemplate *pDT = dynamic_cast<CMultiDocTemplate *>(theApp.GetNextDocTemplate(pos));

    //得到当前文档的个数，即窗口的个数
    POSITION po = pDT->GetFirstDocPosition();
    UINT nCount = 0;
    while (po)
    {
        CDocument *pDoc = pDT->GetNextDoc(po);
        nCount++;
    }
    if (nCount == 0)
        return;
    po = pDT->GetFirstDocPosition();

    CRect rectMain;
    m_wndClientArea.GetClientRect(rectMain);

    UINT nColumn = 0;
    UINT nRow = 0;
    switch (m_enumCurrentLayout)
    {
        case DefaultLayout: {
        };
            break;
        case FocusLayout: {
            if (!po)
                return;
            CDocument *pDoc = pDT->GetNextDoc(po);
            POSITION p = pDoc->GetFirstViewPosition();
            if (!p)
                return;
            CView *pView = pDoc->GetNextView(p);
            CFrameWnd *pFrameWnd = pView->GetParentFrame();
            pFrameWnd->MoveWindow(rectMain.top, rectMain.left, rectMain.Width() / (m_boolIsHorizLayout ? 1 : (nCount > 1 ? 2 : 1)), rectMain.Height() / (m_boolIsHorizLayout ? (nCount > 1 ? 2 : 1) : 1));
            nCount--;
            if (nCount == 0)
                return;
            if (m_boolIsHorizLayout)
                rectMain.top += rectMain.Height() / 2;
            else
                rectMain.left += rectMain.Width() / 2;
            bool bHorizLayout = !m_boolIsHorizLayout;
            (bHorizLayout ? nRow : nColumn) = ceil(sqrt(float(nCount)));
            (bHorizLayout ? nColumn : nRow) = ceil(float(nCount) / (bHorizLayout ? nRow : nColumn));
            if (bHorizLayout)
            {//水平优先
                int cy = rectMain.Height() / nRow;
                int y = rectMain.top;
                for (UINT r = 0; r < nRow; r++)
                {
                    int x = rectMain.left;
                    UINT nCurrColumn = nColumn;
                    if (nCount % nRow)
                        nCurrColumn = ((r < (nCount % nRow)) ? nColumn : (nColumn - 1));
                    int cx = rectMain.Width() / nCurrColumn;
                    for (UINT c = 0; c < nCurrColumn; c++)
                    {
                        if (!po)
                            break;
                        CDocument *pDoc = pDT->GetNextDoc(po);
                        POSITION p = pDoc->GetFirstViewPosition();
                        if (!p)
                            return;
                        CView *pView = pDoc->GetNextView(p);
                        CFrameWnd *pFrameWnd = pView->GetParentFrame();
                        pFrameWnd->MoveWindow(x, y, cx, cy);
                        x += cx;
                    }
                    y += cy;
                }
            }
            else
            {//垂直优先
                int cx = rectMain.Width() / nColumn;
                int x = rectMain.left;
                for (UINT c = 0; c < nColumn; c++)
                {
                    int y = rectMain.top;
                    UINT nCurrRow = nRow;
                    if (nCount % nColumn)
                        nCurrRow = ((c < (nCount % nColumn)) ? nRow : (nRow - 1));
                    int cy = rectMain.Height() / nCurrRow;
                    for (UINT r = 0; r < nCurrRow; r++)
                    {
                        CDocument *pDoc = pDT->GetNextDoc(po);
                        POSITION p = pDoc->GetFirstViewPosition();
                        if (!p)
                            return;
                        CView *pView = pDoc->GetNextView(p);
                        CFrameWnd *pFrameWnd = pView->GetParentFrame();
                        pFrameWnd->MoveWindow(x, y, cx, cy);
                        y += cy;
                    }
                    x += cx;
                }
            }
        };
            break; 
        case MeanLayout: {
            (m_boolIsHorizLayout ? nRow : nColumn) = ceil(sqrt(float(nCount)));
            (m_boolIsHorizLayout ? nColumn : nRow) = ceil(float(nCount) / (m_boolIsHorizLayout ? nRow : nColumn));
            if (m_boolIsHorizLayout)
            {//水平优先
                int cy = rectMain.Height() / nRow;
                int y = rectMain.top;
                for (UINT r = 0; r < nRow; r++)
                {
                    int x = rectMain.left;
                    UINT nCurrColumn = nColumn;
                    if (nCount % nRow)
                        nCurrColumn = ((r < (nCount % nRow)) ? nColumn : (nColumn - 1));
                    int cx = rectMain.Width() / nCurrColumn;
                    for (UINT c = 0; c < nCurrColumn; c++)
                    {
                        if (!po)
                            break;
                        CDocument *pDoc = pDT->GetNextDoc(po);
                        POSITION p = pDoc->GetFirstViewPosition();
                        if (!p)
                            return;
                        CView *pView = pDoc->GetNextView(p);
                        CFrameWnd *pFrameWnd = pView->GetParentFrame();
                        pFrameWnd->MoveWindow(x, y, cx, cy);
                        x += cx;
                    }
                    y += cy;
                }
            }
            else
            {//垂直优先
                int cx = rectMain.Width() / nColumn;
                int x = rectMain.left;
                for (UINT c = 0; c < nColumn; c++)
                {
                    int y = rectMain.top;
                    UINT nCurrRow = nRow;
                    if (nCount % nColumn)
                        nCurrRow = ((c < (nCount % nColumn)) ? nRow : (nRow - 1));
                    int cy = rectMain.Height() / nCurrRow;
                    for (UINT r = 0; r < nCurrRow; r++)
                    {
                        CDocument *pDoc = pDT->GetNextDoc(po);
                        POSITION p = pDoc->GetFirstViewPosition();
                        if (!p)
                            return;
                        CView *pView = pDoc->GetNextView(p);
                        CFrameWnd *pFrameWnd = pView->GetParentFrame();
                        pFrameWnd->MoveWindow(x, y, cx, cy);
                        y += cy;
                    }
                    x += cx;
                }
            }
        };
            break;
    }
    
}

CMainFrame::CMainFrame()
{
    // TODO: 在此添加成员初始化代码
    theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
}

const char * CMainFrame::GetConfigFile()
{
    string fullpath = boost::filesystem::initial_path<boost::filesystem::path>().string();
    m_strConfigFileName = fullpath + "\\Monitor.json";
    return m_strConfigFileName.c_str();
}

void CMainFrame::ShowResult(const ptree & out_root)
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

void CMainFrame::AddRequiryDataCallback(MRequiryDataCallbackInterface * callback)
{
    std::lock_guard<std::mutex> lk(m_mtxAllRequiryDataCallbacksMtx);
    m_setAllRequiryDataCallbacks.insert(callback);
}

void CMainFrame::RemoveRequiryDataCallback(MRequiryDataCallbackInterface * callback)
{
    std::lock_guard<std::mutex> lk(m_mtxAllRequiryDataCallbacksMtx);
    if (m_setAllRequiryDataCallbacks.find(callback) != m_setAllRequiryDataCallbacks.end())
        m_setAllRequiryDataCallbacks.erase(callback);
}

bool CMainFrame::IsRequiryDataCallbackExist(MRequiryDataCallbackInterface* callback)
{
    std::lock_guard<std::mutex> lk(m_mtxAllRequiryDataCallbacksMtx);
    return m_setAllRequiryDataCallbacks.find(callback) != m_setAllRequiryDataCallbacks.end();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    BOOL bNameValid;

    m_wndRibbonBar.Create(this);
    m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

    

    // 启用 Visual Studio 2005 样式停靠窗口行为
    CDockingManager::SetDockingMode(DT_SMART);
    // 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
    EnableAutoHidePanes(CBRS_ALIGN_ANY);

    // 导航窗格将创建在左侧，因此将暂时禁用左侧的停靠: 
    EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

    // 创建并设置“Outlook”导航栏: 
    if (!CreateOutlookBar(m_wndNavigationBar, ID_VIEW_NAVIGATION, m_vecTradeServerList, 250))
    {
        TRACE0("未能创建导航窗格\n");
        return -1;      // 未能创建
    }

    // 已创建 Outlook 栏，应允许在左侧停靠。
    EnableDocking(CBRS_ALIGN_LEFT);
    EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

    // 创建停靠窗口
    {
        BOOL bNameValid;
        // 创建输出窗口
        CString strOutputWnd;
        bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
        ASSERT(bNameValid);
        if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
        {
            TRACE0("未能创建输出窗口\n");
            return FALSE; // 未能创建
        }

        SetDockingWindowIcons(theApp.m_bHiColorIcons);
    }

    m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndOutput);

    // 基于持久值设置视觉管理器和样式
    OnApplicationLook(theApp.m_nAppLook);

    // 启用增强的窗口管理对话框
    EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

    // 将文档名和应用程序名称在窗口标题栏上的顺序进行交换。这
    // 将改进任务栏的可用性，因为显示的文档名带有缩略图。
    ModifyStyle(0, FWS_PREFIXTITLE);

#pragma region 创建数据获取线程
    m_boolIsThreadRunning = true;
    size_t thread_count=std::thread::hardware_concurrency();
    for (unsigned int i = 0;i < thread_count;i++)
        m_vecThreads.push_back(std::async([this]()->bool {
        while (m_boolIsThreadRunning.load())
        {
            std::unique_lock<std::mutex> lck(m_mtxTaskQueMtx);
            m_cvTaskQueCv.wait(lck, [this]()->bool {
                if (false == m_boolIsThreadRunning || m_deqTaskQue.empty() == false)
                    return true;
                else
                    return false;
            });
            if (m_boolIsThreadRunning&&m_deqTaskQue.empty() == false)
            {
                auto data = std::move(m_deqTaskQue.front());
                m_deqTaskQue.pop_front();
                lck.unlock();
                std::stringstream in, out;
                boost::property_tree::write_json(in, get<_DataTree>(data));
                Communicate(get<_IPAddress>(data).c_str(), get<_Port>(data), in, out);
                ptree result;
                boost::property_tree::read_json(out, result);
                if(IsRequiryDataCallbackExist(get<_Callback>(data)))
                    get<_Callback>(data)->OnQuiry(result, get<_Custom>(data));
            }
        }
        return true;
    }));
#pragma endregion

    return 0;
}

void CMainFrame::SetNextDocumentParam(string _ip, unsigned short _port, string _name, TStrategiesListType & _strategyid)
{
    m_strIPAddress= _ip;
    m_uPort= _port;
    m_strServName = _name;
    m_listStrateies= _strategyid;
    
}

TStrategiesInfoType  CMainFrame::GetNextDocumentParam()
{
    return make_tuple(m_strIPAddress,m_uPort, m_strServName, m_listStrateies);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CMDIFrameWndEx::PreCreateWindow(cs) )
        return FALSE;
    // TODO: 在此处通过修改
    //  CREATESTRUCT cs 来修改窗口类或样式

    return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
    HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

    UpdateMDITabbedBarsIcons();
}

BOOL CMainFrame::CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, vector<std::shared_ptr<CTradeServerPanel>> & lists , int nInitialWidth)
{
    
    try
    {
        boost::property_tree::ptree root;
        boost::property_tree::read_json(GetConfigFile(), root);
        auto items = root.get_child("UIStyle");
        if ("NewStyle" == items.get_child("OutLookBarStyle").data())
            bar.SetMode2003();
    }
    catch (...)
    {
        bar.SetMode2003();
    }
        
    
    //

    /*BOOL bNameValid;
    CString strTemp;
    bNameValid = strTemp.LoadString(IDS_SHORTCUTS);
    ASSERT(bNameValid);*/
    if (!bar.Create(L"交易服务器", this, CRect(0, 0, nInitialWidth, 32000), uiID, WS_CHILD | WS_VISIBLE | CBRS_LEFT ))
    {
        return FALSE; // 未能创建
    }
    
    CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)bar.GetUnderlyingWindow();

    if (pOutlookBar == NULL)
    {
        ASSERT(FALSE);
        return FALSE;
    }

    //pOutlookBar->EnableInPlaceEdit(TRUE);

    static UINT uiPageID = 1;

    // 可浮动，可自动隐藏，可调整大小，但不能关闭
    DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE /*| WS_EX_ACCEPTFILES*/;

    CRect rectDummy(0, 0, 0, 0);
    try
    {
        boost::property_tree::ptree root;
        boost::property_tree::read_json(GetConfigFile(), root);
        auto  TradeServerItem = root.get_child("TradeServer");
        unsigned int Count = 0;
        for (auto Server : TradeServerItem)
        {
            map<string, string> keysvalue;
            for (auto EachServerKeyValue : Server.second)
            {
                string key = EachServerKeyValue.first;
                string value = EachServerKeyValue.second.data();
                keysvalue[key] = value;

            }

            std::shared_ptr<CTradeServerPanel> list(new CTradeServerPanel(keysvalue["Name"], keysvalue["IP"], atoi(keysvalue["Port"].c_str())));
            lists.push_back(list);
            CRect rectDummy;
            rectDummy.SetRectEmpty();
            list->Create(NULL, CA2W(keysvalue["Name"].c_str()),WS_VISIBLE, rectDummy,this,0);
            
            string title = keysvalue["Name"] + "(" + keysvalue["IP"] + ":" + keysvalue["Port"] + ")";
            pOutlookBar->AddControl(
                list.get(),
                CA2W(title.c_str()),
                1,
                TRUE,
                dwStyle);

        }
    }
    catch (...)
    {
        
    }
    
    bar.DragAcceptFiles( TRUE);
    

    

    
    
    //bar.SetPaneStyle(bar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC/* | WS_EX_ACCEPTFILES*/);
    //bar.DragAcceptFiles(TRUE);
    if (bar.IsMode2003())
    {
        pOutlookBar->SetImageList(theApp.m_bHiColorIcons ? IDB_PAGES_HC : IDB_PAGES, 24);
        pOutlookBar->SetToolbarImageList(theApp.m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL, 16);
        pOutlookBar->RecalcLayout();

        BOOL bAnimation = theApp.GetInt(_T("OutlookAnimation"), TRUE);
        CMFCOutlookBarTabCtrl::EnableAnimation(bAnimation);

        bar.SetButtonsFont(&afxGlobalData.fontBold);
    }

    return TRUE;
}

void CMainFrame::OnWindowManager()
{
    ShowWindowsDialog();
}

void CMainFrame::OnApplicationLook(UINT id)
{
    CWaitCursor wait;

    theApp.m_nAppLook = id;

    switch (theApp.m_nAppLook)
    {
    case ID_VIEW_APPLOOK_WIN_2000:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
        m_wndRibbonBar.SetWindows7Look(FALSE);
        break;

    case ID_VIEW_APPLOOK_OFF_XP:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
        m_wndRibbonBar.SetWindows7Look(FALSE);
        break;

    case ID_VIEW_APPLOOK_WIN_XP:
        CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
        m_wndRibbonBar.SetWindows7Look(FALSE);
        break;

    case ID_VIEW_APPLOOK_OFF_2003:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
        CDockingManager::SetDockingMode(DT_SMART);
        m_wndRibbonBar.SetWindows7Look(FALSE);
        break;

    case ID_VIEW_APPLOOK_VS_2005:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
        CDockingManager::SetDockingMode(DT_SMART);
        m_wndRibbonBar.SetWindows7Look(FALSE);
        break;

    case ID_VIEW_APPLOOK_VS_2008:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
        CDockingManager::SetDockingMode(DT_SMART);
        m_wndRibbonBar.SetWindows7Look(FALSE);
        break;

    case ID_VIEW_APPLOOK_WINDOWS_7:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
        CDockingManager::SetDockingMode(DT_SMART);
        m_wndRibbonBar.SetWindows7Look(TRUE);
        break;

    default:
        switch (theApp.m_nAppLook)
        {
        case ID_VIEW_APPLOOK_OFF_2007_BLUE:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_BLACK:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_SILVER:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_AQUA:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
            break;
        }

        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
        CDockingManager::SetDockingMode(DT_SMART);
        m_wndRibbonBar.SetWindows7Look(FALSE);
    }

    m_wndOutput.UpdateFonts();
    RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

    theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
    CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
    m_wndOutput.UpdateFonts();
}

afx_msg LRESULT CMainFrame::OnMsgCMainFrameShowMessage(WPARAM wParam, LPARAM lParam)
{
    if (IsWindow(m_wndOutput))
        m_wndOutput.SendMessage(MSG_OUTPUT_WND_DISPLAY_MESSAGE, wParam, NULL);
    return 0;
}

afx_msg LRESULT CMainFrame::OnMsgDragJsonFile(WPARAM wParam, LPARAM lParam)
{
    std::shared_ptr<tuple<int, set<string> >> result(reinterpret_cast<tuple<int, set<string> > *>(wParam));
    for (auto &filename : get<1>(*result))
    {
        std::shared_ptr<ifstream> inFile(new ifstream(filename, ios::binary), [](ifstream*p) {if (p) { if (p->is_open())p->close();delete p; }});
        if (inFile->is_open())
        {
            try {
                boost::property_tree::ptree in_config, out_root;
                boost::property_tree::read_json(*inFile, in_config);
                std::stringstream in, out;
                boost::property_tree::write_json(in, in_config);
                Communicate(
                    m_vecTradeServerList[get<0>(*result)]->m_strIp.c_str(),
                    m_vecTradeServerList[get<0>(*result)]->m_uPort, 
                    in, 
                    out);
                boost::property_tree::read_json(out, out_root);
                ShowResult(out_root);
            }
            catch (std::exception & err)
            {
                AfxGetApp()->GetMainWnd()->SendMessage(MSG_C_MAIN_FRAME_SHOW_MESSAGE, (WPARAM)(new CString(CA2W(err.what()))));
            }
        }
    }
    return 0;
}

void CMainFrame::OnWindowLayoutFocus()
{
    m_enumCurrentLayout = FocusLayout;
    RelayoutAllWindows();
}

void CMainFrame::OnUpdateWindowLayoutFocus(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_enumCurrentLayout==FocusLayout);
}

void CMainFrame::OnWindowLayoutMean()
{
    m_enumCurrentLayout = MeanLayout;
    RelayoutAllWindows();
}

void CMainFrame::OnUpdateWindowLayoutMean(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_enumCurrentLayout == MeanLayout);
}

void CMainFrame::OnWindowLayoutHorizFirst()
{
    m_boolIsHorizLayout = !m_boolIsHorizLayout;
    RelayoutAllWindows();
}

void CMainFrame::OnUpdateWindowLayoutHorizFirst(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_boolIsHorizLayout);
}

void CMainFrame::OnWindowLayoutFreezn()
{
    m_boolFreezeWindow = !m_boolFreezeWindow;
    POSITION pos = theApp.GetFirstDocTemplatePosition();
    if (!pos)
        return;
    CMultiDocTemplate *pDT = dynamic_cast<CMultiDocTemplate *>(theApp.GetNextDocTemplate(pos));
    POSITION po = pDT->GetFirstDocPosition();
    while (po)
    {
        CDocument *pDoc = pDT->GetNextDoc(po);
        POSITION p = pDoc->GetFirstViewPosition();
        if (!p)
            return;
        CView *pView = pDoc->GetNextView(p);
        CFrameWnd *pFrameWnd = pView->GetParentFrame();
        pFrameWnd->ModifyStyle(m_boolFreezeWindow ? WS_CAPTION | WS_THICKFRAME : 0, m_boolFreezeWindow ? 0 : WS_CAPTION | WS_THICKFRAME, SWP_DRAWFRAME);
    }
    if (m_boolFreezeWindow)
        RelayoutAllWindows();
}

void CMainFrame::OnUpdateWindowLayoutFreezn(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_boolFreezeWindow);
}

void CMainFrame::OnButtonResetAllProbeGraph()
{
    POSITION pos = theApp.GetFirstDocTemplatePosition();
    if (!pos)
        return;
    CMultiDocTemplate *pDT = dynamic_cast<CMultiDocTemplate *>(theApp.GetNextDocTemplate(pos));
    POSITION po = pDT->GetFirstDocPosition();
    while (po)
    {
        CDocument *pDoc = pDT->GetNextDoc(po);
        POSITION p = pDoc->GetFirstViewPosition();
        if (!p)
            return;
        CView *pView = pDoc->GetNextView(p);
        pView->PostMessage(MSG_RESET_PROBE_GRAPH);
    }
}

void CMainFrame::OnButtonShowHideCustomButton()
{
    m_boolIsShowMeddleButtons = !m_boolIsShowMeddleButtons;
    POSITION pos = theApp.GetFirstDocTemplatePosition();
    if (!pos)
        return;
    CMultiDocTemplate *pDT = dynamic_cast<CMultiDocTemplate *>(theApp.GetNextDocTemplate(pos));
    POSITION po = pDT->GetFirstDocPosition();
    while (po)
    {
        CDocument *pDoc = pDT->GetNextDoc(po);
        POSITION p = pDoc->GetFirstViewPosition();
        if (!p)
            return;
        CView *pView = pDoc->GetNextView(p);
        pView->PostMessage(MSG_SHOW_HIDE_CUSTOM_BUTTON,(WPARAM)m_boolIsShowMeddleButtons);
    }
}

void CMainFrame::OnClose()
{
    m_boolIsThreadRunning = false;
    m_cvTaskQueCv.notify_all();
    for (auto & fut : m_vecThreads)
        auto a = fut.get();
    CMDIFrameWndEx::OnClose();
}
#pragma endregion
