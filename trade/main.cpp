#include <string>
#include <sys/stat.h>
#include <iostream>
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <stdexcept>
#include <string>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_multifile_backend.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include "OrderRefResolve.h"
#include <boost/log/trivial.hpp>  
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/named_scope.hpp>



#include "public.h"
namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
using namespace boost::posix_time;
using namespace boost::gregorian;


#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;

#include "trade_service.h"
#include "CommuModForServInterface.h"
using namespace std;
// g++ -D BOOST_SPIRIT_THREADSAFE

char ProcessName[256];

void InitLog(std::string configFile)
{
    ptree g_Config;
    boost::property_tree::read_json(configFile, g_Config);
    

    auto LogConfig = g_Config.find("logconfig");
    if (LogConfig != g_Config.not_found())
    {
        auto LogtypeNode = LogConfig->second.find("logtype");
        if(LogtypeNode== LogConfig->second.not_found())
            throw std::runtime_error("[error]Can not find 'LogConfig.Logtype' Value.");
        string LogTypeString = LogtypeNode->second.data();
        if (LogTypeString.find("syslog")!= std::string::npos)
        {
            string _ServerAddress="";
            unsigned short _Port = 0;
            if (LogConfig->second.find("syslog") != LogConfig->second.not_found())
            {
                auto Syslog = LogConfig->second.find("syslog");
                if (Syslog->second.find("serveraddress") != Syslog->second.not_found())
                    _ServerAddress = Syslog->second.find("serveraddress")->second.data();
                else
                    throw std::runtime_error("[error]invalid 'logconfig.syslog.serveraddress' value.");
                if (Syslog->second.find("port") != Syslog->second.not_found())
                    _Port = atoi(Syslog->second.find("port")->second.data().c_str());
                else
                    throw std::runtime_error("[error]invalid 'logconfig.syslog.port' value.");
            }
            else
                throw std::runtime_error("[error]could not find 'logconfig.syslog' node.");
            boost::shared_ptr< sinks::synchronous_sink< sinks::syslog_backend > > sink(new sinks::synchronous_sink< sinks::syslog_backend >());
            sinks::syslog::custom_severity_mapping< severity_levels > mapping("Severity");
            mapping[severity_levels::normal] = sinks::syslog::info;
            mapping[severity_levels::warning] = sinks::syslog::warning;
            mapping[severity_levels::error] = sinks::syslog::critical;
            sink->locked_backend()->set_severity_mapper(mapping);
            sink->locked_backend()->set_target_address(_ServerAddress, _Port);
            logging::core::get()->add_sink(sink);
        }

        if (LogTypeString.find("file") != std::string::npos)
        {

            string LogFileHead = "_";
            int RotationSize = 1024;
            bool AutoFlush = true;
            if (LogConfig->second.find("file") != LogConfig->second.not_found())
            {
                auto FileLogNode = LogConfig->second.find("file");
                if (FileLogNode->second.find("filenamehead") != FileLogNode->second.not_found())
                    LogFileHead = FileLogNode->second.find("filenamehead")->second.data();

                if (FileLogNode->second.find("rotationsize") != FileLogNode->second.not_found())
                    RotationSize = atoi(FileLogNode->second.find("rotationsize")->second.data().c_str());

                if (FileLogNode->second.find("autoflush") != FileLogNode->second.not_found())
                    AutoFlush = FileLogNode->second.find("autoflush")->second.data()=="true"? true : false;
            }
            else
                throw std::runtime_error("[error]could not find 'logconfig.file' node.");
            typedef sinks::synchronous_sink<sinks::text_file_backend> TextSink;
            boost::shared_ptr<sinks::text_file_backend> backend1 = boost::make_shared<sinks::text_file_backend>(
                keywords::file_name = LogFileHead+"%Y-%m-%d_%H-%M-%S.%N.log",
                keywords::rotation_size = RotationSize,
                keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
                keywords::min_free_space = 30 * 1024 * 1024);
            
            backend1->auto_flush(AutoFlush);
            boost::shared_ptr<TextSink> sink(new TextSink(backend1));
            //sink->set_formatter(formatter);
            logging::core::get()->add_sink(sink);
        }

        if (LogTypeString.find("console") != std::string::npos)
        {
            auto console_sink = logging::add_console_log();
            //console_sink->set_formatter(formatter);
            logging::core::get()->add_sink(console_sink);
        }
    }
    else
        throw std::runtime_error("[error]could not find 'logconfig' node.");

}

int main(int argc,char *argv[]) {
    if (argc < 3)
    {
        cout<<"Usage:./thunder-trader %ConfigFileName%.json sysNumber [daemon]"<<endl;
        return 0;
    }
    if (argc > 3 && argv[3] && strcmp(argv[3], "daemon") == 0)
    {
        pid_t pc = fork();
        if (pc < 0) {
            printf("error fork\n");
            exit(1);
        }
        else if (pc>0)
            exit(0);
        setsid();
        cout<<chdir(".")<<endl;;
        umask(0);

    }    
    try
    {
        strncpy(ProcessName, argv[0], sizeof(ProcessName));
        if(atoi(argv[2])>_MaxAccountNumber)
            throw std::runtime_error("sysNumber error");
        InitLog(argv[1]);
        boost::log::sources::severity_logger< severity_levels > m_Logger;
        BOOST_LOG_SEV(m_Logger, normal) << "Run.SystemNumber="<< argv[2] << " [" << to_iso_string(microsec_clock::universal_time()) << "]";
        CTradeService service(argv[1],atoi(argv[2]));
        service.Start();
        service.Join();
    }
    catch (std::exception & exp)
    {
        std::cout << exp.what() << endl;
        return 1;
    }
    return 0;
}

