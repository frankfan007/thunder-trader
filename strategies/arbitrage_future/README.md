# An implementation of arbitrage strategy


## Quick start
We will show a complete procession of arbitraging between IF1901 and IF1902 using thunder-trader in this section.

Firstly, we should add the configuration information of IF1901 and IF1902 to the Monitor.json, so that the thunder-trader will know how to subscribe these two instrumentid.

```
{
  "TradeServer": [
    {
      "Name": "Trade server #1",
      "IP": "10.255.83.34",
      "Port": "8800"
    }
  ],
  "UIStyle": {
    "OutLookBarStyle": "NewStyle",
    "OutLookBarTabStyle": "STYLE_3D_ONENOTE"
  },
  "SymbolDefine":{
    "Stock Index Futures 1901":{
      "type":"ctp",
      "instrumentid":"IF1901",
      "exchangeid":"CFFEX"
    },
    "Stock Index Futures 1902":{
      "type":"ctp",
      "instrumentid":"IF1902",
      "exchangeid":"CFFEX"
    }
  }
}
```
Every child of **SymbolDefine** node defined a instrumentid. 
The keyword field, for example, **Stock Index Futures 1901**, will be displayed in the drop-down menu of the strategy configuration interface of monitor.
It is just for human viewing, you can give him a name you like.
The **Type** should be `ctp`, the CTP plugin will check this value, and if it's not `ctp`, it will report an error.
The **instrumentid** is very important, and it's case sensitive.
The **exchangeid** is also very important, because ctp plugin need it when subscribing instrumentid. The value can be one of `CFFEX`,`SHFE`,`DCE`,`CZCE`.


