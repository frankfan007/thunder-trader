# Thunder Trader
Based on C++ 11, this project implements a quantitative trading system supporting high-frequency transactions. The system supports the following characteristics:
> * Using plug-ins to support different quotations and trading interfaces, such as CTP, Femas, Dfitc and TWS in China.
> * Support multiple accounts and multiple strategies. Each strategy can use multiple quotes, and can place orders for multiple accounts.
> * The real-time tracing of the variables in the policy can be realized by "probe" and the display mode of the data can be customized.
> * Supporting the "intervention" of the strategy, by predefining some intervention actions, to achieve a fixed operation in the transaction process.
> * C/S architecture

Usage:
```
cmake .
make
```

Software running screenshots:<br>
The Monitor:
![monitor-1](https://github.com/solopointer/thunder-trader/blob/master/assets/hft-monitor1.png)
![monitor-2](https://github.com/solopointer/thunder-trader/blob/master/assets/hft-monitor2.png)
![monitor-3](https://github.com/solopointer/thunder-trader/blob/master/assets/hft-monitor3.png)
![monitor-4](https://github.com/solopointer/thunder-trader/blob/master/assets/hft-monitor4.png)
The strategy research1 platform:<br>
![hft-research1.png](https://github.com/solopointer/thunder-trader/blob/master/assets/hft-research1.png)
