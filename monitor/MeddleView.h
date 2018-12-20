#pragma once
#include "stdafx.h"
#include "afxwin.h"
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
#include "DlgMeddle.h"
#include <string>
using namespace std::tr1;
using namespace boost::posix_time;
using namespace std;
using namespace StrategyData;
class CMeddleView :
    public CView
{
    DECLARE_DYNCREATE(CMeddleView)
public:
    CDlgMeddle m_dlgMainUI;
    CMeddleView();
    CMeddleView(
        string _strStrategyID,
        string _strIp,
        unsigned int _uPort,
        string _strName,
        string _strStrategyName,
        string _strStrategyComment);
    ~CMeddleView();
    virtual void OnDraw(CDC* /*pDC*/);
    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    
};

