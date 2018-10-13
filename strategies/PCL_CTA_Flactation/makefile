Includes=-I../../CommonFiles \
 -I../../CommonFiles/CompreTradeSystemHeaders \
 -I../../CommonFiles/CTPForLinux32 \
 -I../../QFCompreTradeSystem/AtmMarketDataPlugins \
 -I../../QFCompreTradeSystem/AtmTradePlugins \
 -I../../CommonFiles/Tick\
 -I/root/boost_1_58_0

Libs=-ldl -lboost_system -lboost_filesystem -lpthread -lboost_thread -lboost_date_time -lboost_serialization -lboost_log_setup -lboost_log

Configs=-g -std=c++0x -fexceptions -Wall -Wl,-Bsymbolic -DBOOST_SPIRIT_THREADSAFE -DBOOST_ALL_DYN_LINK

PCL_CTA_Flactation.so:PCL_CTA_Flactation.cpp
	g++ -fPIC -shared -L . $(Includes) $(Libs) $(Configs) PCL_CTA_Flactation.cpp -o PCL_CTA_Flactation.so

.PHONY : clean
clean:
	rm PCL_CTA_Flactation.so
