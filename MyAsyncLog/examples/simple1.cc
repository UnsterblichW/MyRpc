#include <malog/logger.h>
using namespace malog;

int main()
{
   Log::trace("hello MyAsyncLog");
   Log::debug("hello MyAsyncLog");
   Log::info("hello MyAsyncLog");
   Log::warn("hello MyAsyncLog");
   Log::error("hello MyAsyncLog");
   Log::fatal("hello MyAsyncLog");
}