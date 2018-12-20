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

#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList 窗口

class COutputWnd : public CDockablePane
{
// 构造
public:
    COutputWnd();

    void UpdateFonts();

// 特性
protected:
    
    CListCtrl m_wndOutput;

protected:
    void AdjustHorzScroll(CListBox& wndListBox);

// 实现
public:
    virtual ~COutputWnd();
    BOOL CanBeClosed() const;
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnMsgOutputWndDisplayMessage(WPARAM wParam, LPARAM lParam);
};

