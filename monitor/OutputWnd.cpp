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

#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_MESSAGE(MSG_OUTPUT_WND_DISPLAY_MESSAGE, &COutputWnd::OnMsgOutputWndDisplayMessage)
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // 创建选项卡窗口: 
    const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

    if (!m_wndOutput.Create(dwStyle, rectDummy, this, 0))
    {
        TRACE0("未能创建输出窗口\n");
        return -1;      // 未能创建
    }

    UpdateFonts();
    {
#define LIST m_wndOutput
        LIST.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 100);// 插入列 
        LIST.InsertColumn(1, _T("时间"), LVCFMT_LEFT, 180);// 插入列 
        LIST.InsertColumn(2, _T("消息内容"), LVCFMT_LEFT, 1000);// 插入列 
        LONG lStyle;
        lStyle = GetWindowLong(LIST.m_hWnd, GWL_STYLE);// 获取当前窗口style 
        lStyle &= ~LVS_TYPEMASK; // 清除显示方式位 
        lStyle |= LVS_REPORT; // 设置style 
        SetWindowLong(LIST.m_hWnd, GWL_STYLE, lStyle);// 设置style 
        DWORD dwStyle = LIST.GetExtendedStyle();
        dwStyle |= LVS_EX_FULLROWSELECT;// 选中某行使整行高亮（只适用与report 风格的listctrl ） 
        dwStyle |= LVS_EX_GRIDLINES;// 网格线（只适用与report 风格的listctrl ） 
        LIST.SetExtendedStyle(dwStyle); // 设置扩展风格 
    }
    return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    // 选项卡控件应覆盖整个工作区: 
    m_wndOutput.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
    CClientDC dc(this);
    CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

    int cxExtentMax = 0;

    for (int i = 0; i < wndListBox.GetCount(); i ++)
    {
        CString strItem;
        wndListBox.GetText(i, strItem);

        cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
    }

    wndListBox.SetHorizontalExtent(cxExtentMax);
    dc.SelectObject(pOldFont);
}

BOOL COutputWnd::CanBeClosed() const
{
    return FALSE;
}
void COutputWnd::UpdateFonts()
{
    m_wndOutput.SetFont(&afxGlobalData.fontRegular);
}


afx_msg LRESULT COutputWnd::OnMsgOutputWndDisplayMessage(WPARAM wParam, LPARAM lParam)
{
    CString * msg = reinterpret_cast<CString*>(wParam);


    SYSTEMTIME st;
    GetLocalTime(&st);
    CString _time;
    _time.Format(L"%d-%d-%d %d:%d:%d.%d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    CString ID;
    int cnt = this->m_wndOutput.GetItemCount();
    ID.Format(_T("%d"), cnt);
    int nRow = m_wndOutput.InsertItem(0, ID);
    m_wndOutput.SetItemText(nRow, 1, _time);
    m_wndOutput.SetItemText(nRow, 2, *msg);
    delete msg;

    return 0;
}
