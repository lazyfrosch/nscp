#include <vector>
#include <string>
#include <nscapi/functions.hpp>

#include <gtest/gtest.h>

std::string do_parse(std::string str) {
	Plugin::QueryResponseMessage::Response r;
	nscapi::protobuf::functions::parse_performance_data(&r, str);
	return nscapi::protobuf::functions::build_performance_data(r);
}


TEST(PerfDataTest, fractions) {
	EXPECT_EQ("'aaa'=1.23374g;0.12345;4.47538;2.23747;5.94849", do_parse("aaa=1.2337399999999999999g;0.123456;4.4753845;2.2374742;5.9484945"));
}

TEST(PerfDataTest, empty_string) {
	EXPECT_EQ("", do_parse(""));
}

TEST(PerfDataTest, full_string) {
	EXPECT_EQ("'aaa'=1g;0;4;2;5", do_parse("aaa=1g;0;4;2;5"));
}
TEST(PerfDataTest, full_string_with_ticks) {
	EXPECT_EQ("'aaa'=1g;0;4;2;5", do_parse("aaa=1g;0;4;2;5"));
}
TEST(PerfDataTest, full_spaces) {
	EXPECT_EQ("'aaa'=1g;0;4;2;5", do_parse("     'aaa'=1g;0;4;2;5     "));
}
TEST(PerfDataTest, mltiple_strings) {
	EXPECT_EQ("'aaa'=1g;0;4;2;5 'bbb'=2g;3;4;2;5", do_parse("aaa=1g;0;4;2;5 bbb=2g;3;4;2;5"));
}
TEST(PerfDataTest, only_value) {
	EXPECT_EQ("'aaa'=1g", do_parse("aaa=1g"));
}
TEST(PerfDataTest, multiple_only_values) {
	EXPECT_EQ("'aaa'=1g 'bbb'=2g 'ccc'=3g", do_parse("aaa=1g bbb=2g 'ccc'=3g"));
}
TEST(PerfDataTest, multiple_only_values_no_units) {
	EXPECT_EQ("'aaa'=1 'bbb'=2 'ccc'=3", do_parse("aaa=1 'bbb'=2 ccc=3"));
}
TEST(PerfDataTest, value_with_warncrit) {
	EXPECT_EQ("'aaa'=1g;0;5", do_parse("aaa=1g;0;5"));
}
TEST(PerfDataTest, value_without_warncrit_with_maxmin) {
	EXPECT_EQ("'aaa'=1g;;;0;5", do_parse("aaa=1g;;;0;5"));
}
TEST(PerfDataTest, value_without_warncrit_maxmin) {
	EXPECT_EQ("'aaa'=1g", do_parse("aaa=1g"));
}
TEST(PerfDataTest, leading_space) {
	EXPECT_EQ("'aaa'=1g", do_parse(" aaa=1g"));
	EXPECT_EQ("'aaa'=1g", do_parse("                   aaa=1g"));
}
TEST(PerfDataTest, negative_vvalues) {
	EXPECT_EQ("'aaa'=-1g;-0;-4;-2;-5 'bbb'=2g;-3;4;-2;5", do_parse("aaa=-1g;-0;-4;-2;-5 bbb=2g;-3;4;-2;5"));
}

TEST(PerfDataTest, value_various_reparse) {
	std::vector<std::string> strings;
	strings.push_back("'aaa'=1g;0;4;2;5");
	strings.push_back("'aaa'=6g;1;2;3;4");
	strings.push_back("'aaa'=6g;;2;3;4");
	strings.push_back("'aaa'=6g;1;;3;4");
	strings.push_back("'aaa'=6g;1;2;;4");
	strings.push_back("'aaa'=6g;1;2;3");
	strings.push_back("'aaa'=6g;;;3;4");
	strings.push_back("'aaa'=6g;1;;;4");
	strings.push_back("'aaa'=6g;1;2");
	strings.push_back("'aaa'=6g");
	BOOST_FOREACH(std::string s, strings) {
		EXPECT_EQ(s.c_str(), do_parse(s));
	}
}
