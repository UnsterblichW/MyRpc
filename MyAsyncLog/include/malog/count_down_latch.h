//
// Created by Unsterblich on 2023-9-23.
//

#pragma once
#include <condition_variable>
#include <mutex>

#include "micros.h"

MALOG_NAMESPACE_BEGIN
namespace detail {
class CountDownLatch
{
public:
   explicit CountDownLatch(int count);
   void wait();
   void countDown();
   int  getCount();

private:
   std::mutex              m_mtx;
   std::condition_variable m_cv;
   int                     m_count;
};
}   // namespace detail
MALOG_NAMESPACE_END
