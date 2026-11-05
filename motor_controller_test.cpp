#include <gtest/gtest.h>
#include <gmock/gmock.h>
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

TEST(GooTest, SetMotorSpeedZero) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-POSITIVE");

  EXPECT_EQ(processCommand("SET", "0"), 0);
}

TEST(GooTest, HighSensorWithZeroDivisorUsesDefault) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeControl(SENSOR_HIGH_THRESHOLD + 1, 0), DEFAULT_VALUE);
}

TEST(GooTest, SensorAtLowThresholdUsesDefault) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeControl(SENSOR_LOW_THRESHOLD, 10), DEFAULT_VALUE);
}

TEST(GooTest, ReadCommandReturnsSensorValue) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t previous_sensor = SENSOR_DATA;
  SENSOR_DATA = 321;

  EXPECT_EQ(processCommand("READ", ""), 321);

  SENSOR_DATA = previous_sensor;
}

TEST(GooTest, UnknownCommandIsRejected) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t previous_speed = MOTOR_SPEED;

  EXPECT_EQ(processCommand("UNKNOWN", "12"), -1);
  EXPECT_EQ(MOTOR_SPEED, previous_speed);
}

TEST(GooTest, SetRejectsLongOverflow) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t previous_speed = MOTOR_SPEED;

  // strtol reports ERANGE before the later integer range checks are evaluated.
  EXPECT_EQ(processCommand("SET", "999999999999999999999999999999"), -1);
  EXPECT_EQ(MOTOR_SPEED, previous_speed);
}

TEST(GooTest, SetRejectsEmptyArgument) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t previous_speed = MOTOR_SPEED;

  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(MOTOR_SPEED, previous_speed);
}

TEST(GooTest, SetRejectsTrailingCharacters) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t previous_speed = MOTOR_SPEED;

  EXPECT_EQ(processCommand("SET", "12x"), -1);
  EXPECT_EQ(MOTOR_SPEED, previous_speed);
}

TEST(GooTest, SetRejectsValueBelowIntMinimum) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t previous_speed = MOTOR_SPEED;

  // One below INT_MIN on the 32-bit ARM target.
  EXPECT_EQ(processCommand("SET", "-2147483649"), -1);
  EXPECT_EQ(MOTOR_SPEED, previous_speed);
}

TEST(GooTest, SetRejectsValueAboveIntMaximum) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t previous_speed = MOTOR_SPEED;

  // One above INT_MAX on the 32-bit ARM target.
  EXPECT_EQ(processCommand("SET", "2147483648"), -1);
  EXPECT_EQ(MOTOR_SPEED, previous_speed);
}
