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

// FloatingProfitView.h : CFloatingProfitView 类的接口
//

#pragma once
#include "stdafx.h"
#include "RemoteFetchInterface.h"
#include "Pegrpapi.h"
#include <vector>
#include <unordered_map>
#include "StrategyData.h"
#include <map>
#include <memory>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <thread>
#include <future>
#include <mutex>
#include <deque>
#include "RequiryDataCallbackInterface.h"
using namespace std::tr1;
using namespace boost::posix_time;
using namespace std;
using namespace StrategyData;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio;
class CFloatingProfitDoc;
class CFloatingProfitView : public CView,public MRequiryDataCallbackInterface
{
public:
    DECLARE_DYNCREATE(CFloatingProfitView)
    CFloatingProfitView();
    virtual ~CFloatingProfitView();
    int m_intRealTimeCounter = 0;
    CFloatingProfitDoc* GetDocument() const;
    //std::mutex m_mtxhPE;
    HWND m_hPE = nullptr;
    bool m_boolTimmerRunning = true;
    int m_intFloatingProfitRightLimit = 0;
#pragma region 基本信息
    
    string m_strIp;
    unsigned int m_uPort;
    string m_strName;
    list<tuple<string, string,string> > m_listStrategies;
#pragma endregion
    std::mutex m_mtxCallbackMtx;
    
    unsigned int m_uSerialCnt = 0;
    void SendReqGetFloatingProfit();

    std::shared_ptr<float> m_YData;
    TM m_tmXData;
    double NULLDATAVALUE = 0;
    bool InitChartCtrl();
    virtual void OnQuiry(ptree &,TCustomParam);
    virtual void OnDraw(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    void ShowMessage(string msg);
    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
protected:
    afx_msg LRESULT OnMsgFloatingProfitViewUpdate(WPARAM wParam, LPARAM lParam);

};
#ifndef _DEBUG
inline CFloatingProfitDoc* CFloatingProfitView::GetDocument() const
   { return reinterpret_cast<CFloatingProfitDoc*>(m_pDocument); }
#endif

