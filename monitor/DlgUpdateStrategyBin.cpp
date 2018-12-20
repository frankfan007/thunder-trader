// DlgUpdateStrategyBin.cpp : 实现文件
//

#include "stdafx.h"
#include "Monitor.h"
#include "DlgUpdateStrategyBin.h"
#include "afxdialogex.h"


// CDlgUpdateStrategyBin 对话框
std::string CDlgUpdateStrategyBin::s_strResult;
IMPLEMENT_DYNAMIC(CDlgUpdateStrategyBin, CDialogEx)

CDlgUpdateStrategyBin::CDlgUpdateStrategyBin(std::vector<std::string> & ValidBinName, CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG_UPDATE_STRATEGY_BIN, pParent), m_ValidBinName(ValidBinName)
{

}

CDlgUpdateStrategyBin::~CDlgUpdateStrategyBin()
{
}

void CDlgUpdateStrategyBin::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO1, m_combValidStrategyBins);
}


BEGIN_MESSAGE_MAP(CDlgUpdateStrategyBin, CDialogEx)
    ON_BN_CLICKED(IDOK, &CDlgUpdateStrategyBin::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgUpdateStrategyBin 消息处理程序


BOOL CDlgUpdateStrategyBin::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    for (auto & bin : m_ValidBinName)
        m_combValidStrategyBins.AddString(CA2W(bin.c_str()));
    s_strResult = "";
    return TRUE; 
}


void CDlgUpdateStrategyBin::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    CString current;
    m_combValidStrategyBins.GetWindowTextW(current);
    s_strResult = CW2A(current);
    CDialogEx::OnOK();
}
