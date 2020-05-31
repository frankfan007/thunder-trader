# 一个高频交易系统的实现 [0].绪论

<p>在介绍高频交易系统之前，首先科普一下量化交易，或者叫程序化交易的基本概念。</p>
<p>量化交易在英文中的称呼是Quantitative transaction，即**“（精确）定量的交易”**。其内在含义便是根据市场当下的量价情况，基于严谨的交易策略，计算出合适的出入场价格和量。</p>
<p>但是人的大脑并不能做出如此实时复杂的计算，所以量化交易具体事通过程序化交易的技术手段来实现的。</p>
接下来我们来聊一下：
> * 量化交易系统现状
> * 我们为什么要造轮子

<p>读完本文，读者可了解目前业内一些交易系统的情况，明白我们为什么要搭建自己的交易系统。</p>

<div>
 <h1>量化交易系统现状</h1>
  根据笔者了解到的情况，目前业内量化公募或者私募实现自己的交易策略有两种方法：
  > * 软件厂商售卖的产品化交易系统
  > * 投资机构的自研系统

 <div>
  <h2>软件厂商提供的交易系统产品</h2>
   <p>笔者用过一些类似的产品，比如
   <a href='https://cxh.wenhua.com.cn/index.asp'>文华财经</a>、
   <a href='https://www.multicharts.cn'>MultiCharts</a>、
   <a href='http://www.tradeblazer.net'>Tradeblazer</a>等，这些系统笔者都用过，基本上来说可以满足大部分量化投资者的需求。</p>
   <p>这些产品为了考虑到所有的专业和非专业人士的需求，往往会有较为傻瓜化的操作方式。
   使用者不需要了解很多计算机的知识，只需要学会使用简单的编程语言（多为Python）再加上一些点按和选择操作，就可以实现大部分自己的策略。有些产品甚至不需要编写策略代码，可以直接使用一种特定格式的计算公式进行组合，就能构造出自己的策略。</p>
   <p>但是，为什么比较专业的团队还是不喜欢用这些产品呢？因为这些产品功能虽然丰富，但是对于一些资深团队来说不够专业:</p>
   <h3>系统效率低下</h3>
   <p>因为这些产品大都是定制一门现有语言作为自己的策略实现语言，例如我的上家所开发的产品，是对Python进行了定制后作为策略开发的语言。
      python语言由于其本身的特点，并不支持多线程技术，所以整个策略几乎以一种同步（Synchronous）的方式在执行，无法高效利用异步（Asynchronous）的方式达到对网络、磁盘IO等信息的最快速处理。学过计算机的同学知道，Synchronous的执行模式会导致程序阻塞并且在阻塞期间无法进行其他的运算。可以想象，当一个最佳报价Tick来了的时候你的程序还阻塞在向磁盘写日志的过程中不能自拔，从而错过了一个最佳入场机会，是不是会很郁闷。</p>
   <h3>系统可开发性弱</h3>
   <p>这样的系统无法提供较高的性能，适合做CTA这类型不需要频繁下单，不需要进行准确入场的策略。</p>
   <h3>系统稳定性弱</h3>
   <p>另外这些系统为了实现用户友好，往往选择了支持图形界面的windows系统。而windows系统本身的稳定性就较低，这也会给交易带来一部分额外的风险。</p>
   <p>此外，这些系统往往将实验、交易两部分功能集成为一个统一的软件，导致很多时候策略就运行在客户机系统上，这样则增加了交易的风险。</p>
 </div>

 <div>
  <h2>投资机构的自研系统</h2>
  <p>具有一定软件开发实力的机构，拥有自己的策略实现方式。由于不受交易系统设计的限制，这样几乎可以100%的实现策略逻辑，实现对下单撤单更精确的控制。</p>
  <p>笔者见过最多的就是一个程序就是一个策略，如果运行多个策略，需要启动多个程序。</p>
  但是，这就会引发一些问题：
  <h3>策略与系统耦合度很高</h3>
  <p>如果一个策略想在另外一个交易商接口上运行就很麻烦。例如之前的策略是针对CTP的，如果新账户使用的是Femas飞马接口，就需要重新实现一次，这样同一策略在不同平台上的一致性就很难保证。</p>
  <h3>没有足够的策略抽象</h3>
  策略开发者在开发策略的时候除了要考虑策略本身的逻辑，还要关注交易底层的细节。这样就无法全身心投入策略逻辑设计。
  此外，一个程序一个策略的方式，也不方便进行统一策略管理。有些账号的登陆时有限制的，例如CTP账号同时只能登陆N次，那也就限制了在这个账户上面最多能运行N个策略。
 </div>

</div>
<div>
 <h1>我们为什么要造轮子</h1>
 <h2>专业的策略开发框架</h2>
 > * 使用C++作为策略开发语言，内存占用低，运行速度快。另外，可使用大量第三方库，如有名的QuantLIb。<br>
**“天下武功，唯快不破”**
 > * 灵活配置交易目标。<br>
**“我的这个策略需要同时输入期货的3个合约、期权的两个合约以及一只股票的tick数据，而且要同时对一个股票账户与一个期权账户下单，能做到吗？能！”**
 > * 对交易逻辑进行高层次抽象，将策略开发者从交易接口的底层细节中解脱出来。<br>
**“CTP接口的CThostFtdcDepthMarketDataField是啥？飞马接口的OnRspUserLogin又是啥？我其实一点都不需要关心。”**
 > * 可自定义C++变量的“探针”，通过“探针”可以在盘中实时可视化任何变量的走势。<br>
**“bidprice/askprice 这两个指标放在一个图里，分别用红色和蓝色。volume放在另外一个图里，用黄色柱状图表示，在交易的时候我需要实时的看到这两个图的数据变化！”**
 > * 策略可以将当前运行状态保存到磁盘，方便进行系统迁移。<br>
**“系统需要重启你下，我先把当前线上的运行的所有策略保存一下。重启完毕后我再恢复～”**
 > * 策略可以自定义“人工干预动作”,在线上策略运行过程中，可人工向策略发送一些预定义的信号。<br>
**“突然发布的新闻对螺纹钢走势产生了严重影响，人工发一个信号让策略强行平仓吧。”**
 > * 同一交易服务器上的不同策略之间可以互相通信。<br>
**“这10个策略组成的策略组最多持仓30手，组内每个策略在开仓前先判断一下其他策略的持仓情况。”**
 > * 优雅的框架，定义了大量的宏，是的策略的代码简洁易懂。<br>
**“在写代码方面，我有洁癖。”**


 <p>下面就是一个最简单的策略，这个策略什么都不做，但是展示了系统的大部分功能:</p>

```cpp
#include "Order.h"
#include "StrategyDefine.h"
#include "StrategyData.h"
#include "Tick.h"
#include "FutureTick.h"
#include "StockTick.h"
#include "FutureTick.h"
#include "OptionTick.h"
#include <cmath>
#include <list>
#include <string>
#include <fstream>
using namespace std;

class MyStrategy : public MStrategy
{
public:
    STRATEGY_TEMPLATE_DECLARE(MyStrategy)
    
    BEGIN_SERIALIZATION
        SERIALIZATION(m_intLevelN)
        SERIALIZATION(m_dbPriceTick)
        SERIALIZATION(m_intGap)
    END_SERIALIZATION
    
    int m_intLevelN = 5;
    double m_dbPriceTick = 0.2;
    int m_intGap = 2;
    
    BEGIN_PARAMETER_BIND
        PARAMETER_INT(m_intLevelN)
        PARAMETER_DOUBLE(m_dbPriceTick)
        PARAMETER_INT(m_intGap)
    END_PARAMETER_BIND
    
    bool IsSupport(TStrategyTickType ticktype)
    {
    	return true;
    }
    
    BEGIN_PROBE_BIND
        BEGIN_GRAPH
            PROBE(&out_AskPrice1, "out_AskPrice1", TProbe_Color_Green)
        END_GRAPH("Line")
        BEGIN_GRAPH
            PROBE(&out_BidPrice1, "out_BidPrice1", TProbe_Color_Blue)
        END_GRAPH("Line")
    END_PROBE_BIND
    
    bool OnInquiry(MStrategyInquiryDataInterface*);
    TLastErrorIdType OnInit(ptime);
    void OnTick(TMarketDataIdType, const CTick *);
    void OnTrade(
    	TOrderRefIdType,
    	TOrderSysIdType,
    	TVolumeType,
    	TPriceType,
    	TOrderDirectionType,
    	TOrderOffsetType);
    void OnOrder(
    	TOrderRefIdType,
    	TOrderSysIdType,
    	TOrderDirectionType,
    	TOrderStatusType,
    	TPriceType,
    	TTradedVolumeType,
    	TRemainVolumeType
    	);
    void OnEndup();
    
    TLastErrorIdType OnInit_FromArchive(ptime)
    {
    	return TLastErrorIdType::LB1_NO_ERROR;
    };
    
    bool OnGetPositionInfo(int *) { return false; };
    bool OnGetCustomInfo(char *, size_t) { return false; };
    bool OnGetFloatingProfit(double *) { return false; };
    bool OnGetStatus(char *, size_t) { return false; };
private:
    atomic<double> out_AskPrice1;
    atomic<double> out_BidPrice1;
};

STRATEGY_TEMPLATE_DEFINITION(MyStrategy)

bool MyStrategy::OnInquiry(MStrategyInquiryDataInterface * data)
{
    return true;
}

TLastErrorIdType MyStrategy::OnInit(ptime)
{
    return LB1_NO_ERROR;
}

void MyStrategy::OnTick(TMarketDataIdType dataid, const CTick *pDepthMarketData)
{
    out_AskPrice1.store(pDepthMarketData->m_dbAskPrice[0]);
    out_BidPrice1.store(pDepthMarketData->m_dbBidPrice[0]);
}

void MyStrategy::OnEndup()
{
}

void MyStrategy::OnTrade(
	TOrderRefIdType ref,
	TOrderSysIdType sys,
	TVolumeType volume,
	TPriceType price,
	TOrderDirectionType dir,
	TOrderOffsetType offset)
{

}

void MyStrategy::OnOrder(
	TOrderRefIdType ref,
	TOrderSysIdType sysId,
	TOrderDirectionType direction,
	TOrderStatusType Status,
	TPriceType LimitPrice,
	TTradedVolumeType VolumeTraded,
	TRemainVolumeType VolumeRemain)
{

}

```
<p>看到上面的策略模版，读者们可能要问：`STRATEGY_TEMPLATE_DECLARE`、<br>`BEGIN_SERIALIZATION`、`PARAMETER_DOUBLE`、`BEGIN_GRAPH`类似的这些是什么东东，为什么我学C++的时候从来没有看到过？</p>
<p>别着急，这些其实都是为了策略的代码更加简洁而预先设定好的一些宏定义。在后面的文章中我会展开详细讲一些这些宏的设计。</p>
<p>其中`OnTick`是当行情数据来了的时候策略需要做的事情，`OnTrade`是报单成交的时候需要做的事情，而`OnOrder`会通知一些撤单相关的信息。策略开发者并不需要关心
策略到底是跑在CTP平台还是飞马或者盈透平台，是不是so easy~~</p>

 
 <h2>专业的交易系统</h2>
> * 研发环境、交易环境、监控环境分离。<br>**“刚才我不小心踢掉电源了！不要着急，策略都在远程的服务器上安全地运行。”**
> * 资源占用小，运行稳定。<br>**“我这个策略要连续不间断跑一年，而且要无人值守！”**
> * 扩展性强，可对接大部分的交易&&行情接口。目前已经支持`CTP`、<br>`飞马`、`盈透`、`大连飞创`等接口。<br>**“咱们这个新产品需要用xxx接口去交易。没问题，写个插件就行。”**
> * 友好的策略监控。<br>**“把这10个策略的监控图调出来，平铺显示！”**
> * 实时日志显示。<br>**“每一笔交易的日志我都要立马看到！”**
 <p>下面运行在本地的监控系统一些截图：</p>
 <p>这些是热轧卷板和螺纹钢的套利，同一个策略，不同的运行参数进行不同周期的套利。可以看到一共有几十个策略在运行。</p>
 <amp-img src="{{ site.baseurl }}assets/images/hft/hft-monitor1.png" width="656" height="350" layout="responsive" alt="" class="mb3"></amp-img>
 <amp-img src="{{ site.baseurl }}assets/images/hft/hft-monitor2.png" width="656" height="350" layout="responsive" alt="" class="mb3"></amp-img>
 <amp-img src="{{ site.baseurl }}assets/images/hft/hft-monitor3.png" width="656" height="350" layout="responsive" alt="" class="mb3"></amp-img>
 <amp-img src="{{ site.baseurl }}assets/images/hft/hft-monitor4.png" width="656" height="350" layout="responsive" alt="" class="mb3"></amp-img>
 <h2>专业的策略研发系统</h2>
> * 数据可视化。
> * 多种盈亏自动计算。
 <p>下图是对一个策略的历史数据测试结果：</p>
 <amp-img src="{{ site.baseurl }}assets/images/hft/hft-research1.png" width="656" height="350" layout="responsive" alt="" class="mb3"></amp-img>
</div>


<p>在接下来的一系列文章中，我将带领大家一步一步实现这个交易系统。<br>
这里先附上Github地址</p>
[https://github.com/solopointer/thunder-trader](https://github.com/solopointer/thunder-trader)
