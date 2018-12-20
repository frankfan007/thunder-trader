#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Resource.h"
#include <string>
#include <list>
using namespace std;
// CDlgMeddle 对话框

class CDlgMeddle : public CDialogEx
{
    DECLARE_DYNAMIC(CDlgMeddle)

public:
    CDlgMeddle(
        CWnd* pParent,
        string _strStrategyID,
        string _strIp,
        unsigned int _uPort,
        string _strName,
        string _strStrategyName,
        string _strStrategyComment);   // 标准构造函数
    virtual ~CDlgMeddle();

    
    string m_strIp;
    unsigned int m_uPort;
    string m_strName;
    list< tuple< string, string, string> > m_listStrategies;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    //virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
public:
    CButton m_butSendMeddle;
    CButton m_butGetResponse;
    CEdit m_editMeddleCommand;
    CListCtrl m_listMeddleResponse;
    CStatic m_textLabel;
    CStatic m_textCommandLabel;
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedButtonSendMeddle();
    afx_msg void OnBnClickedButtonGetMeddleResponse();
    void AppendOutput(string,string);
    void SetConfig(
        string _strIp,
        unsigned int _uPort,
        string _strName,
        list< tuple< string, string, string> > & _listStrategies);
};
