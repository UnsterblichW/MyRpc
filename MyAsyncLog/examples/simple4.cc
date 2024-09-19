#define ENABLE_MALG_LOG
#include <malog/logger.h>
using namespace malog;

int main()
{
   GlobalConfig::Get()
     .setFilepath(LOG_DIR)
     .setLevel(Levels::kTrace)
     .setFormatter(formatter::colorfulFormatter);
   MALG_TRACE("hello MyAsyncLog");
   MALG_DEBUG("hello MyAsyncLog");
   MALG_INFO("hello MyAsyncLog");
   MALG_WARN("hello MyAsyncLog");
   MALG_ERROR("hello MyAsyncLog");
}