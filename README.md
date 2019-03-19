# Thunder Trader

[![Documentation Status](https://img.shields.io/badge/docs-latest-brightgreen.svg?style=flat)](http://www.huyifeng.net/HFT-start.html)
[![Documentation Status](https://img.shields.io/badge/中文文档-最新-brightgreen.svg)](http://www.huyifeng.net/HFT-start.html)
[![License](https://img.shields.io/badge/license-Apache%202-blue.svg)](LICENSE)

## Features

<img src='https://github.com/solopointer/thunder-trader/blob/master/assets/thunder-trader.png' width=200><br>
Based on C++ 11, this project implements a quantitative trading system supporting high-frequency transactions. The system supports the following characteristics:
- **Flexibility**

    - Using plug-ins to support different quotations and trading interfaces, such as CTP, Femas, Dfitc and TWS in China.
    - Support multiple accounts and multiple strategies. Each strategy can use multiple quotes, and can place orders for multiple accounts.

- **Easy to use**

    - Support both linux and windows
    - B/S architecture
    - Client with graphical interface
    - The real-time tracing of the variables in the strategy can be supported by "probe", and the display mode of the "probe" can be customized.
    - Supporting the "intervention" of the strategy, by predefining some intervention actions, to achieve a fixed operation in the transaction process.

- **Efficiency**

    - C++ implementation
    - Asynchronous event driven

## Snapshot

![monitor-1.jpg](monitor-1.jpg)
![monitor-2.jpg](monitor-2.jpg)


## User's guide
Welcome to my [personal blog](http://www.huyifeng.net/). There are User's guide.
> * [Launch thunder-trader](http://www.huyifeng.net/HFT-usage.html#start_thunder_trader)
> * [Use WebMonitor](http://www.huyifeng.net/HFT-usage.html#use_monitor)
>   * [TradeServer](http://www.huyifeng.net/HFT-usage.html#TradeServer)
>   * [UIStyle](http://www.huyifeng.net/HFT-usage.html#UIStyle)
>   * [SymbolDefine](http://www.huyifeng.net/HFT-usage.html#SymbolDefine)


## Documentation 

Welcome to my [personal blog](http://www.huyifeng.net/). There are detailed introductory manuals.

> * [Introductory](http://www.huyifeng.net/HFT-start.html)
> * [Prepare](http://www.huyifeng.net/HFT-prepare.html)
> * Strategy
> 	*  [Strategy template](http://www.huyifeng.net/HFT-strategy.html)
> 	*  [An arbitrage strategy implementation](https://github.com/solopointer/thunder-trader/tree/master/strategies/arbitrage_future)
> * Trade server 
> * Monitor
> * Research platform

## Quit start
This preject is tested in following environment:
> * gcc 7.2 or 4.8.4
> * boost 1.58.0
> * centos7

Build:
```
>cmake .
>make
>make install
>export LD_LIBRARY_PATH=/path/to/gcc/lib/:/path/to/boost/lib:$LD_LIBRARY_PATH ; /path/to/thunder-trader /path/to/thunder-trader.conf 1

Then will see output like: 
Run.SystemNumber=1 [20181021T062516.546639]
```
If you use a customed gcc path, you should specify the /path/to/gcc/lib/, such as /opt/gcc7/lib64.

As the same, if you use a customed boost path, you should specify the /path/to/boost/lib, such as /opt/boost/lib.

The usage of thunder-trader is `./thunder-trader config_file.conf system_number`, .e.g. `./thunder-trader thunder-trader.conf 1`, the system_number is a customed int that indicate the process by onlyself. You should make sure that different thunder-trader process has different system_number. 

The reason of use a system_number will be illustrated in the document.

Then you can use the [Monitor](http://www.huyifeng.net/HFT-monitor.html) to connect the thunder-trader.

And if the cmake cannot find the boost, you should set the BOOST_ROOT to the custom boost install path.In the CMakeLists.txt,it should be like this: 

```
set(BOOST_ROOT /usr/local/install/boost/install/path)
```

For WebMonitor, we should firstly prepare a nginx for it.
But usually we use [openresty](http://openresty.org/cn/) because he has integrated the nginx-lua and lua-socket we need.
Our thunder-trader monitors the local port. WebMonitor communicates with thunder-trader through lua-socket.
The following is the process of building WebMonitor.


Compile steps: 
* Open the [monitor.sln](https://github.com/solopointer/thunder-trader/blob/master/monitor/monitor.sln) using Microsoft visual stdio 2013 or above.
* Open the property page of project, and switch to the VC++ directory, set the boost root to the correct install path of your boost. 
* Build it.
* Config [Monitor.json](https://github.com/solopointer/thunder-trader/blob/master/monitor/Monitor.json).
* Put the Monitor.exe and [Monitor.json](https://github.com/solopointer/thunder-trader/blob/master/monitor/Monitor.json) and [PEGRP32E.DLL](https://github.com/solopointer/thunder-trader/blob/master/monitor/PEGRP32E.DLL) together in a directory.
* Run Monitor.exe.

## Update log
* 2018.11.20 : Support gcc 4.8.4
* 2018.12.04 : Update arbitrage_strategy & simple_strategy
* 2018.12.20 : Commit the Monitor v0.1


## Ask Questions

E-mail:solopointer@qq.com or QQ Group:628409183

<img src='https://github.com/solopointer/thunder-trader/blob/master/assets/thunder-trader-group.jpeg' width=200><br><br>

Particular attention:
If you encounter an 
```
^C*** Error in `./thunder-trader': double free or corruption (!prev): 0x0000000001e32bb0 ***
```
like error when run thunder-trader, don't worry. This is a bug in CTPLibrary that usually appears when the process exit, 
but does not affect the normal operation of the program.Turn off the ctp option, or tell me how to avoid this error if you knows please.

If you like this project, give me a star ^_^<img src='https://github.com/solopointer/thunder-trader/blob/master/assets/star.jpeg' width=20>

## Copyright and License
Thunder-trader is provided under the [Apache-2.0 license](LICENSE).
