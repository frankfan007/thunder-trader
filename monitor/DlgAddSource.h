#pragma once
#include "afxwin.h"
#include "afxpropertygridctrl.h"
#include <string>
#include <map>
using namespace std;
// CDlgAddSource 对话框
class CMyCComboBox :public CComboBox
{

public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnCbnSelchange();
    afx_msg void OnCbnKillfocus();
    afx_msg void OnCbnEditupdate();
    afx_msg void OnCbnSelendcancel();
};
class CDlgAddSource : public CDialogEx
{
    DECLARE_DYNAMIC(CDlgAddSource)
    map<string, string>   m_mapPropNames;
public:
    CDlgAddSource(map<string,string> & propNames,CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CDlgAddSource();

    static string s_strSelectedType;
    static map<string, string> s_mapPropValues;
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_ADD_SOURCE};
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    CMyCComboBox m_combSourceType;
    CMFCPropertyGridCtrl m_propSourceProperties;
    afx_msg void OnBnClickedOk();
protected:
    afx_msg LRESULT OnMsgAddSourceDlgCombSelChanged(WPARAM wParam, LPARAM lParam);
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
