#pragma once
#include "stdafx.h"
#include "afxwin.h"
#include <string>
#include <unordered_map>
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include "boost/property_tree/ptree.hpp"  
#include "boost/property_tree/json_parser.hpp"
using namespace boost::property_tree;
using namespace std;

class CMyCMFCTabCtrl :public CMFCTabCtrl
{
    virtual BOOL SetActiveTab(int iTab);
};

class CMyCMFCPropertyGridCtrl :public CMFCPropertyGridCtrl
{
public:
    CWnd * m_pwndMessageSink = nullptr;
    unsigned int m_uTabID;
    CMyCMFCPropertyGridCtrl(CWnd * MessageSink, unsigned int tabID) :
        CMFCPropertyGridCtrl(),
        m_pwndMessageSink(MessageSink),
        m_uTabID(tabID)
    {
        
    };
    DECLARE_MESSAGE_MAP()
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

class CMyCMFCListCtrl :public CListCtrl
{
public:
    CWnd * m_pwndMessageSink = nullptr;
    unsigned int m_uTabID;
    CMyCMFCListCtrl(CWnd * MessageSink, unsigned int tabID) :
        CListCtrl(),
        m_pwndMessageSink(MessageSink),
        m_uTabID(tabID)
    {

    };
    DECLARE_MESSAGE_MAP()
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
class CTradeServerPanel
    : public CWnd
{
public:
    CTradeServerPanel(string _Name,string _Ip,unsigned int _Port);
    ~CTradeServerPanel();
    enum TStrategyColumes {_id,_position,_comment, _custom, _maxticket,_remainticket,_bin};
    unordered_map<string, TStrategyColumes> m_mapStrategyColumeMap = { 
        { "custom",TStrategyColumes::_custom },
        { "remaintickets",TStrategyColumes::_remainticket },
        { "maxtickets" ,TStrategyColumes::_maxticket},
        { "bin" ,TStrategyColumes::_bin},
        { "comment",TStrategyColumes::_comment},
        { "position",TStrategyColumes::_position }
    };
    CMyCMFCTabCtrl m_wndTabs;
    CMyCMFCListCtrl m_lstStrategy;
    CMyCMFCPropertyGridCtrl m_wndSharedValue;
    CMyCMFCPropertyGridCtrl m_wndMarketDataSource;
    CMyCMFCPropertyGridCtrl m_wndTradeSource;
    bool GetSelectedStrategyIds(list< tuple< string, string, string> >&);
private:
    void RemoteGetAndShowResult(const ptree &);
    void RemoteGetResult(const ptree &in,ptree &out);
    void ShowResult(const ptree &);
    void ShowMessage(const char *,...);
    void SaveJsonToFile(const ptree&);
public:
    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    void UpdateFonts();
public:
    string m_strName;
    string m_strIp;
    unsigned int m_uPort;
protected:
    afx_msg LRESULT OnMsgTabChanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMsgPropItemRClick(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnSourceblackrclickAddMdSource();
    afx_msg void OnSourceitemrclickDelMdSource();
    afx_msg void OnSourceblackrclickAddTdSource();
    afx_msg void OnSourceitemrclickDelTdSource();
    afx_msg void OnStrategyblackDeployNewStrategy();
    afx_msg void OnStrategyitemCancelStrategy();
    afx_msg void OnStrategyitemStrategySupervisory();
    afx_msg void OnStrategyitemStrategySaveConfigJson();
    afx_msg void OnStrategyitemStrategyUpdateBin();
    afx_msg void OnStrategyitemCancelStrategyAndSave();
    afx_msg void OnSetsharedvalue();
    afx_msg void OnStrategyitemStrategyUpdateOrderTicket();
};

