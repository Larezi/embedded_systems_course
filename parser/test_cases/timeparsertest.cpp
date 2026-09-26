#include <gtest/gtest.h>
#include "../TimeParser.h"

// Test suite: TimeParserTest
TEST(TimeParserTest, TestCaseCorrectTime) {

    // Note that this test fails on purpose!!

    // Test with correct time string
    char time_test[] = "141205";
    ASSERT_EQ(time_parse(time_test), 51125);



}
TEST(TimeParserTest, MaxTime) {
    char time_test[] = "235959";
    ASSERT_EQ(time_parse(time_test), 86399);
}
TEST(TimeParserTest, overTime) {
    char time_test[] = "246060";
    ASSERT_EQ(time_parse(time_test), TIME_VALUE_ERROR);
}
TEST(TimeParserTest, overSeconds) {
    char time_test[] = "000060";
    ASSERT_EQ(time_parse(time_test), TIME_VALUE_ERROR);
}
TEST(TimeParserTest, overMinutes) {
    char time_test[] = "006000";
    ASSERT_EQ(time_parse(time_test), TIME_VALUE_ERROR);
}
TEST(TimeParserTest, overHours) {
    char time_test[] = "240000";
    ASSERT_EQ(time_parse(time_test), TIME_VALUE_ERROR);
}