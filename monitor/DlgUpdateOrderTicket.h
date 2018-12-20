#pragma once
#include "afxwin.h"


// CDlgUpdateOrderTicket 对话框

class CDlgUpdateOrderTicket : public CDialogEx
{
    DECLARE_DYNAMIC(CDlgUpdateOrderTicket)
    static int s_intMaxTickets;
    static int s_intRemainTickets;
public:
    CDlgUpdateOrderTicket(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CDlgUpdateOrderTicket();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_UPDATE_ORDER_TICKET };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    CEdit m_editMaxTicketsCount;
    CEdit m_editRemainTicketsCount;
    afx_msg void OnBnClickedOk();
};
