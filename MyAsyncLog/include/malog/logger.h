#pragma once
#include <sstream>
#include <unordered_map>

#include "async_logging.h"
#include "config.h"
#include "context.h"
#include "fmt/ranges.h"
#include "source_location.h"

MALOG_NAMESPACE_BEGIN

using std::string;
using ConfigPtr = std::unique_ptr<Config>;

namespace detail {
class LoggerImpl : noncopyable
{
private:
   LoggerImpl();

public:
   ~LoggerImpl();

   static LoggerImpl& GetInstance();

   void waitForDone();

   /**
    * Not thread-safe,please use this before all log printing starts
    * @param name
    * @param config
    */
   void registerConfig(StringView name, ConfigPtr config);

   Config* getConfig(StringView name);

   void DoLog(SharedContext const& ctx);

   // 自定义化的config打印
   void DoConfigLog(Config* config, SharedContext const& ctx);

   // 打印Logger内部情况
   static void DoInternalLog(SharedContext const& ctx);

   void LogFile(Config* config, SharedContext const& ctx);

   void LogConsole(Config* config, SharedContext const& ctx);

   [[maybe_unused]] static void LogConsoleUnsafe(Config*              config,
                                                 SharedContext const& ctx);

private:
   void init_data();

private:
   std::unordered_map<std::string, ConfigPtr> m_localConfigFactory;
   std::unique_ptr<AsyncLogging>              m_logging;
   // locked console
   std::mutex                                 m_mutex;
};

inline void DoLog(const SharedContext& ctx)
{
   detail::LoggerImpl::GetInstance().DoLog(ctx);
}

inline void DoInternalLog(SharedContext const& ctx)
{
   detail::LoggerImpl::DoInternalLog(ctx);
}

/**
 * Get the string length at compile time
 * @param str
 * @param len
 * @return
 */
inline constexpr size_t GetStrLen(const char* str, size_t len = 0)   // NOLINT
{
   return str[len] ? GetStrLen(str, len + 1) + 1 : 0;
}

/**
 * Get the file name based on the path at compile time
 * @param filename
 * @param len
 * @return
 */
inline constexpr const char* GetShortName(const char* filename,
                                          size_t len = string::npos)   // NOLINT
{
   return len == string::npos
            ? (len = GetStrLen(filename), GetShortName(filename, len))
          : (len > 0 && (filename[len - 1] == '/' || filename[len - 1] == '\\'))
            ? filename + len
          : (len > 0) ? GetShortName(filename, len - 1)
                      : filename;
}

}   // namespace detail

/**
 * @brief
 * The interface provided to the outside to customize the config output,
 * the default provided interface only supports the global config,
 * when your project needs multiple config configuration output,
 * you should need this interface
 * @param config
 * @param ctx
 */
inline void DoConfigLog(Config* config, SharedContext const& ctx)
{
   detail::LoggerImpl::GetInstance().DoConfigLog(config, ctx);
}

// A small helper for print any pointer.
template <typename T>
void* Ptr(T* ptr)
{
   return ptr;
}

// A small helper for CHECK().
struct logger_helper;

logger_helper Check(bool cond, source_location const& location);

// A small helper for CHECK_EQ, CHECK_NE, CHECK_LE, CHECK_LT,
// CHECK_GE,CHECK_GT,CHECK_NOTNULL().
void CheckIfFatal(bool cond, source_location const& location, const char* text);

// wait for async logging done
inline void WaitForDone() { detail::LoggerImpl::GetInstance().waitForDone(); }

template <typename... Args>
using format_string_t = fmt::format_string<Args...>;
using loc             = malog::source_location;

// log by class
class Log
{
   Log() = default;

public:
   friend struct logger_helper;

   explicit Log(Levels level) : m_level(level) {}

   explicit Log(Levels level, StringView config_name)
     : m_level(level), m_config(GetConfig(config_name))
   {
   }

   /**
    * Not thread-safe,please use this before all log printing starts
    * @param name
    * @param config
    */
   static void RegisterConfig(StringView name, ConfigPtr config)
   {
      detail::LoggerImpl::GetInstance().registerConfig(name, std::move(config));
   }

   static Config* GetConfig(StringView name)
   {
      return detail::LoggerImpl::GetInstance().getConfig(name);
   }

   void set_level(Levels level) { m_level = level; }

   Levels get_level() { return m_level; }

   template <typename T, typename... Args>
   void println(T&& first, Args&&... args) const
   {
      auto     ctx = context::New();
      buffer_t buffer;
      fmt::format_to(std::back_inserter(buffer), "{}, ",
                     std::forward<T>(first));
      println_(ctx, buffer, std::forward<Args>(args)...);
   }

   template <typename T, typename... Args>
   void println(source_location const& loc, T&& first, Args&&... args) const
   {
      auto ctx = context::New();
      init_context_(ctx, loc);
      buffer_t buffer;
      fmt::format_to(std::back_inserter(buffer), "{}, ", first);
      ctx->text = fmt::format("{}", first);
      println_(ctx, buffer, std::forward<Args>(args)...);
   }

#define MALOG_DECAY_IS(a, b)                                                   \
   std::is_same<typename std::decay<a>::type, b>::value

#define MALOG_ENABLE_NOT_STRING                                                \
   (!MALOG_DECAY_IS(T, StringView) && !MALOG_DECAY_IS(T, std::string) &&       \
    !MALOG_DECAY_IS(T, const char*) && !MALOG_DECAY_IS(T, char*))

#define MALOG_ENABLE_IS_STRING                                                 \
   (MALOG_DECAY_IS(T, StringView) || MALOG_DECAY_IS(T, std::string) ||         \
    MALOG_DECAY_IS(T, const char*) || MALOG_DECAY_IS(T, char*))

   template <typename T,
             typename std::enable_if<MALOG_ENABLE_IS_STRING, bool>::type = true>
   void println(T&& first) const
   {
      auto ctx  = context::New();
      ctx->text = first;
      log_it_(ctx);
   }

   template <typename T, typename std::enable_if<MALOG_ENABLE_NOT_STRING,
                                                 bool>::type = true>
   void println(T&& first) const
   {
      auto ctx  = context::New();
      ctx->text = fmt::format("{}", first);
      log_it_(ctx);
   }

   template <typename T,
             typename std::enable_if<MALOG_ENABLE_IS_STRING, bool>::type = true>
   void println(source_location const& loc, T&& first) const
   {
      auto ctx = context::New();
      init_context_(ctx, loc);
      ctx->text = std::string(std::forward<T>(first));
      log_it_(ctx);
   }

   template <typename T, typename std::enable_if<MALOG_ENABLE_NOT_STRING,
                                                 bool>::type = true>
   void println(source_location const& loc, T&& first) const
   {
      auto ctx = context::New();
      init_context_(ctx, loc);
      ctx->text = fmt::format("{}", std::forward<T>(first));
      log_it_(ctx);
   }

   template <class T>
   void printf(T&& first) const
   {
      println(first);
   }

   template <class T>
   void printf(source_location const& loc, T&& first) const
   {
      println(loc, first);
   }

   template <typename... Args>
   void printf(format_string_t<Args...> format, Args&&... args) const
   {
      auto ctx  = context::New();
      ctx->text = fmt::format(format, std::forward<Args>(args)...);
      log_it_(ctx);
   }

   template <typename... Args>
   void printf(source_location const& loc, format_string_t<Args...> format,
               Args&&... args) const
   {
      auto ctx = context::New();
      init_context_(ctx, loc);
      ctx->text = fmt::format(format, std::forward<Args>(args)...);
      log_it_(ctx);
   }

   inline static Log& Get(Levels level)
   {
      Log& ret    = instance();
      ret.m_level = level;
      return ret;
   }

   template <typename... Args>
   inline static void trace(format_string_t<Args...> format, Args&&... args)
   {
      if (should_log_(kTrace))
         Get(kTrace).printf(format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline static void trace(source_location const&   loc,
                            format_string_t<Args...> format, Args&&... args)
   {
      if (should_log_(kTrace))
         Get(kTrace).printf(loc, format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline static void debug(format_string_t<Args...> format, Args&&... args)
   {
      if (should_log_(kDebug))
         Get(kDebug).printf(format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline static void debug(source_location const&   loc,
                            format_string_t<Args...> format, Args&&... args)
   {
      if (should_log_(kDebug))
         Get(kDebug).printf(loc, format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline static void info(format_string_t<Args...> format, Args&&... args)
   {
      if (should_log_(kInfo))
         Get(kInfo).printf(format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline static void info(source_location const&   loc,
                           format_string_t<Args...> format, Args&&... args)
   {
      if (should_log_(kInfo))
         Get(kInfo).printf(loc, format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline static void warn(format_string_t<Args...> format, Args&&... args)
   {
      if (should_log_(kWarn))
         Get(kWarn).printf(format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline static void warn(source_location const&   loc,
                           format_string_t<Args...> format, Args&&... args)
   {
      if (should_log_(kWarn))
         Get(kWarn).printf(loc, format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline static void error(format_string_t<Args...> format, Args&&... args)
   {
      if (should_log_(kError))
         Get(kError).printf(format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline static void error(source_location const&   loc,
                            format_string_t<Args...> format, Args&&... args)
   {
      if (should_log_(kError))
         Get(kError).printf(loc, format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline static void fatal(format_string_t<Args...> format, Args&&... args)
   {
      if (should_log_(kFatal))
         Get(kFatal).printf(format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline static void fatal(source_location const&   loc,
                            format_string_t<Args...> format, Args&&... args)
   {
      if (should_log_(kFatal))
         Get(kFatal).printf(loc, format, std::forward<Args>(args)...);
   }

   template <typename T>
   inline static void trace(T const& format)
   {
      if (should_log_(kTrace)) Get(kTrace).printf(format);
   }

   template <typename T>
   inline static void trace(source_location const& loc, T const& format)
   {
      if (should_log_(kTrace)) Get(kTrace).printf(loc, format);
   }

   template <typename T>
   inline static void debug(T const& format)
   {
      if (should_log_(kDebug)) Get(kDebug).printf(format);
   }

   template <typename T>
   inline static void debug(source_location const loc, T const& format)
   {
      if (should_log_(kDebug)) Get(kDebug).printf(loc, format);
   }

   template <typename T>
   inline static void info(T const& format)
   {
      if (should_log_(kInfo)) Get(kInfo).printf(format);
   }

   template <typename T>
   inline static void info(source_location const& loc, T const& format)
   {
      if (should_log_(kInfo)) Get(kInfo).printf(loc, format);
   }

   template <typename T>
   inline static void warn(T const& format)
   {
      if (should_log_(kWarn)) Get(kWarn).printf(format);
   }

   template <typename T>
   inline static void warn(source_location const& loc, T const& format)
   {
      if (should_log_(kWarn)) Get(kWarn).printf(loc, format);
   }

   template <typename T>
   inline static void error(T const& format)
   {
      if (should_log_(kError)) Get(kError).printf(format);
   }

   template <typename T>
   inline static void error(source_location const& loc, T const& format)
   {
      if (should_log_(kError)) Get(kError).printf(loc, format);
   }

   template <typename T>
   inline static void fatal(T const& format)
   {
      if (should_log_(kFatal)) Get(kFatal).printf(format);
   }

   template <typename T>
   inline static void fatal(source_location const& loc, T const& format)
   {
      if (should_log_(kFatal)) Get(kFatal).printf(loc, format);
   }

private:
   static Log& instance();

   void log_it_(SharedContext& ctx) const;

   static bool should_log_(Levels level)
   {
      return GlobalConfig::Get().level() <= level;
   }

   template <typename T, typename... Args>
   void println_(SharedContext& ctx, buffer_t& buffer, T&& first,
                 Args&&... args) const
   {
      fmt::format_to(std::back_inserter(buffer), "{}, ", first);
      println_(ctx, buffer, std::forward<Args>(args)...);
   }

   template <typename T>
   void println_(SharedContext& ctx, buffer_t& buffer, T&& first) const
   {
      fmt::format_to(std::back_inserter(buffer), "{}", first);
      ctx->text = fmt::to_string(buffer);
      log_it_(ctx);
   }

   static void init_context_(SharedContext&         ctx,
                             source_location const& location)
   {
      ctx->line           = location.line();   // NOLINT
      ctx->func_name      = location.function_name();
      ctx->long_filename  = location.file_name();
      ctx->short_filename = detail::GetShortName(ctx->long_filename);
   }

private:
   Levels  m_level{};
   Config* m_config{};
};

struct logger_helper
{
   explicit logger_helper(source_location const& source)
     : source_(source), log_()
   {
   }

   logger_helper() : source_(), log_() {}

   template <typename... Args>
   inline void trace(format_string_t<Args...> format, Args&&... args)
   {
      if (!should_log_()) return;
      log_.set_level(kTrace);
      log_.printf(source_, format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline void debug(format_string_t<Args...> format, Args&&... args)
   {
      if (!should_log_()) return;
      log_.set_level(kDebug);
      log_.printf(source_, format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline void info(format_string_t<Args...> format, Args&&... args)
   {
      if (!should_log_()) return;
      log_.set_level(kInfo);
      log_.printf(source_, format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline void warn(format_string_t<Args...> format, Args&&... args)
   {
      if (!should_log_()) return;
      log_.set_level(kWarn);
      log_.printf(source_, format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline void error(format_string_t<Args...> format, Args&&... args)
   {
      if (!should_log_()) return;
      log_.set_level(kError);
      log_.printf(source_, format, std::forward<Args>(args)...);
   }

   template <typename... Args>
   inline void fatal(format_string_t<Args...> format, Args&&... args)
   {
      if (!should_log_()) return;
      log_.set_level(kFatal);
      log_.printf(source_, format, std::forward<Args>(args)...);
   }

private:
   bool should_log_() { return source_.file_name() != nullptr; }

   source_location source_;
   Log             log_;
};

MALOG_NAMESPACE_END

#ifdef ENABLE_MALG_CHECK
// check micro
#define MALG_CHECK(condition)                                                  \
   malog::Check(condition, malog::source_location::current())

#define MALG_ASSERT_IF(cond)                                                   \
   malog::CheckIfFatal((cond), malog::source_location::current(),              \
                       "assertion failed:\"" #cond "\"")

#define MALG_CHECK_NOTNULL(ptr) (MALG_ASSERT_IF(ptr != nullptr), ptr)

#define MALG_CHECK_EQ(v1, v2) MALG_ASSERT_IF(v1 == v2)

#define MALG_CHECK_NE(v1, v2) MALG_ASSERT_IF(v1 != v2)

#define MALG_CHECK_LE(v1, v2) MALG_ASSERT_IF(v1 <= v2)

#define MALG_CHECK_LT(v1, v2) MALG_ASSERT_IF(v1 < v2)

#define MALG_CHECK_GE(v1, v2) MALG_ASSERT_IF(v1 >= v2)

#define MALG_CHECK_GT(v1, v2) MALG_ASSERT_IF(v1 > v2)

#endif

#ifdef ENABLE_MALG_LOG
// log with position micro
#define MALG_TRACE(fmt, ...)                                                   \
   malog::Log::trace(malog::loc::current(__FILE__, __LINE__), fmt,             \
                     ##__VA_ARGS__)

#define MALG_DEBUG(fmt, ...)                                                   \
   malog::Log::debug(malog::loc::current(__FILE__, __LINE__), fmt,             \
                     ##__VA_ARGS__)

#define MALG_INFO(fmt, ...)                                                    \
   malog::Log::info(malog::loc::current(__FILE__, __LINE__), fmt, ##__VA_ARGS__)

#define MALG_WARN(fmt, ...)                                                    \
   malog::Log::warn(malog::loc::current(__FILE__, __LINE__), fmt, ##__VA_ARGS__)

#define MALG_ERROR(fmt, ...)                                                   \
   malog::Log::error(malog::loc::current(__FILE__, __LINE__), fmt,             \
                     ##__VA_ARGS__)

#define MALG_FATAL(fmt, ...)                                                   \
   malog::Log::fatal(malog::loc::current(__FILE__, __LINE__), fmt,             \
                     ##__VA_ARGS__)
#endif