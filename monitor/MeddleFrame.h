#pragma once
#include "afxmdichildwndex.h"
#include <string>
#include <boost/thread.hpp>
#include <thread>                // std::thread
#include <mutex>                // std::mutex, std::unique_lock
#include <condition_variable>    // std::condition_variable
#include <atomic>
#include <boost/asio.hpp>
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <thread>
#include <future>
#include <tuple>
using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio;
using namespace std;
class CMeddleFrame :
    public CMDIChildWndEx
{
    DECLARE_DYNCREATE(CMeddleFrame)
public:
    CMeddleFrame();
    ~CMeddleFrame();
    string m_strStrategyID;
    string m_strIp;
    unsigned int m_uPort;
#pragma region ¶¨Ê±Æ÷ÊôÐÔ
    io_service  m_IOservice;
    deadline_timer m_StartAndStopCtrlTimer;
    std::future<bool> m_futTimerThreadFuture;
#pragma endregion

public:
    
    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

