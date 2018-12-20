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

// ChildFrm.h : CFloatingProfitFrame 类的接口
//

#pragma once
#include <string>
#include <boost/thread.hpp>
#include <thread>                // std::thread
#include <mutex>                // std::mutex, std::unique_lock
#include <condition_variable>    // std::condition_variable
#include <atomic>
using namespace std;
class CFloatingProfitFrame : public CMDIChildWndEx
{
    DECLARE_DYNCREATE(CFloatingProfitFrame)
public:
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    CFloatingProfitFrame();
    virtual ~CFloatingProfitFrame();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    DECLARE_MESSAGE_MAP()
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
    afx_msg void OnClose();
};
