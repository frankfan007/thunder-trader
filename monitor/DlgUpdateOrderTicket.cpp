// DlgUpdateOrderTicket.cpp : 实现文件
//

#include "stdafx.h"
#include "Monitor.h"
#include "DlgUpdateOrderTicket.h"
#include "afxdialogex.h"


// CDlgUpdateOrderTicket 对话框
int CDlgUpdateOrderTicket::s_intMaxTickets=-1;
int CDlgUpdateOrderTicket::s_intRemainTickets=-1;
IMPLEMENT_DYNAMIC(CDlgUpdateOrderTicket, CDialogEx)

CDlgUpdateOrderTicket::CDlgUpdateOrderTicket(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG_UPDATE_ORDER_TICKET, pParent)
{

}

CDlgUpdateOrderTicket::~CDlgUpdateOrderTicket()
{
}

void CDlgUpdateOrderTicket::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_STRATEGY_MAX_TICKET, m_editMaxTicketsCount);
    DDX_Control(pDX, IDC_EDIT_STRATEGY_REMAIN_TICKET, m_editRemainTicketsCount);
}


BEGIN_MESSAGE_MAP(CDlgUpdateOrderTicket, CDialogEx)
    ON_BN_CLICKED(IDOK, &CDlgUpdateOrderTicket::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgUpdateOrderTicket 消息处理程序


void CDlgUpdateOrderTicket::OnBnClickedOk()
{
    CString temp;
    m_editMaxTicketsCount.GetWindowTextW(temp);
    s_intMaxTickets = _ttoi(temp);
    m_editRemainTicketsCount.GetWindowTextW(temp);
    s_intRemainTickets = _ttoi(temp);
    CDialogEx::OnOK();
}
