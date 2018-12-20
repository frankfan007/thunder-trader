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

// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
enum TViewUpdateType
{
	InitChart,
	AppendValue
};

#define MSG_CTRADESERVERPANEL_TAB_CHANGED (WM_USER + 1)
#define MSG_C_MAIN_FRAME_SHOW_MESSAGE (WM_USER + 2)
#define MSG_TREE_ITEM_DB_CLICK (WM_USER + 3)
#define MSG_PROP_ITEM_R_CLICK (WM_USER + 4)
#define MSG_ADD_SOURCE_DLG_COMB_SEL_CHANGED (WM_USER + 5)
#define MSG_OUTPUT_WND_DISPLAY_MESSAGE  (WM_USER + 6)
#define MSG_ON_UPDATE_VIEW  (WM_USER + 7)
#define MSG_ON_VIEW_INIT_FINISHED  (WM_USER + 8)
#define MSG_SEND_MEDDLE  (WM_USER + 9)
#define MSG_MEDDLE_RESPONSE  (WM_USER + 10)
#define MSG_STRATEGY_BIN_CHANGED  (WM_USER + 11)
#define MSG_ARCHIVEFILE_CHANGED  (WM_USER + 12)
#define MSG_DRAG_JSON_FILE (WM_USER + 13)
#define MSG_GRAPH_VIEW_UPDATE  (WM_USER + 14)
#define MSG_RESET_PROBE_GRAPH  (WM_USER + 15)
#define MSG_INIT_TREE_HAS_FETCHED   (WM_USER + 16)
#define MSG_SHOW_HIDE_CUSTOM_BUTTON   (WM_USER + 17)