//
// Created by Unsterblich on 2023-9-21.
//
#pragma once

#include <cstdio>

#include "micros.h"
#include "noncopyable.h"

MALOG_NAMESPACE_BEGIN
namespace detail {
class FileAppender : noncopyable
{
public:
   // 'e' 代表O_CLOEXEC，可以防止多进程中文件描述符暴露给子进程
   explicit FileAppender(const char* filename);
   ~FileAppender();
   void                 append(const char* line, size_t len);
   void                 flush();
   void                 resetWritten() { m_writenBytes = 0; }
   [[nodiscard]] size_t writtenBytes() const { return m_writenBytes; }

private:
   size_t write(const char* line, size_t len);
   void   init(const char*);

private:
   char   m_buffer[64 * 1024]{};
   FILE*  m_file{};
   size_t m_writenBytes{};
};
}   // namespace detail
MALOG_NAMESPACE_END