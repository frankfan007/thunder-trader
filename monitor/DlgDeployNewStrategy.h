#pragma once
#include "afxwin.h"
#include "afxpropertygridctrl.h"
#include <vector>
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
#include <string>
#include <map>
using namespace std;
class CMyStrategyBinCombo :public CComboBox
{
public:

    DECLARE_MESSAGE_MAP()
    afx_msg void OnCbnSelendcancel();
};

class CMyArchiveFileCombo :public CComboBox
{
public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnCbnSelendcancel();
};

class CDlgDeployNewStrategy : public CDialogEx
{
    
    string m_strIP;
    unsigned int m_uPort;
    vector<CString> m_vecInstrumentID;
    vector<CString> m_vecMDSource;
    vector<CString> m_vecTDSource;
    DECLARE_DYNAMIC(CDlgDeployNewStrategy)
public:
    static boost::property_tree::ptree s_Result;
    static vector< pair<string, map<string, string> > > s_vecParams;
    CDlgDeployNewStrategy(
        string _ip,unsigned int _port,
        CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CDlgDeployNewStrategy();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_DEPLOY_STRATEGY };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    CMyStrategyBinCombo m_combStrategyBin;
    CMFCPropertyGridCtrl m_propDataidConfig;
    CMFCPropertyGridCtrl m_propParamConfig;
    CMyArchiveFileCombo m_combStrategyArchives;
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonDeployStrategyAddDataid();
    
    afx_msg void OnBnClickedOk();
    CEdit m_editStrategyComment;
protected:
    afx_msg LRESULT OnMsgStrategyBinChanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMsgArchiveFileChanged(WPARAM wParam, LPARAM lParam);
public:
    CEdit m_editMaxTicketsCountPerDay;
    afx_msg void OnBnClickedCheckIsBatchDeploy();
    CEdit m_editBatchDeployCsvFile;
    afx_msg void OnBnClickedButtonSelectBatchCsvFile();
    CButton m_checkBatchDeploy;
    CButton m_butSelectBatchCsvFile;
};
