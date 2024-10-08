#include <doctest/doctest.h>
#include <fmt/ostream.h>
#include <malog/logger.h>

using namespace malog;

TEST_SUITE_BEGIN("common test");

TEST_CASE("test buffer_helper")
{
   buffer_t buffer;
   buffer.push_back('a');
   malog::buffer_helper helper{&buffer};
   helper.formatTo("{}b{}", "a", 123);

   REQUIRE_EQ(to_string(buffer), "aab123");
}

TEST_CASE("test OutputBuffer")
{
   {
      buffer_t buffer;
      buffer.push_back('a');
      output_buf_t output(&buffer);
      output.formatTo("{}b{}", "a", 123);
      REQUIRE_EQ(to_string(buffer), "aab123");
   }
   {
      buffer_t     buffer;
      output_buf_t out(&buffer);
      out.setContext("String");
      REQUIRE_EQ(std::string(any_cast<const char *>(out.getContext())),
                 "String");
      out.setContext(324);
      REQUIRE_EQ(any_cast<int>(out.getContext()), 324);
   }
}

TEST_CASE("test const_util")
{
   REQUIRE_EQ(malog::detail::GetStrLen("abc"), 3);
   REQUIRE_EQ(malog::detail::GetStrLen("a"), 1);
   REQUIRE_EQ(malog::detail::GetStrLen("ab"), 2);

   REQUIRE_EQ(malog::detail::GetShortName("a/b/c"), std::string("c"));
   REQUIRE_EQ(malog::detail::GetShortName("/"), std::string(""));
   REQUIRE_EQ(malog::detail::GetShortName("asfds/afsa/saf"),
              std::string("saf"));
   REQUIRE_EQ(malog::detail::GetShortName("asfds\\afsa\\sdf"),
              std::string("sdf"));
}

TEST_SUITE_END;