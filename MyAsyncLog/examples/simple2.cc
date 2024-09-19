#include <malog/logger.h>
using namespace malog;

int main()
{
   GlobalConfig::Get()
     .setLevel(Levels::kTrace)
     .setFormatter(formatter::colorfulFormatter);
   Log::trace(loc::current(), "hello MyAsyncLog");
   Log::debug(loc::current(), "hello MyAsyncLog");
   Log::info(loc::current(), "hello MyAsyncLog");
   Log::warn(loc::current(), "hello MyAsyncLog");
   Log::error(loc::current(), "hello MyAsyncLog");
}