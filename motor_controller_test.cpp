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
TEST(GooTest, TestValueAboveMaxTreshold) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-ZERO");
 
  EXPECT_EQ(computeControl(600, 10), 60);
}

// 
TEST(GooTest, TestGetValueBelowMaxTreshold) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-POSITIVE");

  EXPECT_EQ(computeControl(200, 10), 600);
}

TEST(GooTest, ZeroDivisorUsesDefaultValue) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeControl(600, 0), DEFAULT_VALUE);
}

TEST(GooTest, SetMotorSpeedZero) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-POSITIVE");

  EXPECT_EQ(processCommand("SET", "0"), 0);
}

TEST(GooTest, SetMotorSpeedValidNumber) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("SET", "123"), 123);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST(GooTest, InvalidMotorSpeedLeavesSpeedUnchanged) {
  RecordProperty("cpptest_filename", __FILE__);
  setMotorSpeed(42);

  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(processCommand("SET", "invalid"), -1);
  EXPECT_EQ(processCommand("SET", "123x"), -1);
  EXPECT_EQ(processCommand("SET", "999999999999999999999999"), -1);
  EXPECT_EQ(processCommand("SET", "-999999999999999999999999"), -1);
  EXPECT_EQ(MOTOR_SPEED, 42);
}

TEST(GooTest, SensorAtLowThresholdUsesDefaultValue) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeControl(SENSOR_LOW_THRESHOLD, 10), DEFAULT_VALUE);
}

TEST(GooTest, ReadCommandReturnsSensorValue) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t original_sensor = SENSOR_DATA;
  SENSOR_DATA = 321;

  EXPECT_EQ(processCommand("READ", ""), 321);

  SENSOR_DATA = original_sensor;
}

TEST(GooTest, UnknownCommandLeavesSpeedUnchanged) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t original_speed = MOTOR_SPEED;
  setMotorSpeed(42);

  EXPECT_EQ(processCommand("UNKNOWN", "123"), -1);
  EXPECT_EQ(MOTOR_SPEED, 42);

  setMotorSpeed(original_speed);
}

TEST(GooTest, MotorSpeedBelowIntMinimumIsRejected) {
  RecordProperty("cpptest_filename", __FILE__);
  char argument[32];
  std::snprintf(argument, sizeof(argument), "%lld",
                static_cast<long long>(INT_MIN) - 1);
  const uint16_t original_speed = MOTOR_SPEED;
  setMotorSpeed(42);

  EXPECT_EQ(processCommand("SET", argument), -1);
  EXPECT_EQ(MOTOR_SPEED, 42);
#if LONG_MIN == INT_MIN
  // On ARM, strtol reports ERANGE before parsed < INT_MIN can be true.
  EXPECT_EQ(errno, ERANGE);
#endif

  setMotorSpeed(original_speed);
}

TEST(GooTest, MotorSpeedAboveIntMaximumIsRejected) {
  RecordProperty("cpptest_filename", __FILE__);
  char argument[32];
  std::snprintf(argument, sizeof(argument), "%lld",
                static_cast<long long>(INT_MAX) + 1);
  const uint16_t original_speed = MOTOR_SPEED;
  setMotorSpeed(42);

  EXPECT_EQ(processCommand("SET", argument), -1);
  EXPECT_EQ(MOTOR_SPEED, 42);
#if LONG_MAX == INT_MAX
  // On ARM, strtol reports ERANGE before parsed > INT_MAX can be true.
  EXPECT_EQ(errno, ERANGE);
#endif

  setMotorSpeed(original_speed);
}
