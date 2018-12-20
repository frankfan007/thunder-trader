// DlgDeployNewStrategy.cpp : 实现文件
//

#include "stdafx.h"
#include "Monitor.h"
#include "DlgDeployNewStrategy.h"
#include "afxdialogex.h"
#include <sstream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <future>
#pragma region ptree
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;
#pragma endregion
using namespace std;
void Communicate(const char * address, unsigned int port, const std::stringstream & in, std::stringstream & out);
// CDlgDeployNewStrategy 对话框
boost::property_tree::ptree CDlgDeployNewStrategy::s_Result;
vector< pair<string, map<string, string> > > CDlgDeployNewStrategy::s_vecParams;
IMPLEMENT_DYNAMIC(CDlgDeployNewStrategy, CDialogEx)

CDlgDeployNewStrategy::CDlgDeployNewStrategy(
    string _ip, unsigned int _port,
    CWnd* pParent)
    : CDialogEx(IDD_DIALOG_DEPLOY_STRATEGY, pParent),
    m_strIP(_ip), m_uPort(_port)
{

}

CDlgDeployNewStrategy::~CDlgDeployNewStrategy()
{
}

void CDlgDeployNewStrategy::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_DEPLOY_STRATEGY_STRATEGY_BIN, m_combStrategyBin);
    DDX_Control(pDX, IDC_MFCPROPERTYGRID_DEPLOY_STRATEGY_DATAID, m_propDataidConfig);
    DDX_Control(pDX, IDC_MFCPROPERTYGRID_DEPLOY_STRATEGY_PARAM, m_propParamConfig);
    DDX_Control(pDX, IDC_COMBO_DEPLOY_STRATEGY_ARCHIVE_PATH, m_combStrategyArchives);
    DDX_Control(pDX, IDC_EDIT_DEPLOY_STRATEGY_STRATEGY_COMMENT, m_editStrategyComment);
    DDX_Control(pDX, IDC_EDIT_DEPLOY_STRATEGY_MAX_TICKETS_PER_DAY, m_editMaxTicketsCountPerDay);
    DDX_Control(pDX, IDC_EDIT1, m_editBatchDeployCsvFile);
    DDX_Control(pDX, IDC_CHECK_IS_BATCH_DEPLOY, m_checkBatchDeploy);
    DDX_Control(pDX, IDC_BUTTON_SELECT_BATCH_CSV_FILE, m_butSelectBatchCsvFile);
}

BEGIN_MESSAGE_MAP(CDlgDeployNewStrategy, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_DEPLOY_STRATEGY_ADD_DATAID, &CDlgDeployNewStrategy::OnBnClickedButtonDeployStrategyAddDataid)
    ON_BN_CLICKED(IDOK, &CDlgDeployNewStrategy::OnBnClickedOk)
    ON_MESSAGE(MSG_STRATEGY_BIN_CHANGED, &CDlgDeployNewStrategy::OnMsgStrategyBinChanged)
    ON_MESSAGE(MSG_ARCHIVEFILE_CHANGED, &CDlgDeployNewStrategy::OnMsgArchiveFileChanged)
    ON_BN_CLICKED(IDC_CHECK_IS_BATCH_DEPLOY, &CDlgDeployNewStrategy::OnBnClickedCheckIsBatchDeploy)
    ON_BN_CLICKED(IDC_BUTTON_SELECT_BATCH_CSV_FILE, &CDlgDeployNewStrategy::OnBnClickedButtonSelectBatchCsvFile)
END_MESSAGE_MAP()

void split(char * str, char ch, vector<string> & out)
{
    if (nullptr == str)
        return;
    out.clear();
    char * p = str;
    string this_string = "";
    while (*p)
    {
        if (*p == ch)
        {
            out.push_back(this_string);
            this_string = "";

        }
        else
            this_string += *p;
        ++p;
    }
    if (this_string.empty() == false)
        out.push_back(this_string);
}

BOOL CDlgDeployNewStrategy::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_editBatchDeployCsvFile.EnableWindow(FALSE);
    m_butSelectBatchCsvFile.EnableWindow(FALSE);
    m_checkBatchDeploy.SetCheck(FALSE);
    {
        try {
            boost::property_tree::ptree root;
            string fullpath = boost::filesystem::initial_path<boost::filesystem::path>().string();
            string ConfigName = fullpath + "\\Monitor.json";
            boost::property_tree::read_json(ConfigName, root);
            auto Instrument = root.get_child("SymbolDefine");
            for (auto & key : Instrument)
                m_vecInstrumentID.push_back(CString(CA2W(key.first.c_str())));
        }
        catch (std::exception & err)
        {
            AfxMessageBox(CA2W(err.what()));
        }
    }
    {//获取所有策略bin文件
        std::stringstream in, out;
        boost::property_tree::ptree root, result;
        root.put("type", "reqallstrategybin");
        try {
            boost::property_tree::write_json(in, root);
            Communicate(m_strIP.c_str(), m_uPort, in, out);
            boost::property_tree::read_json(out, result);
        }
        catch (std::exception & err)
        {
            return FALSE;
        }
        for (auto & node : result)
        {
            auto StrategyBin = CString(CA2W(node.second.data().c_str()));
            m_combStrategyBin.AddString(StrategyBin);
        }
    }
    {//获取所有行情源
        std::stringstream in, out;
        boost::property_tree::ptree root, result;
        root.put("type", "reqgetallmarketdatasource");
        try {
            boost::property_tree::write_json(in, root);
            Communicate(m_strIP.c_str(), m_uPort, in, out);
            boost::property_tree::read_json(out, result);
        }
        catch (std::exception & err)
        {
            return FALSE;
        }
        for (auto & node : result)
        {
            auto MD = CString(CA2W(node.first.c_str()));
            m_vecMDSource.push_back(MD);
        }
    }
    {//获取所有交易源
        std::stringstream in, out;
        boost::property_tree::ptree root, result;
        root.put("type", "reqgetalltradesource");
        try {
            boost::property_tree::write_json(in, root);
            Communicate(m_strIP.c_str(), m_uPort, in, out);
            boost::property_tree::read_json(out, result);
        }
        catch (std::exception & err)
        {
            return FALSE;
        }
        for (auto & node : result)
        {
            auto TD = CString(CA2W(node.first.c_str()));
            m_vecTDSource.push_back(TD);
        }
    }

    CRect rct;
    HDITEM item;
    {
        m_propDataidConfig.GetClientRect(rct);
        item.cxy = rct.Width() / 2;
        item.mask = HDI_WIDTH;
        m_propDataidConfig.GetHeaderCtrl().SetItem(0, new HDITEM(item));
        m_propDataidConfig.EnableHeaderCtrl(FALSE);//标头
        m_propDataidConfig.SetVSDotNetLook();//
    }
    {
        m_propParamConfig.GetClientRect(rct);
        item.cxy = rct.Width() / 2;
        item.mask = HDI_WIDTH;
        m_propParamConfig.GetHeaderCtrl().SetItem(0, new HDITEM(item));
        m_propParamConfig.EnableHeaderCtrl(FALSE);//标头
        m_propParamConfig.SetVSDotNetLook();//
    }
                                           
    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CDlgDeployNewStrategy::OnBnClickedButtonDeployStrategyAddDataid()
{
    auto Dataid=m_propDataidConfig.GetPropertyCount();
    CString DataidStr;
    DataidStr.Format(L"%d", Dataid);
    auto prop = new CMFCPropertyGridProperty(DataidStr);
    {
        CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(
            _T("合约"),
            m_vecInstrumentID.empty() ? L"" : m_vecInstrumentID[0],
            _T("请选择合约名称"));
        for (auto & ins : m_vecInstrumentID)
            pProp->AddOption(ins);
        pProp->AllowEdit(FALSE);
        prop->AddSubItem(pProp);
    }
    {
        CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(
            _T("行情源"),
            m_vecMDSource.empty() ? L"" : m_vecMDSource[0],
            _T("请选择行情源"));
        for (auto & ins : m_vecMDSource)
            pProp->AddOption(ins);
        pProp->AllowEdit(FALSE);
        prop->AddSubItem(pProp);
    }
    {
        CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(
            _T("交易源"),
            m_vecTDSource.empty() ? L"" : m_vecTDSource[0],
            _T("请选择交易源"));
        for (auto & ins : m_vecTDSource)
            pProp->AddOption(ins);
        pProp->AllowEdit(FALSE);
        prop->AddSubItem(pProp);
    }
    m_propDataidConfig.AddProperty(prop);
}

void CDlgDeployNewStrategy::OnBnClickedOk()
{
    stringstream ss;
    string a;
    CString temp;

    s_vecParams.clear();
    if (m_checkBatchDeploy.GetCheck())
    {
        CString filename;
        char buf[2048];
        m_editBatchDeployCsvFile.GetWindowTextW(filename);
        std::shared_ptr<ifstream> csv_file(new ifstream(string(CW2A(filename)), ios::in),
            [](ifstream*p) 
        {
            if (p) {
                if (p->is_open())
                    p->close();
                delete p;
        }});
        if (csv_file->is_open())
        {
            csv_file->getline(buf, sizeof(buf));
            vector<string> cols;
            split(buf, ',', cols);
            if (cols.size() < 1)
            {
                AfxMessageBox(L"参数文件必须包含comment列");
                return;
            }
            vector<string> values;
            while (csv_file->getline(buf, sizeof(buf)))
            {
                split(buf, ',', values);
                if(values.size()!=cols.size())
                {
                    AfxMessageBox(L"数据列数与表头列数不一致");
                    return;
                }
                string comment = values[0];
                s_vecParams.push_back(make_pair(comment, map<string, string>()));
                for (size_t i = 1;i < values.size();i++)
                    s_vecParams[s_vecParams.size() - 1].second[cols[i]] = values[i];
            }

        }
        else
        {
            AfxMessageBox(L"打开参数配置csv文件错误!");
            return;
        }
    }
    m_editStrategyComment.GetWindowTextW(temp);
    if (temp.IsEmpty())
    {
        AfxMessageBox(L"策略注释不能为空");
        return;
    }
    m_editMaxTicketsCountPerDay.GetWindowTextW(temp);
    if (temp.IsEmpty())
    {
        AfxMessageBox(L"单日最大报单门票不能为空");
        return;
    }


    s_Result.clear();
    
    s_Result.put("type", "reqdeploynewstrategy");
    m_combStrategyBin.GetWindowTextW(temp);
    s_Result.put("bin",string(CW2A(temp)));
    m_combStrategyArchives.GetWindowTextW(temp);
    s_Result.put("archive", string(CW2A(temp)));
    m_editMaxTicketsCountPerDay.GetWindowTextW(temp);
    if (temp.IsEmpty() == false)
    {
        unsigned int i = _ttoi(temp);
        if (i!=0)
            s_Result.put("maxincreaseordercountperday", i);
    }
    if (m_propParamConfig.GetPropertyCount() != 0)
    {
        boost::property_tree::ptree params;
        for (auto i = 0;i < m_propParamConfig.GetPropertyCount();i++)
        {
            auto prop = m_propParamConfig.GetProperty(i);
            params.put(string(CW2A(prop->GetName())), string(CW2A(CString(prop->GetValue()))));
        }
        s_Result.put_child("param", params);
    }
    
    boost::property_tree::ptree DataChannel;
    for (auto i = 0;i < m_propDataidConfig.GetPropertyCount();i++)
    {
        boost::property_tree::ptree PerDataIDConfig;
        auto Dataid = string(CW2A(m_propDataidConfig.GetProperty(i)->GetName()));
        auto didSubItem = m_propDataidConfig.GetProperty(i);
        for (auto t = 0;t < didSubItem->GetSubItemsCount();t++)
        {
            CString ItemName = didSubItem->GetSubItem(t)->GetName();
            CString ItemValue = didSubItem->GetSubItem(t)->GetValue();
            if (ItemName == L"合约")
            {
                try{
                    boost::property_tree::ptree root;
                    string fullpath = boost::filesystem::initial_path<boost::filesystem::path>().string();
                    string ConfigName = fullpath + "\\Monitor.json";
                    boost::property_tree::read_json(ConfigName, root);
                    auto Instrument = root.get_child("SymbolDefine");
                    auto tar_insnode = Instrument.find(string(CW2A(ItemValue)));
                    if (tar_insnode != Instrument.not_found())
                        PerDataIDConfig.put_child("symboldefine", tar_insnode->second);
                    else
                    {
                        string exp = "Could not find the config of " + string(CW2A(ItemValue));
                        throw std::exception(exp.c_str());
                    }
                }
                catch (std::exception & err)
                {
                    AfxMessageBox(CA2W(err.what()));
                    return;
                }
                
            }
            else if(ItemName == L"行情源")
                PerDataIDConfig.put("marketdatasource", string(CW2A(ItemValue)));
            else if (ItemName == L"交易源")
                PerDataIDConfig.put("tradesource", string(CW2A(ItemValue)));
        }
        DataChannel.put_child(Dataid, PerDataIDConfig);
    }
    s_Result.put_child("dataid", DataChannel);
    
    m_editStrategyComment.GetWindowTextW(temp);
    s_Result.put("comment", string(CW2A(temp)));
    CDialogEx::OnOK();
}

BEGIN_MESSAGE_MAP(CMyStrategyBinCombo, CComboBox)
    ON_CONTROL_REFLECT(CBN_SELENDCANCEL, &CMyStrategyBinCombo::OnCbnSelendcancel)
END_MESSAGE_MAP()

void CMyStrategyBinCombo::OnCbnSelendcancel()
{
    GetParent()->SendMessage(MSG_STRATEGY_BIN_CHANGED, NULL, NULL);
}

BEGIN_MESSAGE_MAP(CMyArchiveFileCombo, CComboBox)
    ON_CONTROL_REFLECT(CBN_SELENDCANCEL, &CMyArchiveFileCombo::OnCbnSelendcancel)
END_MESSAGE_MAP()

void CMyArchiveFileCombo::OnCbnSelendcancel()
{
    GetParent()->SendMessage(MSG_ARCHIVEFILE_CHANGED, NULL, NULL);
}

afx_msg LRESULT CDlgDeployNewStrategy::OnMsgStrategyBinChanged(WPARAM wParam, LPARAM lParam)
{
    CString StrategyBin;
    m_combStrategyBin.GetWindowTextW(StrategyBin);
    m_combStrategyArchives.SetWindowTextW(L"");
    m_combStrategyArchives.Clear();
    {
        std::stringstream in, out;
        boost::property_tree::ptree root, result;
        root.put("type", "reqstrategyparams");
        root.put("strategybin", string(CW2A(StrategyBin)));
        try {
            boost::property_tree::write_json(in, root);
            Communicate(m_strIP.c_str(), m_uPort, in, out);
            boost::property_tree::read_json(out, result);
        }
        catch (std::exception & err)
        {
            return FALSE;
        }
        m_propParamConfig.RemoveAll();
        for (auto & node : result)
        {
            auto Parname = CString(CA2W(node.first.c_str()));
            auto Parval = CString(CA2W(node.second.data().c_str()));
            auto prop = new CMFCPropertyGridProperty(Parname, Parval);
            m_propParamConfig.AddProperty(prop);
        }
    }
    {
        std::stringstream in, out;
        boost::property_tree::ptree root, result;
        root.put("type", "reqallarchivefile");
        root.put("strategyname", string(CW2A(StrategyBin)));
        try {
            boost::property_tree::write_json(in, root);
            Communicate(m_strIP.c_str(), m_uPort, in, out);
            boost::property_tree::read_json(out, result);
//#ifdef _DEBUG
//            stringstream ss;
//            write_json(ss, result);
//            string debug = ss.str();
//#endif
        }
        catch (std::exception & err)
        {
            return FALSE;
        }
        m_combStrategyArchives.Clear();
        m_combStrategyArchives.AddString(L"");
        for (auto & node : result)
        {
            auto Parname = CString(CA2W(node.second.data().c_str()));
            m_combStrategyArchives.AddString(Parname);
        }
    }
    return 0;
}

afx_msg LRESULT CDlgDeployNewStrategy::OnMsgArchiveFileChanged(WPARAM wParam, LPARAM lParam)
{
    CString StrategyBin,ArchiveFile;
    m_combStrategyBin.GetWindowTextW(StrategyBin);
    m_combStrategyArchives.GetWindowTextW(ArchiveFile);
    {
        std::stringstream in, out;
        boost::property_tree::ptree root, result;
        root.put("type", "reqstrategyparams");
        root.put("strategybin", string(CW2A(StrategyBin)));
        if(ArchiveFile.GetLength()!=0)
            root.put("archivefile", string(CW2A(ArchiveFile)));
        try {
            boost::property_tree::write_json(in, root);
            Communicate(m_strIP.c_str(), m_uPort, in, out);
            boost::property_tree::read_json(out, result);
        }
        catch (std::exception & err)
        {
            return FALSE;
        }
        m_propParamConfig.RemoveAll();
        for (auto & node : result)
        {
            auto Parname = CString(CA2W(node.first.c_str()));
            auto Parval = CString(CA2W(node.second.data().c_str()));
            auto prop = new CMFCPropertyGridProperty(Parname, Parval);
            m_propParamConfig.AddProperty(prop);
        }
    }
    return 0;
}

void CDlgDeployNewStrategy::OnBnClickedCheckIsBatchDeploy()
{
    if (m_checkBatchDeploy.GetCheck())
    {
        m_editBatchDeployCsvFile.EnableWindow(TRUE);
        m_butSelectBatchCsvFile.EnableWindow(TRUE);
        
    }
    else
    {
        m_editBatchDeployCsvFile.SetWindowTextW(L"");
        m_editBatchDeployCsvFile.EnableWindow(FALSE);
        m_butSelectBatchCsvFile.EnableWindow(FALSE);
    }
}

void CDlgDeployNewStrategy::OnBnClickedButtonSelectBatchCsvFile()
{
    CString FilePathName;
    CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
    if (dlg.DoModal() == IDOK)
        FilePathName = dlg.GetPathName();
    else
        return;
    if (FilePathName.Right(4) != L".csv")
    {
        AfxMessageBox(L"请选择正确的策略csv文件");
        return;
    }
    m_editBatchDeployCsvFile.SetWindowTextW(FilePathName);

}
