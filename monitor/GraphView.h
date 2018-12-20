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

// GraphView.h : CGraphView 类的接口
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
class CGraphDoc;
class CGraphView : public CView,public MRequiryDataCallbackInterface
{
public:
    DECLARE_DYNCREATE(CGraphView)
    const unordered_map<string, COLORREF> m_mapColorMap =
    {
        { "0",PERGB(255, 255, 0, 0) },
        { "1",PERGB(255, 0, 255, 0) },
        { "2",PERGB(255, 0, 0, 255) },
        { "3",PERGB(255, 255, 255, 0) },
        { "4",PERGB(255, 255, 0, 0) },
        { "5",PERGB(255, 255, 69, 0) },
        { "6",PERGB(255, 253, 78, 113) },
        { "7",PERGB(255, 65, 105, 225) },
        { "8",PERGB(255, 176, 224, 230) },
        { "9",PERGB(100, 255, 0, 0) },
        { "10",PERGB(100, 0, 255, 0) },
        { "11",PERGB(100, 0, 0, 255) },
        { "12",PERGB(100, 255, 255, 0) },
        { "13",PERGB(100, 255, 0, 0) },
        { "14",PERGB(100, 255, 69, 0) },
        { "15",PERGB(100, 253, 78, 113) },
        { "16",PERGB(100, 65, 105, 225) },
        { "17",PERGB(100, 176, 224, 230) },
    };
    CGraphView();
    virtual ~CGraphView();
    int m_intRealTimeCounter = 0;
    vector< tuple< std::unique_ptr< CButton>,CString,string> > m_vecButtons;
    CGraphDoc* GetDocument() const;
    //std::mutex m_mtxhPE;
    HWND m_hPE = nullptr;
    bool m_boolTimmerRunning = true;
    vector<wstring> m_strSerialNames;
    int m_intGraphRightLimit = 0;
#pragma region 基本信息
    double NULLDATAVALUE = 0;
    string m_strIp;
    unsigned int m_uPort;
    string m_strName;
    list<tuple<string, string,string> > m_listStrategies;
#pragma endregion
    ptree m_treeInitTree;
    //bool m_boolHasCustomButton = false;
    bool m_boolIsShowCustomButton = true;
    std::mutex m_mtxCallbackMtx;
    void ResizeViewLayout(int cx, int cy);
    unsigned int m_uSerialCnt = 0;
    void SendReqGetProbe(TCustomParam cus);

    std::shared_ptr<float> m_YData;
    TM m_tmXData;

    void Ptime2TM(const ptime & CurrentTime, TM & _tmXData);
    void UintLeast642TM(const uint_least64_t & CurrentTime, TM & _tmXData);

    bool InitChartCtrl(ptree&);
    virtual void OnQuiry(ptree &,TCustomParam);
    virtual void OnDraw(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    void ShowMessage(string msg);
    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnCustomButtonClick(UINT uID);
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
protected:
    afx_msg LRESULT OnMsgGraphViewUpdate(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnButtonResetProbeGraph();
protected:
    afx_msg LRESULT OnMsgResetProbeGraph(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMsgInitTreeHasFetched(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMsgShowHideCustomButton(WPARAM wParam, LPARAM lParam);
};
#ifndef _DEBUG
inline CGraphDoc* CGraphView::GetDocument() const
   { return reinterpret_cast<CGraphDoc*>(m_pDocument); }
#endif

