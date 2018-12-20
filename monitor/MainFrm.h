// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问 
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "OutputWnd.h"
#include "Resource.h"
#include <vector>
#include <memory>
#include "TradeServerPanel.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <future>
#include <atomic>
#include <mutex>
#include "RequiryDataCallbackInterface.h"
#include <condition_variable>
#include <unordered_map>
#include <set>
#include <tuple>
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio;
using namespace std;


typedef list< tuple< string, string, string> > TStrategiesListType;

typedef tuple<string/*IP*/, unsigned short/*PORT*/, string /*NAME*/, TStrategiesListType /*STRATEGY*/> TStrategiesInfoType;

enum TWinLayoutType { DefaultLayout, FocusLayout, MeanLayout };

class COutlookBar : public CMFCOutlookBar
{
    virtual BOOL AllowShowOnPaneMenu() const 
    { 
        return TRUE;
    }
    virtual void GetPaneName(CString& strName) const 
    { 
        BOOL bNameValid = strName.LoadString(IDS_OUTLOOKBAR);
        ASSERT(bNameValid); 
        if (!bNameValid) 
            strName.Empty(); 
    }
public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

class CMyMFCRibbonBar :public CMFCRibbonBar
{
public:
    virtual BOOL OnShowRibbonQATMenu(CWnd* pWnd, int x, int y, CMFCRibbonBaseElement* pHit)
    {
        return FALSE;
    }
    virtual BOOL OnShowRibbonContextMenu(CWnd* pWnd, int x, int y, CMFCRibbonBaseElement* pHit)
    {
        return FALSE;
    }
};

class CMainFrame : public CMDIFrameWndEx
{
    DECLARE_DYNAMIC(CMainFrame)
public:
    CMainFrame();

    bool m_boolFreezeWindow = false;
    bool m_boolIsHorizLayout = true;
    TWinLayoutType m_enumCurrentLayout = DefaultLayout;
    void RelayoutAllWindows();
    
    const char * GetConfigFile();
    void SetNextDocumentParam(
        string/*IP*/, 
        unsigned short/*PORT*/, 
        string/*SER_NAME*/,
        TStrategiesListType & /*STRATEGIES*/);
    TStrategiesInfoType GetNextDocumentParam();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    void ShowResult(const ptree & out_root);
    virtual ~CMainFrame();
    bool m_boolIsShowMeddleButtons = true;
    string m_strConfigFileName;
    CMyMFCRibbonBar     m_wndRibbonBar;
    COutputWnd        m_wndOutput;
    COutlookBar       m_wndNavigationBar;
    
    deque<tuple<string, unsigned int, ptree, MRequiryDataCallbackInterface*, TCustomParam>> m_deqTaskQue;
    std::mutex m_mtxTaskQueMtx;
    std::condition_variable m_cvTaskQueCv;
    atomic_bool m_boolIsThreadRunning;
    vector<std::future<bool>> m_vecThreads;

    std::mutex m_mtxAllRequiryDataCallbacksMtx;
    set<MRequiryDataCallbackInterface*> m_setAllRequiryDataCallbacks;
    void AddRequiryDataCallback(MRequiryDataCallbackInterface*);
    void RemoveRequiryDataCallback(MRequiryDataCallbackInterface*);
    bool IsRequiryDataCallbackExist(MRequiryDataCallbackInterface*);



    string m_strIPAddress;
    unsigned short m_uPort;
    string m_strServName;
    list< tuple< string, string, string> > m_listStrateies;
    
    vector<std::shared_ptr<CTradeServerPanel>> m_vecTradeServerList;
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnWindowManager();
    afx_msg void OnApplicationLook(UINT id);
    afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
    DECLARE_MESSAGE_MAP()
    void SetDockingWindowIcons(BOOL bHiColorIcons);
    BOOL CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, vector<std::shared_ptr<CTradeServerPanel>> & trees, int nInitialWidth);
    
public:
    afx_msg LRESULT OnMsgCMainFrameShowMessage(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMsgDragJsonFile(WPARAM wParam, LPARAM lParam);
    afx_msg void OnWindowLayoutFocus();
    afx_msg void OnUpdateWindowLayoutFocus(CCmdUI *pCmdUI);
    afx_msg void OnWindowLayoutMean();
    afx_msg void OnUpdateWindowLayoutMean(CCmdUI *pCmdUI);
    afx_msg void OnWindowLayoutHorizFirst();
    afx_msg void OnUpdateWindowLayoutHorizFirst(CCmdUI *pCmdUI);
    afx_msg void OnWindowLayoutFreezn();
    afx_msg void OnUpdateWindowLayoutFreezn(CCmdUI *pCmdUI);
    afx_msg void OnButtonResetAllProbeGraph();
    afx_msg void OnButtonShowHideCustomButton();
    afx_msg void OnClose();
};


