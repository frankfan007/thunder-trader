# 一个高频交易系统的实现 [1].准备工作

在搭建一个高频交易系统之前，我们首先需要搭建开发环境以及收集好所需要的券商服务接口。
> * 开发环境搭建
> * 券商服务接口



## 开发环境搭建

### 1.操作系统
操作系统推荐使用CentOS6 或者 CentOS7。
一般来说版本比较高的linux中附带的软件也是较新版本的。

### 2.gcc
<p>建议使用gcc5以上的版本，最好是gcc7或者gcc8。因为在本项目中，大量使用了C++11的新特征，例如`lambda`、多线程库等等。</p>
<p>使用这些c++标准库的目的主要是为了提高系统的可移植性。大家知道C++真正意义上从语言层面支持多线程就是从C++11开始的。
在之前的C++98中，Windows系统下有`CreateThread`，而Linux系统下则是`pthread`，Windows下有`CRITICAL_SECTION`，而Linux下则是`mutex`。而在C++11的时代，则被`std::thread`，`std::atomic`，`std::feature`，`std::mutex`等标准库一统天下，此外还有`std::lock_guard`，`std::unique_lock`等好用的互斥管理工具。提到C++11的多线程，笔者推荐一本好书<a href='https://item.jd.com/11691853.html'>《C++并发编程实战》。</a><br></p>
<p>本项目的服务端被设计为既可以在Windows/Linux中编译的，如果没有特别的说明，后续文章中均默认开发环境为linux。</p>

### 3.Boost
说到Boost，我的态度是尽量少用。但是构建一个工业级的系统，不可能任何基础功能都自己来实现。
为了把更多的精力放在业务系统本身上面，本项目中还是使用了Boost提供的一些功能。
主要有以下：
#### 1). boost::posix_time && boost::gregorian 
<p>高频交易系统对延迟有着非常高的要求，需要有比较精确且功能强大的时间计算库来支持。
经过笔者对比，目前C++标准库中的时间`std::chrono`在易用性上面不如`boost::posix_time`。
`boost::posix_time`不但能够精确记录时间点，还支持时间的基本运算，时间区间等操作。</p>
#### 2). boost::log
<p>日志在高频交易系统中是至关重要的，交易的细节全部都记录在日志中。</p>
<p>笔者深有体会，盘中交易发生的不符合预期的保单撤单，基本上都可以在盘后通过日志来分析清楚。高频交易中任何不清楚的地方，都足以导致系统变成一个名副其实的碎钞机。由于高频交易系统的对性能要求较高，所以需要一个能提供线程安全并且能高效协调磁盘IO的日志系统,并且日志记录必须是异步形式，不可阻塞交易线程。另外在交易过程中，交易员需要实时看到交易产生的各种日志，尤其是错误日志。boost::log就满足需求。</p>
<p>在本系统中，日志可实时发送到日志服务器的syslog中，配合类似Syslog Watcher Pro这样的图形化日志监控软件，在实际运行中会有不错的效果。</p>



## gcc安装
首先我们从这里下载 [GCC-7.2.0源代码包](https://bigsearcher.com/mirrors/gcc/releases/gcc-7.2.0/gcc-7.2.0.tar.gz)
标准安装过程是这样的（安装在/opt/gcc720）:
```
> tar zxvf gcc-7.2.0.tar.gz
> cd gcc-7.2.0
> ./contrib/download_prerequisites
> cd ..
> mkdir gcc-build-7.2.0
> cd gcc-build-7.2.0
> ../gcc-7.2.0/configure --enable-checking=release --enable-languages=c,c++ --disable-multilib --prefix=/opt/gcc720
> nohup make -j2 1>make.log 2>&1 &
> make install
```

其中`./contrib/download_prerequisites`这个脚本是下载安装一些gcc依赖的库。
由于国内网络的问题，往往需要很长时间才能下载完。
所以我建议自己下载好以后直接安装。
打开`./contrib/download_prerequisites`我们开到开头的代码：
```
gmp='gmp-6.1.0.tar.bz2'
mpfr='mpfr-3.1.4.tar.bz2'
mpc='mpc-1.0.3.tar.gz'
isl='isl-0.16.1.tar.bz2'
```
我们可以直接下载：
> * [gmp-6.1.0.tar.bz2](https://ftp.gnu.org/gnu/gmp/gmp-6.1.0.tar.bz2)
> * [mpfr-3.1.4.tar.bz2](https://www.mpfr.org/mpfr-3.1.4/mpfr-3.1.4.tar.bz2)
> * [mpc-1.0.3.tar.gz](https://ftp.gnu.org/gnu/mpc/mpc-1.0.3.tar.gz)
> * [isl-0.16.1.tar.bz2](https://gcc.gnu.org/pub/gcc/infrastructure/isl-0.16.1.tar.bz2)

将上述文件下载到gcc-7.2.0目录下，然后修改`./contrib/download_prerequisites`这个文件，
将218行左右的这段负责下载的代码注释掉
```
#for ar in $(echo_archives)
#do
#    if [ ${force} -gt 0 ]; then rm -f "${directory}/${ar}"; fi
#    [ -e "${directory}/${ar}" ]                                               \
#        || ${fetch} --no-verbose -O "${directory}/${ar}" "${base_url}${ar}"       \
#        || die "Cannot download ${ar} from ${base_url}"
#done
#unset ar
```
然后再执行`./contrib/download_prerequisites`

## Boost安装
