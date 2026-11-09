#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cerrno>
#include <climits>
#include <cstdio>
#include "motor_controller.h"
#include "comm-plugin/cpptest_gtest_xml_listener.h"

int main(int argc, char** const argv) {
  ::testing::InitGoogleTest(&argc, argv);
  
  cpptest::InstallXmlTransportListener();

  const int test_result = RUN_ALL_TESTS();
  const int transport_result =
      cpptest::GetXmlTransportStatus();
  
  return (test_result == 0 && transport_result == 0)
             ? 0
             : 1;
} 

// 
TEST(BasicTests, TestValueAboveMaxTreshold) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-ZERO");
 
  EXPECT_EQ(computeControl(600, 10), 60);
}

// 
TEST(BasicTests, TestGetValueBelowMaxTreshold) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-POSITIVE");

  EXPECT_EQ(computeControl(200, 10), 600);
}

TEST(BasicTests, SetMotorSpeedZero) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-POSITIVE");

  EXPECT_EQ(processCommand("SET", "0"), 0);
}

class AdditionalMotorTests : public ::testing::Test {
 protected:
  void SetUp() override {
    saved_sensor_ = SENSOR_DATA;
    saved_speed_ = MOTOR_SPEED;
    saved_errno_ = errno;
    SENSOR_DATA = 0;
    MOTOR_SPEED = 123;
  }

  void TearDown() override {
    SENSOR_DATA = saved_sensor_;
    MOTOR_SPEED = saved_speed_;
    errno = saved_errno_;
  }

 private:
  uint16_t saved_sensor_ = 0;
  uint16_t saved_speed_ = 0;
  int saved_errno_ = 0;
};

TEST_F(AdditionalMotorTests, ProcessorUsesDefaultForHighSensorAndZeroDivisor) {
  RecordProperty("cpptest_filename", __FILE__);
  SENSOR_DATA = SENSOR_HIGH_THRESHOLD + 1;

  EXPECT_EQ(processor("READ", ""),
            static_cast<int>(SENSOR_DATA) + DEFAULT_VALUE);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(AdditionalMotorTests, ProcessorUsesDefaultAtLowThresholdForUnknownCommand) {
  RecordProperty("cpptest_filename", __FILE__);
  SENSOR_DATA = SENSOR_LOW_THRESHOLD;

  EXPECT_EQ(processor("UNKNOWN", ""), static_cast<int>(DEFAULT_VALUE) - 1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(AdditionalMotorTests, SetRejectsEmptyArgument) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(AdditionalMotorTests, SetRejectsTrailingCharacters) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("SET", "12x"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(AdditionalMotorTests, SetRejectsPositiveLongOverflow) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("SET", "99999999999999999999999999999999999999"), -1);
  EXPECT_EQ(errno, ERANGE);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(AdditionalMotorTests, SetRejectsValueBelowIntMinimum) {
  RecordProperty("cpptest_filename", __FILE__);
  char argument[32];
  std::snprintf(argument, sizeof(argument), "%lld",
                static_cast<long long>(INT_MIN) - 1);

  EXPECT_EQ(processCommand("SET", argument), -1);
  // On targets with equal int/long ranges, strtol rejects this first.
  EXPECT_EQ(errno, LONG_MIN == INT_MIN ? ERANGE : 0);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(AdditionalMotorTests, SetRejectsValueAboveIntMaximum) {
  RecordProperty("cpptest_filename", __FILE__);
  char argument[32];
  std::snprintf(argument, sizeof(argument), "%lld",
                static_cast<long long>(INT_MAX) + 1);

  EXPECT_EQ(processCommand("SET", argument), -1);
  // On targets with equal int/long ranges, strtol rejects this first.
  EXPECT_EQ(errno, LONG_MAX == INT_MAX ? ERANGE : 0);
  EXPECT_EQ(MOTOR_SPEED, 123);
}
