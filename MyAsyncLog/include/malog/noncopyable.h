#pragma once
namespace malog {
class noncopyable
{
public:
   noncopyable(const noncopyable &)    = delete;
   void operator=(const noncopyable &) = delete;

protected:
   noncopyable()  = default;
   ~noncopyable() = default;
};
}   // namespace malog