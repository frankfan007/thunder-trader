project(auto_trade_machine)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-Bsymbolic")
    set(BOOST_THREAD "boost_thread-mt")
else()
    set(BOOST_THREAD "boost_thread")
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(CMAKE_BUILD_TYPE "Release")

include_directories("${PROJECT_SOURCE_DIR}/common")
include_directories("${PROJECT_SOURCE_DIR}/common/trade_headers")  
include_directories("${PROJECT_SOURCE_DIR}/trade/md_plugins")  
include_directories("${PROJECT_SOURCE_DIR}/trade/trade_plugins")  
include_directories("${PROJECT_SOURCE_DIR}/common/tick")  

set(libs dl boost_system boost_filesystem pthread boost_thread boost_date_time boost_serialization boost_log_setup boost_log)
add_definitions(-DBOOST_SPIRIT_THREADSAFE)
add_definitions(-DBOOST_ALL_DYN_LINK)

message(STATUS "${BOOST_THREAD}")
add_definitions(-DBOOST_SPIRIT_THREADSAFE)
find_package(Boost REQUIRED COMPONENTS regex system thread serialization date_time)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
if(NOT Boost_FOUND)
    message("Can not find Boost")
endif()
message(STATUS "Found Boost IncPath ${Boost_INCLUDE_DIRS}")
message(STATUS "Found Boost LibPath ${Boost_LIBRARY_DIRS}")
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_library(io_service_pool STATIC trade/io_service_pool.hpp trade/io_service_pool.cpp)
add_library(server STATIC  trade/server.hpp trade/server.cpp)
add_library(connection STATIC  trade/connection.hpp trade/connection.cpp)
add_library(auto_pend STATIC  common/AutoPend.cpp common/AutoPend.h)

####################### CTP ##########################################
#include_directories("${PROJECT_SOURCE_DIR}/third/CTPForLinux32/")
#
#add_definitions(-DCTP_FUTURE_MDPlugin)
#set(ctp_md_plugpath "${PROJECT_SOURCE_DIR}/trade/md_plugins/CTP_FUTURE_MDPlugin")
#include_directories("${ctp_md_plugpath}")
#add_library(ctp_future_mdplugin static ${ctp_md_plugpath}/CTP_FUTURE_MDPlugin.cpp)
#target_link_libraries(ctp_future_mdplugin "${PROJECT_SOURCE_DIR}/third/CTPForLinux32/thostmduserapi.so")
#
#add_definitions(-DCTP_FUTURE_TDPlugin)
#set(ctp_td_plugpath "${PROJECT_SOURCE_DIR}/trade/trade_plugins/CTP_FUTURE_TDPlugin")
#include_directories("${ctp_td_plugpath}")
#add_library(ctp_future_tdplugin static ${ctp_td_plugpath}/CTP_FUTURE_TDPlugin.cpp ${CTP_TD_PlugPath}/CTP_FUTURE_TDPlugin.h )
#target_link_libraries(ctp_future_tdplugin "${PROJECT_SOURCE_DIR}/third/CTPForLinux32/thosttraderapi.so"
#
######################## DFITC ########################################
#include_directories("${PROJECT_SOURCE_DIR}/third/DfitcForLinux64/")
#
#add_definitions(-DDFITC_SOP_MDPlugin)
#set(dfitc_sop_mdplugpath "${PROJECT_SOURCE_DIR}/trade/md_plugins/DFITC_SOP_MDPlugin")
#include_directories("${dfitc_sop_mdplugpath}")
#add_library(dfitc_sop_mdplugin static ${dfitc_sop_mdplugpath}/DFITC_SOP_MDPlugin.cpp)
#target_link_libraries(dfitc_sop_mdplugin "${project_source_dir}/third/DfitcForLinux64/libDFITCSECMdApi.so"
#
#add_definitions(-DDFITC_SOP_TDPlugin)
#set(dfitc_sop_tdplugpath "${PROJECT_SOURCE_DIR}/trade/trade_plugins/DFITC_SOP_TDPlugin")
#include_directories("${dfitc_sop_tdplugpath}")
#add_library(dfitc_sop_tdplugin static ${dfitc_sop_tdplugpath}/DFITC_SOP_TDPlugin.cpp)
#target_link_libraries(dfitc_sop_tdplugin "${project_source_dir}/third/DfitcForLinux64/libDFITCSECTraderApi.so"
#
####################### FEMAS ##########################################
#include_directories("${PROJECT_SOURCE_DIR}third/Femas/")
#
#add_definitions(-DFEMAS_FUTURE_MDPlugin) 
#set(femas_future_mdplugpath "${PROJECT_SOURCE_DIR}/trade/md_plugins/FEMAS_FUTURE_MDPlugin")
#include_directories("${femas_future_mdplugpath}")
#add_library(femas_future_mdplugin static ${femas_future_mdplugpath}/FEMAS_FUTURE_MDPlugin.cpp)
#target_link_libraries(femas_future_mdplugin "${PROJECT_SOURCE_DIR}third/Femas/libUSTPmduserapi.so")
#
#add_definitions(-DFEMAS_FUTURE_TDPlugin)
#set(femas_future_tdplugpath "${PROJECT_SOURCE_DIR}/trade/trade_plugins/FEMAS_FUTURE_TDPlugin")
#include_directories("${femas_future_tdplugpath}")
#add_library(femas_future_tdplugin static ${femas_future_tdplugpath}/FEMAS_FUTURE_TDPlugin.cpp)
#target_link_libraries(femas_future_tdplugin "${PROJECT_SOURCE_DIR}third/Femas/libUSTPtraderapi.so")

#add_library(TradeService static ${ATMPath}/public.h ${ATMPath}/TradeService.h ${ATMPath}/TradeService.cpp)

#ATM.o:$(ATMPath)/public.h $(ATMPath)/ATM.cpp $(ATMPath)/TradeService.h $(ATMPath)/TradeService.cpp
#	g++ $(Includes) $(Libs) $(Configs) $(PlugInDef) -c $(ATMPath)/ATM.cpp
#
#ATM:ATM.o TradeService.o libCTP_FUTURE_MDPlugin.a libCTP_FUTURE_TDPlugin.a libFEMAS_FUTURE_MDPlugin.a libFEMAS_FUTURE_TDPlugin.a libDFITC_SOP_MDPlugin.a libDFITC_SOP_TDPlugin.a 
