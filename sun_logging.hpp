// One Logger for one process.

#include "sun_config.hpp"
#include "sun_string.hpp"
#include "sun_util.hpp"

#include <cstring>

namespace sun {

namespace log {

enum SeverityLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS
};

const char* LogSeverityLevelName[SeverityLevel::NUM_LOG_LEVELS] =
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL",
};

class LogStream : noncopyable {

}; // class LogStream

class Logger : noncopyable {
public:
  class SourceFile {
  public:
    SourceFile(const char *filename) : data_(filename){
      const char* slash = strrchr(data_, '/');
      if (slash)
      {
          data_ = slash + 1;
      }
      size_ = sun_strlen(data_);
    }

  private:
    const char *data_;

    size_t size_;
  }; // class SourceFile

  Logger(SourceFile file, int line);
  Logger(SourceFile file, int line, SeverityLevel level);
  Logger(SourceFile file, int line, SeverityLevel level, const char* func);
  Logger(SourceFile file, int line, bool toAbort);

  ~Logger();

private:
    static SeverityLevel log_level_;
    static SeverityLevel InitLogLevel();

public:
    static void ResetLogLevel(SeverityLevel lv);
    static SeverityLevel log_level();

private:
  class Impl {
  public:
    Impl(SeverityLevel level, int old_errno, const SourceFile& file, int line);
    LogStream stream_;

  }; // class Impl

  Impl impl_;

public:
  typedef void (*OutputFunc)(const char* msg, int len);
  typedef void (*FlushFunc)();
  
  static void setOutput(OutputFunc);
  static void setFlush(FlushFunc);
  
  static void setTimeZone();

private:
  OutputFunc g_output_;
  FlushFunc g_flush_;

public:
  LogStream& stream();

}; // class Logger

Logger::~Logger() {
    if (impl_.level_ == FATAL)
    {
        g_flush_();
        abort();
    }
}

LogStream& Logger::stream() {
    return impl_.stream_;
}

} // namespace log


#define LOG_TRACE    (log::Logger::log_level() <= log::TRACE) && log::Logger(__FILE__, __LINE__, log::TRACE).stream()
#define LOG_DEBUG    (log::Logger::log_level() <= log::DEBUG) && log::Logger(__FILE__, __LINE__, log::DEBUG).stream()
#define LOG_INFO     (log::Logger::log_level() <= log::INFO) && log::Logger(__FILE__, __LINE__, log::INFO).stream()
#define LOG_WARNNING (log::Logger::log_level() <= log::WARNNING) && log::Logger(__FILE__, __LINE__, log::WARNING).stream()
#define LOG_ERROR    log::Logger(__FILE__, __LINE__, log::ERROR).stream()
#define LOG_FATAL    log::Logger(__FILE__, __LINE__, log::FATAL).stream()

} // namespace sun