#pragma once
#include "afxwin.h"
#include "StrategyData.h"

// CDlgSetShareValue 对话框

class CDlgSetShareValue : public CDialogEx
{
    DECLARE_DYNAMIC(CDlgSetShareValue)

public:
    CDlgSetShareValue(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CDlgSetShareValue();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_SET_SHARE_VALUE };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    CEdit m_editIndex;
    CEdit m_editNewValue;
    static StrategyData::TSharedIndexType s_intIndex;
    static double s_dbNewValue;
    afx_msg void OnBnClickedOk();
};
