// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问 
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// Monitor.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Monitor.h"
#include "MainFrm.h"

#include "GraphFrm.h"
#include "GraphDoc.h"
#include "GraphView.h"

#include "MeddleView.h"
#include "MeddleFrame.h"
#include "MeddleDoc.h"

#include "FloatingProfitView.h"
#include "FloatingProfitFrm.h"
#include "FloatingProfitDoc.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMonitorApp

BEGIN_MESSAGE_MAP(CMonitorApp, CWinAppEx)
    ON_COMMAND(ID_APP_ABOUT, &CMonitorApp::OnAppAbout)
    // 基于文件的标准文档命令
    ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
    // 标准打印设置命令
    ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CMonitorApp 构造

CMonitorApp::CMonitorApp()
{
    m_bHiColorIcons = TRUE;

    // 支持重新启动管理器
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
    // 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
    //     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
    //     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
    System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

    // TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
    //为 CompanyName.ProductName.SubProduct.VersionInformation
    SetAppID(_T("Monitor.AppID.NoVersion"));

    // TODO: 在此处添加构造代码，
    // 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CMonitorApp 对象

CMonitorApp theApp;


// CMonitorApp 初始化

BOOL CMonitorApp::InitInstance()
{
    // 如果一个运行在 Windows XP 上的应用程序清单指定要
    // 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
    //则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // 将它设置为包括所有要在应用程序中使用的
    // 公共控件类。
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinAppEx::InitInstance();


    // 初始化 OLE 库
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }

    AfxEnableControlContainer();

    EnableTaskbarInteraction();

    // 使用 RichEdit 控件需要 AfxInitRichEdit2()    
    // AfxInitRichEdit2();

    // 标准初始化
    // 如果未使用这些功能并希望减小
    // 最终可执行文件的大小，则应移除下列
    // 不需要的特定初始化例程
    // 更改用于存储设置的注册表项
    // TODO: 应适当修改该字符串，
    // 例如修改为公司或组织名
    SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
    LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


    InitContextMenuManager();
    InitShellManager();

    InitKeyboardManager();

    InitTooltipManager();
    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
        RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

    // 注册应用程序的文档模板。  文档模板
    // 将用作文档、框架窗口和视图之间的连接
    CMultiDocTemplate* pDocTemplate;
    pDocTemplate = new CMultiDocTemplate(IDR_GraphViewTYPE,
        RUNTIME_CLASS(CGraphDoc),
        RUNTIME_CLASS(CGraphFrame), // 自定义 MDI 子框架
        RUNTIME_CLASS(CGraphView));
    if (!pDocTemplate)
        return FALSE;
    AddDocTemplate(pDocTemplate);


    pDocTemplate = new CMultiDocTemplate(IDR_MeddleViewTYPE,
        RUNTIME_CLASS(CMeddleDoc),
        RUNTIME_CLASS(CMeddleFrame), // 自定义 MDI 子框架
        RUNTIME_CLASS(CMeddleView));
    if (!pDocTemplate)
        return FALSE;
    AddDocTemplate(pDocTemplate);

    pDocTemplate = new CMultiDocTemplate(IDR_FloatingProfitViewTYPE,
        RUNTIME_CLASS(CFloatingProfitDoc),
        RUNTIME_CLASS(CFloatingProfitFrame), // 自定义 MDI 子框架
        RUNTIME_CLASS(CFloatingProfitView));
    if (!pDocTemplate)
        return FALSE;
    AddDocTemplate(pDocTemplate);

    // 创建主 MDI 框架窗口
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
    {
        delete pMainFrame;
        return FALSE;
    }
    m_pMainWnd = pMainFrame;

    // 仅当具有后缀时才调用 DragAcceptFiles
    //  在 MDI 应用程序中，这应在设置 m_pMainWnd 之后立即发生
    // 启用拖/放
    m_pMainWnd->DragAcceptFiles();

    // 分析标准 shell 命令、DDE、打开文件操作的命令行
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);
    cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
    // 启用“DDE 执行”
    EnableShellOpen();
    RegisterShellFileTypes(TRUE);


    // 调度在命令行中指定的命令。  如果
    // 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;
    // 主窗口已初始化，因此显示它并对其进行更新
    pMainFrame->ShowWindow(m_nCmdShow);
    pMainFrame->UpdateWindow();

    return TRUE;
}

int CMonitorApp::ExitInstance()
{
    //TODO: 处理可能已添加的附加资源
    AfxOleTerm(FALSE);
    CleanState();
    return CWinAppEx::ExitInstance();
}

// CMonitorApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CMonitorApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

// CMonitorApp 自定义加载/保存方法

void CMonitorApp::PreLoadState()
{
    BOOL bNameValid;
    CString strName;
    bNameValid = strName.LoadString(IDS_EDIT_MENU);
    ASSERT(bNameValid);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CMonitorApp::LoadCustomState()
{
}

void CMonitorApp::SaveCustomState()
{
}

// CMonitorApp 消息处理程序

