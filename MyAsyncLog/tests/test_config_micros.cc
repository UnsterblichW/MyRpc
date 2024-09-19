#include <chrono>
#include <vector>

#define ENABLE_MALG_CHECK
#define ENABLE_MALG_LOG
#include "doctest/doctest.h"
#include "malog/logger.h"
#include "nanobench.h"

using namespace malog;

struct Timer
{
   std::chrono::time_point<std::chrono::high_resolution_clock> startPoint;
   void start() { startPoint = std::chrono::high_resolution_clock::now(); }
   [[nodiscard]] int64_t end() const
   {
      auto endPoint = std::chrono::high_resolution_clock::now();
      return std::chrono::time_point_cast<std::chrono::nanoseconds>(endPoint)
               .time_since_epoch()
               .count() -
             std::chrono::time_point_cast<std::chrono::nanoseconds>(startPoint)
               .time_since_epoch()
               .count();
   }
};

void set_console_json_config()
{
   GlobalConfig::Get()
     .enableConsole(true)
     .setLevel(malog::kInfo)
     .setFormatter(formatter::jsonFormatter)
     .setFlag(kStdFlags + kThreadId);
}

void set_console_colorful_config()
{
   GlobalConfig::Get()
     .enableConsole(true)
     .setFormatter(formatter::colorfulFormatter)
     .setFlag(kStdFlags + kThreadId);
}

void set_custom_config()
{
   GlobalConfig::Get()
     .enableConsole(true)
     .setFormatter(
       formatter::customFromString("[%n][%T][tid:%t][%L][%F][%f]: %v"))
     .setFlag(kStdFlags + kThreadId);
}

void set_timer_callback_and_load_config()
{
   GlobalConfig::Get()
     .loadFromJSON(PROJECT_ROOT "config.json")
     .setBefore([](output_buf_t& bf) {
        bf.setContext(Timer{});
        auto& tm = any_cast<Timer&>(bf.getMutableContext());
        tm.start();
     })
     .setAfter([](output_buf_t& bf) {
        auto& tm      = any_cast<Timer&>(bf.getMutableContext());
        auto  consume = tm.end();
        bf.formatTo("-----formatter takes:{}ns", consume);
     });
}

void test_timer_callback_and_load_config()
{
   set_timer_callback_and_load_config();
   auto info = Log(malog::kInfo);
   auto warn = Log(malog::kWarn);
   for (int i = 0; i < 10; i++)
   {
      Timer tm;
      tm.start();
      info.println("test1", "test2", "test3", std::vector<int>{3243, 242, 324});
      warn.printf("sum of time:{}ns", tm.end());
   }
}

void test_localConfig()
{
   // register local config
   auto config           = malog::make_unique<Config>();
   config->log_before    = [](output_buf_t& out) { out.append("before"); };
   config->log_after     = [](output_buf_t& out) { out.append("after"); };
   config->log_flag      = kStdFlags + kThreadId;
   config->log_name      = "test_local";
   config->log_level     = kInfo;
   config->log_formatter = formatter::colorfulFormatter;
   Log::RegisterConfig("test_local", std::move(config));

   // create Log by localConfig
   auto log = Log(kInfo, "test_local");
   for (int i = 0; i < 10; i++) log.println("hello world");
}

void test_console_json_config()
{
   set_console_json_config();
   malog::Log::info("hello ejson4cpp");
}

void test_custom_config()
{
   set_custom_config();
   malog::Log::info("hello ejson4cpp");
}

void test_console_colorful_config()
{
   set_console_colorful_config();
   malog::Log::info("hello ejson4cpp");
}

TEST_SUITE_BEGIN("test config&micros");

TEST_CASE("test config")
{
   // test config with callback
   test_timer_callback_and_load_config();
   // test local config
   test_localConfig();
   // test json formatter
   test_console_json_config();
   // test custom formatter
   test_custom_config();
   // test colorful formatter
   test_console_colorful_config();
}

TEST_CASE("test log micros")
{
   GlobalConfig::Get().setLevel(malog::kTrace);
   MALG_TRACE("hello {}", "world");
   MALG_DEBUG("hello {}", "world");
   MALG_INFO("hello {}", "world");
   MALG_WARN("hello {}", "world");
   MALG_ERROR("hello {}", "world");
   CHECK_THROWS_AS(MALG_FATAL("hello {}", "world");, std::runtime_error);
}

TEST_CASE("test check micros")
{
   int a = 1, b = 1;
   MALG_CHECK_EQ(a, b);
   auto* ptr = new malog::context;
   ptr->text = "abc";
   auto* p   = MALG_CHECK_NOTNULL(ptr);
   REQUIRE_EQ(p->text, "abc");
   ptr = nullptr;
   MALG_CHECK_NE(ptr, p);
   a = 0;
   MALG_CHECK_LT(a, b);
   a = 1;
   MALG_CHECK_LE(a, b);
   MALG_CHECK_GE(a, b);
   a = 2;
   MALG_CHECK_GT(a, b);
   delete p;
}

TEST_SUITE_END;