project(auto_trade_machine)
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
message(${PROJECT_SOURCE_DIR})
message(${PROJECT_BINARY_DIR})
set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)
configure_file (
  "${PROJECT_SOURCE_DIR}/common/config.h.in"
  "${PROJECT_BINARY_DIR}/common/config.h"
  )
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-Bsymbolic")
    set(BOOST_THREAD "boost_thread-mt")
else()
    set(BOOST_THREAD "boost_thread")
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++11 -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c++11 -fno-strict-aliasing")
set(CMAKE_BUILD_TYPE "Release")

include_directories("${PROJECT_SOURCE_DIR}/common")
include_directories("${PROJECT_SOURCE_DIR}/common/trade_headers")  
include_directories("${PROJECT_SOURCE_DIR}/trade/md_plugins")  
include_directories("${PROJECT_SOURCE_DIR}/trade/trade_plugins")  
include_directories("${PROJECT_SOURCE_DIR}/common/tick")  

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

message(STATUS "${BOOST_THREAD}")
add_definitions(-DBOOST_SPIRIT_THREADSAFE)
find_package(Boost REQUIRED COMPONENTS filesystem regex system thread serialization date_time log log_setup)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
if(NOT Boost_FOUND)
    message("Can not find Boost")
endif()
message(STATUS "Found Boost IncPath ${Boost_INCLUDE_DIRS}")
message(STATUS "Found Boost LibPath ${Boost_LIBRARY_DIRS}")

set(libs dl pthread boost_system boost_filesystem boost_thread boost_regex boost_date_time boost_serialization boost_log_setup boost_log)

add_definitions(-DBOOST_SPIRIT_THREADSAFE)
add_definitions(-DBOOST_ALL_DYN_LINK)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

option (CTP_FUTURE_MDPlugin "Support CTP quotation" ON) 
option (CTP_FUTURE_TDPlugin "Support CTP trade" ON) 

option (DFITC_SOP_MDPlugin "Support DFITC quotation" ON) 
option (DFITC_SOP_TDPlugin "Support DFITC trade" ON) 

option (FEMAS_FUTURE_MDPlugin "Support FEMAS quotation" OFF) 
option (FEMAS_FUTURE_TDPlugin "Support FEMAS trade" OFF) 

add_library(autopend STATIC  common/AutoPend.cpp)


    ###################### CTP ##########################################
include_directories("${PROJECT_SOURCE_DIR}/third/CTPForLinux32/")
link_directories("${PROJECT_SOURCE_DIR}/third/CTPForLinux32/")
    
if (CTP_FUTURE_MDPlugin)
    add_definitions(-DCTP_FUTURE_MDPlugin)
    set(ctp_md_plugpath "${PROJECT_SOURCE_DIR}/trade/md_plugins/CTP_FUTURE_MDPlugin")
    include_directories("${ctp_md_plugpath}")
    add_library(ctp_future_mdplugin ${ctp_md_plugpath}/CTP_FUTURE_MDPlugin.cpp )
    target_link_libraries(ctp_future_mdplugin ${libs} thostmduserapi)
    install(FILES "${PROJECT_SOURCE_DIR}/third/CTPForLinux32/libthostmduserapi.so" DESTINATION lib)
endif (CTP_FUTURE_MDPlugin)
    
if (CTP_FUTURE_TDPlugin)
    add_definitions(-DCTP_FUTURE_TDPlugin)
    set(ctp_td_plugpath "${PROJECT_SOURCE_DIR}/trade/trade_plugins/CTP_FUTURE_TDPlugin")
    include_directories("${ctp_td_plugpath}")
    add_library(ctp_future_tdplugin ${ctp_td_plugpath}/CTP_FUTURE_TDPlugin.cpp)
    target_link_libraries(ctp_future_tdplugin ${libs} thosttraderapi)
    install(FILES "${PROJECT_SOURCE_DIR}/third/CTPForLinux32/libthosttraderapi.so" DESTINATION lib)
endif (CTP_FUTURE_TDPlugin)

    ####################### DFITC ########################################
include_directories("${PROJECT_SOURCE_DIR}/third/DfitcForLinux64/")
link_directories("${PROJECT_SOURCE_DIR}/third/DfitcForLinux64")
    
if (DFITC_SOP_MDPlugin)
    add_definitions(-DDFITC_SOP_MDPlugin)
    set(dfitc_sop_mdplugpath "${PROJECT_SOURCE_DIR}/trade/md_plugins/DFITC_SOP_MDPlugin")
    include_directories("${dfitc_sop_mdplugpath}")
    add_library(dfitc_sop_mdplugin ${dfitc_sop_mdplugpath}/DFITC_SOP_MDPlugin.cpp)
    target_link_libraries(dfitc_sop_mdplugin ${libs} DFITCSECMdApi)
    install(FILES "${PROJECT_SOURCE_DIR}/third/DfitcForLinux64/libDFITCSECMdApi.so" DESTINATION lib)
endif (DFITC_SOP_MDPlugin)
    
if (DFITC_SOP_TDPlugin)
    add_definitions(-DDFITC_SOP_TDPlugin)
    set(dfitc_sop_tdplugpath "${PROJECT_SOURCE_DIR}/trade/trade_plugins/DFITC_SOP_TDPlugin")
    include_directories("${dfitc_sop_tdplugpath}")
    add_library(dfitc_sop_tdplugin ${dfitc_sop_tdplugpath}/DFITC_SOP_TDPlugin.cpp)
    target_link_libraries(dfitc_sop_tdplugin ${libs} DFITCSECTraderApi)
    install(FILES "${PROJECT_SOURCE_DIR}/third/DfitcForLinux64/libDFITCSECTraderApi.so" DESTINATION lib)
endif (DFITC_SOP_TDPlugin)

    ####################### FEMAS ##########################################
include_directories("${PROJECT_SOURCE_DIR}third/Femas/")
link_directories("${PROJECT_SOURCE_DIR}/third/Femas/")
    
if (FEMAS_FUTURE_MDPlugin)
    add_definitions(-DFEMAS_FUTURE_MDPlugin) 
    set(femas_future_mdplugpath "${PROJECT_SOURCE_DIR}/trade/md_plugins/FEMAS_FUTURE_MDPlugin")
    include_directories("${femas_future_mdplugpath}")
    add_library(femas_future_mdplugin STATIC ${femas_future_mdplugpath}/FEMAS_FUTURE_MDPlugin.cpp)
    target_link_libraries(femas_future_mdplugin ${libs} "${PROJECT_SOURCE_DIR}third/Femas/libUSTPmduserapi.so")
    install(FILES "${PROJECT_SOURCE_DIR}/third/Femas/libUSTPmduserapi.so" DESTINATION lib)
endif (FEMAS_FUTURE_MDPlugin)
    
if (FEMAS_FUTURE_TDPlugin)
    add_definitions(-DFEMAS_FUTURE_TDPlugin)
    set(femas_future_tdplugpath "${PROJECT_SOURCE_DIR}/trade/trade_plugins/FEMAS_FUTURE_TDPlugin")
    include_directories("${femas_future_tdplugpath}")
    add_library(femas_future_tdplugin STATIC ${femas_future_tdplugpath}/FEMAS_FUTURE_TDPlugin.cpp)
    target_link_libraries(femas_future_tdplugin ${libs} "${PROJECT_SOURCE_DIR}third/Femas/libUSTPtraderapi.so")
    install(FILES "${PROJECT_SOURCE_DIR}/third/Femas/libUSTPtraderapi.so" DESTINATION lib)
endif (FEMAS_FUTURE_TDPlugin)


#################### trade_service 
include_directories(${PROJECT_SOURCE_DIR}/trade)
add_executable(thunder-trader trade/main.cpp trade/connection.cpp trade/io_service_pool.cpp trade/server.cpp trade/trade_service.cpp)
target_link_libraries(thunder-trader ${libs} ctp_future_mdplugin ctp_future_tdplugin dfitc_sop_mdplugin dfitc_sop_tdplugin autopend)

install (TARGETS thunder-trader DESTINATION bin)
install (FILES thunder-trader.conf.default DESTINATION bin RENAME thunder-trader.conf)


add_subdirectory("${PROJECT_SOURCE_DIR}/strategies/arbitrage_future")
add_subdirectory("${PROJECT_SOURCE_DIR}/strategies/simple_strategy")
