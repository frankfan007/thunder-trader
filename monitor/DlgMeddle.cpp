// DlgMeddle.cpp : 实现文件
//

#include "stdafx.h"
#include "Monitor.h"
#include "DlgMeddle.h"
#include "afxdialogex.h"
#include "resource.h"
#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
#include <boost/date_time/posix_time/posix_time.hpp>
#include "MainFrm.h"
using namespace std;
// CDlgMeddle 对话框
void Communicate(const char * address, unsigned int port, const std::stringstream & in, std::stringstream & out);
IMPLEMENT_DYNAMIC(CDlgMeddle, CDialogEx)

CDlgMeddle::CDlgMeddle(CWnd* pParent,
    string _strStrategyID,
    string _strIp,
    unsigned int _uPort,
    string _strName,
    string _strStrategyName,
    string _strStrategyComment)
    : CDialogEx(IDD_DIALOG_MEDDLE, pParent)
{
    _strStrategyID = _strStrategyID;
         _strIp = _strIp;
         _uPort = _uPort;
         _strName = _strName;
         _strStrategyName = _strStrategyName;
         _strStrategyComment = _strStrategyComment;
}

CDlgMeddle::~CDlgMeddle()
{
}

void CDlgMeddle::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_MEDDLE_COMMAND, m_editMeddleCommand);
    DDX_Control(pDX, IDC_LIST_MEDDLE_RESPONSE, m_listMeddleResponse);
    DDX_Control(pDX, IDC_STATIC_STRATEGY_LABEL, m_textLabel);
    DDX_Control(pDX, IDC_STATIC_COMMAND, m_textCommandLabel);
    DDX_Control(pDX, IDC_BUTTON_SEND_MEDDLE, m_butSendMeddle);
    DDX_Control(pDX, IDC_BUTTON_GET_MEDDLE_RESPONSE, m_butGetResponse);
}


BEGIN_MESSAGE_MAP(CDlgMeddle, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON_SEND_MEDDLE, &CDlgMeddle::OnBnClickedButtonSendMeddle)
    ON_BN_CLICKED(IDC_BUTTON_GET_MEDDLE_RESPONSE, &CDlgMeddle::OnBnClickedButtonGetMeddleResponse)
END_MESSAGE_MAP()


// CDlgMeddle 消息处理程序


void CDlgMeddle::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);
#define HIGTH 25
    if (IsWindow(m_textLabel))
        m_textLabel.SetWindowPos(NULL, 0, 0, cx, HIGTH,SWP_NOZORDER);
    if(IsWindow(m_textCommandLabel))
        m_textCommandLabel.SetWindowPos(NULL, 0, HIGTH+1, cx*0.1, HIGTH, SWP_NOZORDER);

    if (IsWindow(m_editMeddleCommand))
        m_editMeddleCommand.SetWindowPos(NULL, cx*0.1 + 1, HIGTH + 1, cx*0.6, HIGTH, SWP_NOZORDER);

    if (IsWindow(m_butSendMeddle))
        m_butSendMeddle.SetWindowPos(NULL, cx*0.7 + 2, HIGTH + 1, cx*0.1, HIGTH, SWP_NOZORDER);

    if (IsWindow(m_butGetResponse))
        m_butGetResponse.SetWindowPos(NULL, cx*0.8 + 3, HIGTH + 1, cx*0.1, HIGTH, SWP_NOZORDER);

    if (IsWindow(m_listMeddleResponse))
        m_listMeddleResponse.SetWindowPos(NULL, 1, HIGTH * 2 + 5,cx , cy- HIGTH * 2-4, SWP_NOZORDER);
    // TODO: 在此处添加消息处理程序代码
}


void CDlgMeddle::OnBnClickedButtonSendMeddle()
{
    CString commandW;
    m_editMeddleCommand.GetWindowTextW(commandW);
    string command = CW2A(commandW);
    boost::property_tree::ptree RequireMeddleFormat;
    RequireMeddleFormat.put("type", "reqmeddle");
    RequireMeddleFormat.put("strategyid", "");
    RequireMeddleFormat.put("command", command);
    for (auto & str : m_listStrategies)
    {
        boost::property_tree::ptree ResultMeddleFormat;
        std::stringstream in,out;
        RequireMeddleFormat.find("strategyid")->second.data() = get<0>(str);
        try {
            boost::property_tree::write_json(in, RequireMeddleFormat);
            Communicate(m_strIp.c_str(), m_uPort, in, out);
            boost::property_tree::read_json(out, ResultMeddleFormat);
        }
        catch (std::exception & err)
        {
            AppendOutput(get<0>(str),err.what());
        }

        auto ResultNode = ResultMeddleFormat.find("result");
        if (ResultNode != ResultMeddleFormat.not_found())
            AppendOutput(get<0>(str), ResultNode->second.data());
        else
            AppendOutput(get<0>(str),"Can not find <result>");
    }
}


void CDlgMeddle::OnBnClickedButtonGetMeddleResponse()
{
    CString commandW;
    m_editMeddleCommand.GetWindowTextW(commandW);
    string command = CW2A(commandW);
    boost::property_tree::ptree RequireMeddleFormat;
    RequireMeddleFormat.put("type", "reqgetmeddleresponse");
    RequireMeddleFormat.put("strategyid", "");
    for (auto &str : m_listStrategies)
    {
        boost::property_tree::ptree ResultMeddleFormat;
        std::stringstream in, out;
        RequireMeddleFormat.find("strategyid")->second.data() = get<0>(str);
        try {
            boost::property_tree::write_json(in, RequireMeddleFormat);
            Communicate(m_strIp.c_str(), m_uPort, in, out);
            boost::property_tree::read_json(out, ResultMeddleFormat);
        }
        catch (std::exception & err)
        {
            
            AppendOutput(get<0>(str),err.what());
            
        }
        
        for (auto & node : ResultMeddleFormat)
            AppendOutput(get<0>(str),node.second.data());
        
    }
}

void CDlgMeddle::AppendOutput(string sid,string response)
{
    
    int nRow = m_listMeddleResponse.InsertItem(m_listMeddleResponse.GetItemCount()+1, CA2W(sid.c_str()));
    m_listMeddleResponse.SetItemText(nRow, 1, CA2W(response.c_str()));
}

void CDlgMeddle::SetConfig(string _strIp,
    unsigned int _uPort,
    string _strName,
    list< tuple< string, string, string> > & _listStrategies)
{
    
    m_strIp= _strIp;
    m_uPort= _uPort;
    m_strName= _strName;
    m_listStrategies = _listStrategies;
    
    stringstream ss;
    ss << _strName << " ";
    for (auto & str : _listStrategies)
        ss << get<0>(str) << " ";
    m_textLabel.SetWindowTextW(CString(CA2W(ss.str().c_str())));

    {
        m_listMeddleResponse.InsertColumn(0, _T("策略ID"), LVCFMT_LEFT, 70);// 插入列 
        m_listMeddleResponse.InsertColumn(1, _T("消息"), LVCFMT_LEFT, 700);// 插入列 
            
        LONG lStyle;
        lStyle = GetWindowLong(m_listMeddleResponse.m_hWnd, GWL_STYLE);// 获取当前窗口style 
        lStyle &= ~LVS_TYPEMASK; // 清除显示方式位 
        lStyle |= LVS_REPORT; // 设置style 
        SetWindowLong(m_listMeddleResponse.m_hWnd, GWL_STYLE, lStyle);// 设置style 
        DWORD dwStyle2 = m_listMeddleResponse.GetExtendedStyle();
        dwStyle2 |= LVS_EX_FULLROWSELECT;// 选中某行使整行高亮（只适用与report 风格的listctrl ） 
        dwStyle2 |= LVS_EX_GRIDLINES;// 网格线（只适用与report 风格的listctrl ） 
        dwStyle2 |= LVS_EX_CHECKBOXES;//item 前生成checkbox 控件 
        m_listMeddleResponse.SetExtendedStyle(dwStyle2); // 设置扩展风格 
    }
    
}
