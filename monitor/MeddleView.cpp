#include "stdafx.h"
#include "MeddleView.h"
#include "Resource.h"
#include "MainFrm.h"
IMPLEMENT_DYNCREATE(CMeddleView, CView)

CMeddleView::CMeddleView():m_dlgMainUI(this, "", "", 0, "", "", "")
{
}

CMeddleView::CMeddleView(string _strStrategyID,
    string _strIp,
    unsigned int _uPort,
    string _strName,
    string _strStrategyName,
    string _strStrategyComment):m_dlgMainUI(this,_strStrategyID, _strIp, _uPort, _strName, _strStrategyName, _strStrategyComment)
{
}


CMeddleView::~CMeddleView()
{
}


void CMeddleView::OnDraw(CDC* /*pDC*/)
{
    // TODO: 在此添加专用代码和/或调用基类
}

BEGIN_MESSAGE_MAP(CMeddleView, CView)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()


int CMeddleView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // 创建视图: 
    const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    if (FALSE==m_dlgMainUI.Create(IDD_DIALOG_MEDDLE,this))//dwViewStyle
    {
        return -1;
    }
    m_dlgMainUI.ShowWindow(SW_SHOW);
    boost::property_tree::ptree m_treeInitTree;
    bool m_boolTimmerRunning = true;
    string _strStrategyID;
    string _strIp;
    unsigned int _uPort;
    string _strName;
    list< tuple< string, string,string> > _listStrateies;
    std::tie(_strIp, _uPort, _strName, _listStrateies)
        = dynamic_cast<CMainFrame*>(AfxGetMainWnd())->GetNextDocumentParam();
    stringstream ss;
    
    m_dlgMainUI.SetConfig(_strIp, _uPort, _strName, _listStrateies);
    return 0;
}


void CMeddleView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    CRect rectClient;
    GetClientRect(rectClient);
    if (IsWindow(m_dlgMainUI))
    {
        m_dlgMainUI.SetWindowPos(
            NULL,
            rectClient.left + 1,
            rectClient.top + 1,
            rectClient.Width() - 2,
            rectClient.Height() - 2,
            SWP_NOZORDER);//
    }
}
