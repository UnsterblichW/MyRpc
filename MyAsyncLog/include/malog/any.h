#pragma once

#if (__cplusplus && __cplusplus >= 201703L) ||                                 \
  (_MSC_VER && _MSVC_LANG >= 201703L)
#include <any>
namespace malog {
using Any_t = std::any;
using std::any_cast;
}   // namespace malog
#else
#include "dependencies/any_lite.h"
namespace malog {
using Any_t = linb::any;
using linb::any_cast;
}   // namespace malog
#endif
