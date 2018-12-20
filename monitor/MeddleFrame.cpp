#include "stdafx.h"
#include "MeddleFrame.h"
#include "MainFrm.h"


IMPLEMENT_DYNCREATE(CMeddleFrame, CMDIChildWndEx)
CMeddleFrame::CMeddleFrame() :m_StartAndStopCtrlTimer(m_IOservice)
{
}


CMeddleFrame::~CMeddleFrame()
{
}
BEGIN_MESSAGE_MAP(CMeddleFrame, CMDIChildWndEx)
    ON_WM_CREATE()
END_MESSAGE_MAP()


int CMeddleFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    string _strIp;
    unsigned int _uPort;
    string _strName;
    list< tuple< string, string,string> > _listStrateies;
    std::tie(_strIp, _uPort, _strName, _listStrateies)
        = dynamic_cast<CMainFrame*>(AfxGetMainWnd())->GetNextDocumentParam();
    
    stringstream ssTitle;
    ssTitle << _strName << " " << get<0>(_listStrateies.front()) << " " << get<1>(_listStrateies.front());
    CString Title = CA2W(ssTitle.str().c_str());
    SetWindowText(Title);
    return 0;
}
