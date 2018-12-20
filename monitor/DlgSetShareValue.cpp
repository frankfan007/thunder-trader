// DlgSetShareValue.cpp : 实现文件
//

#include "stdafx.h"
#include "Monitor.h"
#include "DlgSetShareValue.h"
#include "afxdialogex.h"


StrategyData::TSharedIndexType CDlgSetShareValue::s_intIndex = 0;
double CDlgSetShareValue::s_dbNewValue = 0;
// CDlgSetShareValue 对话框

IMPLEMENT_DYNAMIC(CDlgSetShareValue, CDialogEx)

CDlgSetShareValue::CDlgSetShareValue(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG_SET_SHARE_VALUE, pParent)
{

}

CDlgSetShareValue::~CDlgSetShareValue()
{
}

void CDlgSetShareValue::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_INDEX, m_editIndex);
    DDX_Control(pDX, IDC_EDIT_NEW_VALUE, m_editNewValue);
}


BEGIN_MESSAGE_MAP(CDlgSetShareValue, CDialogEx)
    ON_BN_CLICKED(IDOK, &CDlgSetShareValue::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSetShareValue 消息处理程序


void CDlgSetShareValue::OnBnClickedOk()
{
    CString Index, NewValue;
    m_editIndex.GetWindowTextW(Index);
    m_editNewValue.GetWindowTextW(NewValue);
    s_intIndex = _ttoi(Index);
    s_dbNewValue = _ttof(NewValue);
    CDialogEx::OnOK();
}
