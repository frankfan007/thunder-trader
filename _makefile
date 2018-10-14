Includes=-ICommonFiles \
 -ICommonFiles/CompreTradeSystemHeaders \
 -ICommonFiles/CTPForLinux32 \
 -ICommonFiles/DfitcForLinux64 \
 -ICommonFiles/Femas \
 -IQFCompreTradeSystem/AtmMarketDataPlugins \
 -IQFCompreTradeSystem/AtmTradePlugins \
 -ICommonFiles/Tick\
 -I/root/boost_1_58_0


CTP_MD_PlugPath=QFCompreTradeSystem/AtmMarketDataPlugins/CTP_FUTURE_MDPlugin
CTP_TD_PlugPath=QFCompreTradeSystem/AtmTradePlugins/CTP_FUTURE_TDPlugin
DFITC_SOP_MDPlugPath=QFCompreTradeSystem/AtmMarketDataPlugins/DFITC_SOP_MDPlugin
DFITC_SOP_TDPlugPath=QFCompreTradeSystem/AtmTradePlugins/DFITC_SOP_TDPlugin
FEMAS_FUTURE_MDPlugPath=QFCompreTradeSystem/AtmMarketDataPlugins/FEMAS_FUTURE_MDPlugin
FEMAS_FUTURE_TDPlugPath=QFCompreTradeSystem/AtmTradePlugins/FEMAS_FUTURE_TDPlugin
ATMPath=QFCompreTradeSystem/ATM

PlugInDef=-DCTP_FUTURE_MDPlugin -DCTP_FUTURE_TDPlugin -DDFITC_SOP_MDPlugin -DDFITC_SOP_TDPlugin -DFEMAS_FUTURE_MDPlugin -DFEMAS_FUTURE_TDPlugin

soFiles=Lib/thostmduserapi.so Lib/thosttraderapi.so \
Lib/libDFITCSECMdApi.so Lib/libDFITCSECTraderApi.so \
Lib/libUSTPmduserapi.so Lib/libUSTPtraderapi.so

Libs=-ldl -lboost_system -lboost_filesystem -lpthread -lboost_thread -lboost_date_time -lboost_serialization -lboost_log_setup -lboost_log

Configs=-g -std=c++0x -fexceptions -Wall -DBOOST_SPIRIT_THREADSAFE -DBOOST_ALL_DYN_LINK

ATM:ATM.o TradeService.o libCTP_FUTURE_MDPlugin.a libCTP_FUTURE_TDPlugin.a \
libFEMAS_FUTURE_MDPlugin.a libFEMAS_FUTURE_TDPlugin.a \
libDFITC_SOP_MDPlugin.a libDFITC_SOP_TDPlugin.a \
libCommunicationModuleForServerStatic.a
	g++ $(Configs) -L . -o ATM ATM.o TradeService.o  $(Libs)  -lCommunicationModuleForServerStatic \
	-lCTP_FUTURE_MDPlugin -lCTP_FUTURE_TDPlugin \
	-lDFITC_SOP_MDPlugin -lDFITC_SOP_TDPlugin \
	-lFEMAS_FUTURE_MDPlugin -lFEMAS_FUTURE_TDPlugin \
	$(soFiles)

TestServer: TestServer.o libCommunicationModuleForServerStatic.a
	g++ $(Configs) -L . -o TestServer TestServer.o $(Libs) -lCommunicationModuleForServerStatic

TestServer.o: QFCompreTradeSystem/Test.cpp 
	g++ $(Includes) $(Libs) $(Configs) -c QFCompreTradeSystem/Test.cpp -o TestServer.o


ATM.o:$(ATMPath)/public.h $(ATMPath)/ATM.cpp $(ATMPath)/TradeService.h $(ATMPath)/TradeService.cpp
	g++ $(Includes) $(Libs) $(Configs) $(PlugInDef) -c $(ATMPath)/ATM.cpp

TradeService.o:$(ATMPath)/public.h $(ATMPath)/TradeService.h $(ATMPath)/TradeService.cpp
	g++ $(Includes) $(Libs) $(Configs) $(PlugInDef) -c $(ATMPath)/TradeService.cpp




libCTP_FUTURE_MDPlugin.a:CTP_FUTURE_MDPlugin.o AutoPend.o
	ar -r -s libCTP_FUTURE_MDPlugin.a CTP_FUTURE_MDPlugin.o AutoPend.o
libCTP_FUTURE_TDPlugin.a:CTP_FUTURE_TDPlugin.o AutoPend.o
	ar -r -s libCTP_FUTURE_TDPlugin.a CTP_FUTURE_TDPlugin.o AutoPend.o


CTP_FUTURE_MDPlugin.o:$(CTP_MD_PlugPath)/CTP_FUTURE_MDPlugin.cpp $(CTP_MD_PlugPath)/CTP_FUTURE_MDPlugin.h 
	g++ $(Includes) $(Libs) $(Configs) -c $(CTP_MD_PlugPath)/CTP_FUTURE_MDPlugin.cpp -o CTP_FUTURE_MDPlugin.o

CTP_FUTURE_TDPlugin.o:$(CTP_TD_PlugPath)/CTP_FUTURE_TDPlugin.cpp $(CTP_TD_PlugPath)/CTP_FUTURE_TDPlugin.h 
	g++ $(Includes) $(Libs) $(Configs) -c $(CTP_TD_PlugPath)/CTP_FUTURE_TDPlugin.cpp -o CTP_FUTURE_TDPlugin.o





libDFITC_SOP_MDPlugin.a:DFITC_SOP_MDPlugin.o AutoPend.o
	ar -r -s libDFITC_SOP_MDPlugin.a DFITC_SOP_MDPlugin.o AutoPend.o
libDFITC_SOP_TDPlugin.a:DFITC_SOP_TDPlugin.o AutoPend.o
	ar -r -s libDFITC_SOP_TDPlugin.a DFITC_SOP_TDPlugin.o AutoPend.o


DFITC_SOP_MDPlugin.o:$(DFITC_SOP_MDPlugPath)/DFITC_SOP_MDPlugin.cpp\
 QFCompreTradeSystem/AtmMarketDataPlugins/DFITC_SOP_MDPlugin/DFITC_SOP_MDPlugin.h 
	g++ $(Includes) $(Libs) $(Configs) -c QFCompreTradeSystem/AtmMarketDataPlugins/DFITC_SOP_MDPlugin/DFITC_SOP_MDPlugin.cpp -o DFITC_SOP_MDPlugin.o

DFITC_SOP_TDPlugin.o:$(DFITC_SOP_TDPlugPath)/DFITC_SOP_TDPlugin.cpp $(DFITC_SOP_TDPlugPath)/DFITC_SOP_TDPlugin.h 
	g++ $(Includes) $(Libs) $(Configs) -c $(DFITC_SOP_TDPlugPath)/DFITC_SOP_TDPlugin.cpp -o DFITC_SOP_TDPlugin.o


libFEMAS_FUTURE_MDPlugin.a:FEMAS_FUTURE_MDPlugin.o AutoPend.o
	ar -r -s libFEMAS_FUTURE_MDPlugin.a FEMAS_FUTURE_MDPlugin.o AutoPend.o
libFEMAS_FUTURE_TDPlugin.a:FEMAS_FUTURE_TDPlugin.o AutoPend.o
	ar -r -s libFEMAS_FUTURE_TDPlugin.a FEMAS_FUTURE_TDPlugin.o AutoPend.o


FEMAS_FUTURE_MDPlugin.o:$(FEMAS_FUTURE_MDPlugPath)/FEMAS_FUTURE_MDPlugin.cpp\
 QFCompreTradeSystem/AtmMarketDataPlugins/FEMAS_FUTURE_MDPlugin/FEMAS_FUTURE_MDPlugin.h 
	g++ $(Includes) $(Libs) $(Configs) -c QFCompreTradeSystem/AtmMarketDataPlugins/FEMAS_FUTURE_MDPlugin/FEMAS_FUTURE_MDPlugin.cpp -o FEMAS_FUTURE_MDPlugin.o

FEMAS_FUTURE_TDPlugin.o:$(FEMAS_FUTURE_TDPlugPath)/FEMAS_FUTURE_TDPlugin.cpp $(FEMAS_FUTURE_TDPlugPath)/FEMAS_FUTURE_TDPlugin.h 
	g++ $(Includes) $(Libs) $(Configs) -c $(FEMAS_FUTURE_TDPlugPath)/FEMAS_FUTURE_TDPlugin.cpp -o FEMAS_FUTURE_TDPlugin.o



AutoPend.o:CommonFiles/AutoPend.cpp CommonFiles/AutoPend.h
	g++ $(Includes) $(Libs) $(Configs) -c CommonFiles/AutoPend.cpp -o AutoPend.o

libCommunicationModuleForServerStatic.a:connection.o server.o io_service_pool.o
	ar -r -s libCommunicationModuleForServerStatic.a connection.o io_service_pool.o server.o 

connection.o:QFCompreTradeSystem/CommunicationModuleForServerStatic/connection.hpp QFCompreTradeSystem/CommunicationModuleForServerStatic/connection.cpp
	g++ $(Includes) $(Libs) $(Configs)  -c QFCompreTradeSystem/CommunicationModuleForServerStatic/connection.cpp -o connection.o

server.o:QFCompreTradeSystem/CommunicationModuleForServerStatic/server.hpp QFCompreTradeSystem/CommunicationModuleForServerStatic/server.cpp
	g++ $(Includes) $(Libs) $(Configs)  -c QFCompreTradeSystem/CommunicationModuleForServerStatic/server.cpp -o server.o

io_service_pool.o:QFCompreTradeSystem/CommunicationModuleForServerStatic/io_service_pool.hpp QFCompreTradeSystem/CommunicationModuleForServerStatic/io_service_pool.cpp
	g++ $(Includes) $(Libs) $(Configs) -c QFCompreTradeSystem/CommunicationModuleForServerStatic/io_service_pool.cpp -o io_service_pool.o

.PHONY : clean
.PHONY : cleanTestServer
clean:
	rm -f ATM
	rm -f ATM.o
	rm -f connection.o
	rm -f CTP_FUTURE_MDPlugin.o
	rm -f CTP_FUTURE_TDPlugin.o
	rm -f io_service_pool.o
	rm -f libCommunicationModuleForServerStatic.a
	rm -f libCTP_FUTURE_MDPlugin.a
	rm -f libCTP_FUTURE_TDPlugin.a
	rm -f server.o
	rm -f TradeService.o
	rm -f TestServer.o
	rm -f TestServer
	rm -f libDFITC_SOP_MDPlugin.a
	rm -f libDFITC_SOP_TDPlugin.a
	rm -f DFITC_SOP_MDPlugin.o
	rm -f DFITC_SOP_TDPlugin.o
	rm -f libFEMAS_FUTURE_MDPlugin.a
	rm -f libFEMAS_FUTURE_TDPlugin.a
	rm -f FEMAS_FUTURE_MDPlugin.o
	rm -f FEMAS_FUTURE_TDPlugin.o
cleanTestServer:
	rm -f TestServer.o
	rm -f TestServer
