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

// ChildFrm.cpp : CGraphFrame 类的实现
//

#include "stdafx.h"
#include "Monitor.h"

#pragma region ptree
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;
#pragma endregion
#include "GraphFrm.h"
#include "MainFrm.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include "GraphView.h"
#include "GraphDoc.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
void Communicate(const char * address, unsigned int port, const std::stringstream & in, std::stringstream & out);
IMPLEMENT_DYNCREATE(CGraphFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CGraphFrame, CMDIChildWndEx)
    ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

// CGraphFrame 构造/析构

CGraphFrame::CGraphFrame()
{
    
}

CGraphFrame::~CGraphFrame()
{
}

BOOL CGraphFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style &= ~FWS_ADDTOTITLE;
    // TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
    if( !CMDIChildWndEx::PreCreateWindow(cs) )
        return FALSE;

    return TRUE;
}

// CGraphFrame 诊断

#ifdef _DEBUG
void CGraphFrame::AssertValid() const
{
    CMDIChildWndEx::AssertValid();
}

void CGraphFrame::Dump(CDumpContext& dc) const
{
    CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

int CGraphFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
        return -1;
    boost::property_tree::ptree m_treeInitTree;
    bool m_boolTimmerRunning = true;
    string _strStrategyID;
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

void CGraphFrame::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    auto mainf = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
    mainf->RemoveRequiryDataCallback(dynamic_cast<CGraphView*>(GetActiveView()));
    CMDIChildWndEx::OnClose();
}
