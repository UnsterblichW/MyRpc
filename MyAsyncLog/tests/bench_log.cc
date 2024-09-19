#include "bench_interface.h"
#include "common_func.h"
#define ENABLE_MALG_LOG
#include "malog/logger.h"

using namespace malog;

void before_bench()
{
   GlobalConfig::Get()
     .loadFromJSON("../../config.json")
     .setFilepath(PROJECT_ROOT "tests/test_log/")
     .enableConsole(false);
}

void one_thread_async(int bench_n, int test_n)
{
   bench_start(
     "malog:one_thread",
     [&]() { test_one_thread(test_n, []() { MALG_INFO(OUTPUT_TEXT); }); },
     bench_n);
}

void one_thread_sync(int bench_n, int test_n)
{
   bench_start(
     "malog:one_thread",
     [&]() { test_one_thread(test_n, []() { MALG_INFO(OUTPUT_TEXT); }); },
     bench_n);
}

void multi_thread_sync(int bench_n, int test_n)
{
   bench_start(
     "malog:multi_thread",
     [&]() { test_multi_thread(test_n, []() { MALG_INFO(OUTPUT_TEXT); }); },
     bench_n);
}

void multi_thread_async(int bench_n, int test_n)
{
   bench_start(
     "malog:multi_thread",
     [&]() { test_multi_thread(test_n, []() { MALG_INFO(OUTPUT_TEXT); }); },
     bench_n);
}
