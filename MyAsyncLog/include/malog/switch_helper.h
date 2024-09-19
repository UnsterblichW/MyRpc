#pragma once
#include "string_view.h"
namespace malog {
// 递归 把传入的字符串转换成int
inline constexpr int OP_INT(const StringView& sv)
{
   return sv.empty() ? 0
                     : sv[sv.size() - 1] + OP_INT({sv.data(), sv.size() - 1});
}

// 小技巧，C++11之后允许“用户自定义字面量”，字面量运算符的函数名以 operator""
// 开头，后面紧跟用户定义后缀
inline constexpr int operator""_i(const char* op, size_t len)
{
   return OP_INT({op, len});
}
}   // namespace malog