// DlgAddSource.cpp : 实现文件
//

#include "stdafx.h"
#include "Monitor.h"
#include "DlgAddSource.h"
#include "afxdialogex.h"
#include <vector>
using namespace std;

int split(const string& str, vector<string> & ret_, string sep)
{
    if (str.empty())
    {
        return 0;
    }

    string tmp;
    string::size_type pos_begin = str.find_first_not_of(sep);
    string::size_type comma_pos = 0;

    while (pos_begin != string::npos)
    {
        comma_pos = str.find(sep, pos_begin);
        if (comma_pos != string::npos)
        {
            tmp = str.substr(pos_begin, comma_pos - pos_begin);
            pos_begin = comma_pos + sep.length();
        }
        else
        {
            tmp = str.substr(pos_begin);
            pos_begin = comma_pos;
        }

        if (!tmp.empty())
        {
            ret_.push_back(tmp);
            tmp.clear();
        }
    }
    return 0;
}

// CDlgAddSource 对话框
BEGIN_MESSAGE_MAP(CMyCComboBox, CComboBox)
    ON_CONTROL_REFLECT(CBN_SELCHANGE, &CMyCComboBox::OnCbnSelchange)
    ON_CONTROL_REFLECT(CBN_KILLFOCUS, &CMyCComboBox::OnCbnKillfocus)
    ON_CONTROL_REFLECT(CBN_EDITUPDATE, &CMyCComboBox::OnCbnEditupdate)
    ON_CONTROL_REFLECT(CBN_SELENDCANCEL, &CMyCComboBox::OnCbnSelendcancel)
END_MESSAGE_MAP()


void CMyCComboBox::OnCbnKillfocus()
{
    
}


void CMyCComboBox::OnCbnEditupdate()
{
    
}

void CMyCComboBox::OnCbnSelchange()
{
    
}



void CMyCComboBox::OnCbnSelendcancel()
{
    GetParent()->SendMessage(MSG_ADD_SOURCE_DLG_COMB_SEL_CHANGED, NULL, NULL);
}
IMPLEMENT_DYNAMIC(CDlgAddSource, CDialogEx)

string CDlgAddSource::s_strSelectedType;
map<string, string> CDlgAddSource::s_mapPropValues;

CDlgAddSource::CDlgAddSource(map<string, string> & propNames,CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG_ADD_SOURCE, pParent), m_mapPropNames(propNames)
{

}

CDlgAddSource::~CDlgAddSource()
{
}

void CDlgAddSource::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_SOURCE_TYPE, m_combSourceType);
    DDX_Control(pDX, IDC_MFCPROPERTYGRID_SOURCE_PROP, m_propSourceProperties);
}


BEGIN_MESSAGE_MAP(CDlgAddSource, CDialogEx)
    ON_BN_CLICKED(IDOK, &CDlgAddSource::OnBnClickedOk)
    ON_MESSAGE(MSG_ADD_SOURCE_DLG_COMB_SEL_CHANGED, &CDlgAddSource::OnMsgAddSourceDlgCombSelChanged)
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgAddSource 消息处理程序


void CDlgAddSource::OnBnClickedOk()
{
    CString text;
    m_combSourceType.GetWindowTextW(text);
    s_mapPropValues.clear();
    map<string, string>(s_mapPropValues).swap(s_mapPropValues);

    s_strSelectedType = CW2A(text);
    for (auto i = 0;i < m_propSourceProperties.GetPropertyCount();i++)
    {
        string key=CW2A(m_propSourceProperties.GetProperty(i)->GetName());
        string value = CW2A(CString(m_propSourceProperties.GetProperty(i)->GetValue()));
        s_mapPropValues[key] = value;
    }
    CDialogEx::OnOK();
}



afx_msg LRESULT CDlgAddSource::OnMsgAddSourceDlgCombSelChanged(WPARAM wParam, LPARAM lParam)
{
    CString text;
    m_combSourceType.GetWindowTextW(text);

    string key = CW2A(text);
    vector<string> props;
    split(m_mapPropNames[key], props, ";");
    m_propSourceProperties.RemoveAll();
    for (auto key : props)
        m_propSourceProperties.AddProperty(new CMFCPropertyGridProperty(CString(CA2W(key.c_str())),_T("")));
    m_propSourceProperties.AdjustLayout();
    m_propSourceProperties.AdjustLayout();
    Invalidate();
    return 0;
}


BOOL CDlgAddSource::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    CRect rect;
    GetClientRect(rect);
    HDITEM item;
    item.cxy = rect.Width() / 2;
    item.mask = HDI_WIDTH;
    m_propSourceProperties.GetHeaderCtrl().SetItem(0, new HDITEM(item));
    for (auto & key : m_mapPropNames)
        m_combSourceType.AddString(CA2W(key.first.c_str()));
    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CDlgAddSource::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);
    if (IsWindow(m_propSourceProperties))
    {
        m_propSourceProperties.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
        HDITEM item;
        item.cxy = cx / 2;
        item.mask = HDI_WIDTH;
        m_propSourceProperties.GetHeaderCtrl().SetItem(0, new HDITEM(item));
    }
}
