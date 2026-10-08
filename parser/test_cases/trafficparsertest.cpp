#include <gtest/gtest.h>
#include "../TrafficParser.h"
#include "../TimeParser.h"

TEST(TrafficParserTest, ColorTest)
{
    char valid[] = "R 001000";
    char invalid[] = "X 001000";

    EXPECT_EQ(traffic_parse(valid), 600);
    EXPECT_EQ(traffic_parse(invalid), TRAFFIC_COLOR_ERROR);
}

TEST(TrafficParserTest, LengthTest)
{
    char valid[] = "Y 000500";
    char invalid[] = "Y 00050";

    EXPECT_EQ(traffic_parse(valid), 300);
    EXPECT_EQ(traffic_parse(invalid), TRAFFIC_LEN_ERROR);
}

TEST(TrafficParserTest, FormatTest)
{
    char valid[] = "G 000010";
    char invalid[] = "G-000010";

    EXPECT_EQ(traffic_parse(valid), 10);
    EXPECT_EQ(traffic_parse(invalid), TRAFFIC_FORMAT_ERROR);
}

TEST(TrafficParserTest, NullTest)
{
    char valid[] = "R 000001";

    EXPECT_EQ(traffic_parse(valid), 1);
    EXPECT_EQ(traffic_parse(NULL), TRAFFIC_NULL_ERROR);
}

TEST(TrafficParserTest, InvalidTimeTest)
{
    char valid[] = "G 235959";
    char invalid[] = "G 246060";

    EXPECT_EQ(traffic_parse(valid), 86399);
    EXPECT_EQ(traffic_parse(invalid), TIME_VALUE_ERROR);
}