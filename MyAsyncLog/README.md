
<!-- TOC -->
- [MyAsyncLog简介](#myasynclog简介)
  - [功能概述](#功能概述)
  - [性能](#性能)
- [安装方法](#安装方法)
- [使用方法](#使用方法)
- [技术参考和外部依赖](#技术参考和外部依赖)
<!-- TOC -->

# MyAsyncLog简介
[![License](https://img.shields.io/badge/License-MIT-green)](https://github.com/ACking-you/MyAsyncLog/blob/master/LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Cross--platformable-blue)](https://img.shields.io/badge/Platform-Cross--platformable-blue)
[![Language](https://img.shields.io/badge/Language-C%2B%2B11%20or%20above-red)](https://en.cppreference.com/w/cpp/compiler_support/11)

`MyAsyncLog` 是一个简单易用的日志库，支持C++11及以上版本，支持多平台（Linux、Windows)，接口易用而且性能不错。

## 功能概述
1. API简单，便于使用，在安装了项目之后可以通过调用类的静态方法或者宏定义来输出日志，可见几个简单示例[examples](https://github.com/UnsterblichW/MyAsyncLog/blob/main/examples)
2. 依赖了`fmt`库，使得这个日志库的格式化输出很简单，也易于阅读。
3. 支持用户自定义格式化方式。用户可以自定义`formatter`来实现自己所需的日志输出格式，本库也提供了4个默认的格式化，详见[formatter](https://github.com/UnsterblichW/MyAsyncLog/blob/main/include/malog/formatter.h)
4. 支持用户自定义配置，可配置选项有
   * 滚动日志文件大小，
   * 异步刷入日志间隔时间，
   * 日志输出的最终目的地，
   * 日志打印的级别，
   * 日志信息格式化的方式，
   * 可读取json配置文件后，自定义修改某些参数。
5. 本项目使用CMake配置项目，且支持通过CMake自动拉取所需的依赖项目，对用户来说可以实现极其方便的安装和引入。

## 性能
* 异步输出一条日志的延迟只需 `120ns`，是 spdlog 至少4倍的性能。
对于benchmark，可以参考[tests/bench_start.cc](https://github.com/UnsterblichW/MyAsyncLog/blob/master/tests/bench_start.cc)

# 安装方法
以下两种方式任选其一即可
* Method 1：用cmake中的 `FetchContent` 模块将本项目引入目标工程中
  1. 先引入，CMake代码如下：
        ```cmake
        include(FetchContent)
        FetchContent_Declare(
                MyAsyncLog
                GIT_REPOSITORY https://github.com/UnsterblichW/MyAsyncLog.git
                GIT_TAG origin/fetch
                GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(MyAsyncLog)
        ```
  2. 再链接 `malog`即可
        ```cmake
        target_link_libraries(target  malog)
        ```
* Method 2：下载源码，加入项目子模块
    1. 将该项目添加到子项目中：
        ```cmake
        add_subdirectory(MyAsyncLog)
        ```
    2. 在需要使用该库的目标中链接 `malog` 即可。
        ```cmake
        target_link_libraries(target  malog)
        ```
# 使用方法
1. 使用静态方法 `Log::[loglevel]`
   * `example/simple1.cc`和`example/simple2.cc`中演示了如何使用Log类的静态方法来输出日志，即`Log::trace`、`Log::debug`、`Log::info`、`Log::warn`、`Log::error`、`Log::fatal`；
   * 值得注意的是，本日志库中，`fatal`级别的日志将会主动抛出`std::runtime_error`并且能确保抛出异常前的日志消息正确落盘；
   * 本日志库的日志级别从低到高依次是`trace`、`debug` 、`info` 、`warn` 、`error` 、`fatal`，日志库的默认级别是`debug`，当前消息的日志级别如果低于日志库的默认级别，日志将不会被输出；
2. 使用宏定义`MALG_[loglevel]`
   * `example/simple3.cc`和`example/simple4.cc`中演示了如何使用宏定义来输出日志，`setFilepath("../log/")` 指定了输出文件的文件夹为上一层级的 log 文件夹。
   * 使用宏之前，需要在 `#include<elog/logger.h>` 之前定义 `ENABLE_ELG_LOG` 宏。
   * 如果需要禁用输出到控制台，则只需要添加下列配置：`GlobalConfig::Get().enableConsole(false)` 。同理如果不需要输出到文件，则需要保持 `log_filepath` 的值为默认值 `nullptr` 即可。
3. 自定义`formatter`
    `formatter`是一个用于控制格式化输出的接口实现，本日志库内部已经实现的`formatter`有如下几种：
    * `defaultFormatter` ：这是默认的formatter，格式固定。
    * `jsonFormatter`：以json格式输出，格式固定。
    * `colorfulFormatter`：输出的格式与formatter相同，但输出到控制台的时候有颜色高亮。
    * `customFromString`：可以根据用户传入的字符串自定义输出格式。 具体描述：
    ```shell
      %T：表示整个日期时间，还包括时区。
      %t：表示线程id。
      %F：表示该条日志输出来自哪个文件。
      %f：表示该条日志输出来自哪个函数。
      %e：表示如果 errno 存在错误则表示该错误信息，否则表示空。
      %n：表示当前日志器的名字，如果不存在，则表示为空。
      %L：表示长的代表日志等级的字符串，比如 TRACE 。
      %l：表示短的代表日志等级的字符串，比如 TRC 。
      %v：表示日志输出的内容。
      %c和%C：表示颜色的开始与结束，只在支持 \033 的终端中有效。
    ```
    如果需要自定义 formatter ，则需要了解如下函数签名：
    ```c++
      using formatter_t = std::function<void(Config* config, context const& ctx, buffer_t&buf,Appenders apender_type)>;
    ```
    各个参数的含义如下：

    * config：当前日志输出使用的配置。
    * ctx：当前日志输出的相关内容，包括需要输出的日志内容、日志等级和行号等等信息。
    * buf：当前日志最终格式化后需要输出到的 buffer 。
    * appder_type：这是一个枚举代表当前日志格式化输出的目的地，具体有 文件 或 控制台 两种。
    根据上述解释，如果想要实现一个自己的 formatter ，就可以通过 config 的配置信息和 ctx 的输出信息以及 appender_type 的目的地信息来定制化的格式化输出内容到 buf 中。

# 技术参考和外部依赖
* 多线程异步日志的核心类`AsyncLogging`，借鉴了[`muduo`](https://github.com/chenshuo/muduo)网络库中的日志设计，采用了其中对于日志库的[双缓冲](https://github.com/chenshuo/muduo/blob/master/muduo/base/AsyncLogging.h)的设计
* `formatter`的设计是延用[`fmt`](https://github.com/fmtlib/fmt)库的设计思路
* json配置的读取使用了[`ejson4cpp`](https://gitee.com/acking-you/ejson4cpp)
* 为了让C++11支持类似C++17那样的`string_view`引入了[`string-view-lite`](https://github.com/martinmoene/string-view-lite)这个库
* 在单元测试和性能测试中使用了[`doctest`](https://github.com/doctest/doctest)和[`nanobench`](https://github.com/martinus/nanobench)
