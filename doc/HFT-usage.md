# 一个高频交易系统的实现 [3.2].Thunder-trader使用手册

<div>
Thunder-trader采用的是标准的C/S架构，全部交易策略都无人值守地运行在远程的服务器上，Thunder-trader会在交易时段自动连接交易所的行情服务器接受行情并且驱动策略产生报单。在非交易时段，会自动进入睡眠。而客户端运行在PC电脑上，在需要的是进行策略部署、策略撤销以及查看策略的运行状态(查看指标曲线)。
</div>

<div>
<h2 id="start_thunder_trader">启动交易服务</h2>
<p>交易服务由thunder-trader和一个配置文件组成。</p>
<p>thunder-trader链接了libboost以及一些交易相关的库，在启动前需要确保这些库能被找到。</p>
```cpp
>>>ldd /usr/local/bin/thunder-trader
linux-vdso.so.1 =>  (0x00007ffc617e9000)
libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f6fd98d4000)
libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f6fd96b5000)
libboost_system.so.1.58.0 => /usr/local/lib/libboost_system.so.1.58.0 (0x00007f6fd94b1000)
libboost_filesystem.so.1.58.0 => /usr/local/lib/libboost_filesystem.so.1.58.0 (0x00007f6fd929a000)
libboost_thread.so.1.58.0 => /usr/local/lib/libboost_thread.so.1.58.0 (0x00007f6fd9079000)
libboost_regex.so.1.58.0 => /usr/local/lib/libboost_regex.so.1.58.0 (0x00007f6fd8d92000)
libboost_log.so.1.58.0 => /usr/local/lib/libboost_log.so.1.58.0 (0x00007f6fd8ac8000)
libthostmduserapi.so => /usr/local/lib/libthostmduserapi.so (0x00007f6fd8528000)
libthosttraderapi.so => /usr/local/lib/libthosttraderapi.so (0x00007f6fd7d98000)
libDFITCSECMdApi.so => /usr/local/lib/libDFITCSECMdApi.so (0x00007f6fd7b26000)
libboost_date_time.so.1.58.0 => /usr/local/lib/libboost_date_time.so.1.58.0 (0x00007f6fd7914000)
libDFITCSECTraderApi.so => /usr/local/lib/libDFITCSECTraderApi.so (0x00007f6fd7647000)
libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f6fd7343000)
libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f6fd703c000)
libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f6fd6e26000)
libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f6fd6a61000)
/lib64/ld-linux-x86-64.so.2 (0x00005610f9740000)
librt.so.1 => /lib/x86_64-linux-gnu/librt.so.1 (0x00007f6fd6858000)
```

<p>配置文件中包含了交易服务运行需要的一些参数，举个例子：</p>

```javascript
{
    "basic": {
        "address": "0.0.0.0",
        "listenport": "8800",
        "nethandlerthreadcount": "2",
        "strategybinroot": ".."
    },
    "logconfig": {
        "logtype": "syslog|file|console",
        "syslog": {
            "serveraddress": "127.0.0.1",
            "port": "514"
        },
        "file": {
            "target": "ATMLOG",
            "filenamehead": "atm_log_",
            "rotationsize": "1048576",
            "autoflush": "true"
        },
        "console": {
        }
    }
}
```

<ul>
    <li>
    basic节点中记录了一些基本配置，
        <ul>
            <li><b>address:listenport</b>：交易服务的网络通信监听地址</li>
            <li><b>nethandlerthreadcount</b>：负责网络通信的线程池大小，一般情况下设置为2即可</li>
            <li><b>strategybinroot</b>：存储策略的路径</li>
        </ul>
    </li>
    <li>
    logconfig记录了日志相关的配置
        <ul>
            <li><b>logtype</b>：表示要启动的日志类型，目前支持syslog、文件以及输出在console。可以选在开启某种或者几个。</li>
            <li><b>syslog</b>：指定了syslog服务器的地址</li>
            <li><b>file</b>：指定了文件日志存储的一些细节</li>
        </ul>
    </li>
</ul>
<p>将上述文件保存为thunder-trader.conf后，即可执行命令启动交易服务。</p>

```cpp
/path/to/thunder-trader /path/to/thunder-trader.conf 1
```
</div>

<h2 id="use_monitor">使用Monitor</h2>
<div>
<p><a href="/download/monitor.tar.gz">点击这里</a> 可以下载到已经构建好的Monitor二进制程序，其中包括一个默认的配置文件。
Monitor启动的时候会读取相同目录下面的`Monitor.json`配置文件。举个例子</p>

```javascript
{
  "TradeServer": [
      {
          "Name": "MainServer",
          "IP": "127.0.0.1",
          "Port": "8800"
      }
  ],
  "UIStyle": {
      "OutLookBarStyle": "NewStyle",
      "OutLookBarTabStyle": "STYLE_3D_ONENOTE"
  },
  "SymbolDefine":{
      "IF1601":{
          "type":"ctp",
          "instrumentid":"IF1601",
          "exchangeid":"CFFEX"
      },
      "11000497":{
          "type":"dfitc",
          "instrumentid":"11000497",
          "exchangeid":"SH"
      },
      "XINA50201601":{
          "type":"TWS",
          "symbol":"XINA50",
          "secType":"FUT",
          "expiry":"201601",
          "exchange":"SGX",
          "currency":"USD"
      }
  }
}
```

<p>配置文件中有3个字段需要设置，分别是`TradeServer`，`UIStyle`，`SymbolDefine`。</p>

<h3 id="TradeServer">TradeServer</h3>

<p>该数组里面每一个元素代表一个thunder-trader的进程，会显示在Monitor的左侧的菜单栏中。</p>

<img src="./thunder-trader-list.jpg" width="500" layout="responsive" alt="" class="mb3">

<h3 id="UIStyle">UIStyle</h3>

<p>这个参数控制了Monitor的界面风格，可以选择`STYLE_3D_ONENOTE`，`STYLE_3D_ROUNDED`，
`STYLE_3D_ROUNDED_SCROLL`，`STYLE_3D_SCROLLED`，`STYLE_3D_VS2005`，
`STYLE_FLAT`，`STYLE_FLAT_SHARED_HORZ_SCROLL`。</p>

<h3 id="SymbolDefine">SymbolDefine</h3>

<p>参数重配置了交易用到的一些合约的信息，例如在CTP平台下，如果想交易IF股指期货，就需要知道合约代码和交易所代码。在盈透TWS平台下甚至还需要配置currency等。每一个配置都会被传递到对应的行情插件中，开发行情插件时可以根据需要来使用这些信息。
打开Monitor首先需要添加需要用到的交易账号和行情账号。在左侧的行情源现象卡中点击右键选择添加行情源，</p>

<img src="./new_md_1.jpg" width="500" layout="responsive" alt="" class="mb3">
<p>选择行情源的类型，这里支持的类型与thunder-trader编译时支持的插件有关，例如在本例中CMakeLists.txt中的插件编译配置如下：</p>

```cpp
option (CTP_FUTURE_MDPlugin "Support CTP quotation" ON) 
option (CTP_FUTURE_TDPlugin "Support CTP trade" ON) 

option (DFITC_SOP_MDPlugin "Support DFITC quotation" ON) 
option (DFITC_SOP_TDPlugin "Support DFITC trade" ON) 

option (FEMAS_FUTURE_MDPlugin "Support FEMAS quotation" OFF) 
option (FEMAS_FUTURE_TDPlugin "Support FEMAS trade" OFF) 
```

<p>其中打开了CTP的行情（MD）与交易（TD）插件，大连飞创的期权行情与交易插件，所以我们在行情源选择的下拉菜单可以看到：</p>

<img src="./new_md_2.jpg" width="500" layout="responsive" alt="" class="mb3">

选择好类型后填写相应的登陆账号即可。

<img src="./new_md_3.jpg" width="500" layout="responsive" alt="" class="mb3">

 
<p>如果添加成功，就可以在左侧的行情源选项卡中看到账号的相关信息。</p>

<img src="./new_md_4.jpg" width="500" layout="responsive" alt="" class="mb3">


<p>之后thunder-trader会在开盘时间自动启动相应的行情源并订阅需要用到的合约行情。添加交易源也是相似的操作步骤。</p>

<img src="./new_td_1.jpg" width="500" layout="responsive" alt="" class="mb3">


<p>当然还有更加方便的添加行情源和交易源的方法：使用配置文件。可以事先准备好常用的交易账号和行情账号的配置文件，
打开Monitor时直接将这些配置文件用鼠标拖动到“行情源”和“交易源”的选项卡中可以实现快速添加。
配置文件的格式如下：</p>
<ul>
    <li><a href="https://github.com/solopointer/thunder-trader/blob/master/config_file_templates/CTP_MD.json">CTP_MD.json</a></li>
    <li><a href="https://github.com/solopointer/thunder-trader/blob/master/config_file_templates/CTP_TD.json">CTP_TD.json</a></li>
</ul>
</div>






