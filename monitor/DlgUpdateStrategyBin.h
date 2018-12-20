#pragma once
#include "afxwin.h"
#include <vector>
#include <string>

// CDlgUpdateStrategyBin 对话框

class CDlgUpdateStrategyBin : public CDialogEx
{
    DECLARE_DYNAMIC(CDlgUpdateStrategyBin)

public:
    static std::string s_strResult;
    CDlgUpdateStrategyBin(std::vector<std::string> & ValidBinName,CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CDlgUpdateStrategyBin();
    std::vector<std::string> & m_ValidBinName;
// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_UPDATE_STRATEGY_BIN };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    CComboBox m_combValidStrategyBins;
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
};
