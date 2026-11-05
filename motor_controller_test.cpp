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

TEST(GooTest, AboveHighThresholdWithZeroDivisorUsesDefault) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeControl(SENSOR_HIGH_THRESHOLD + 1, 0), DEFAULT_VALUE);
}

TEST(GooTest, AtLowThresholdUsesDefault) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeControl(SENSOR_LOW_THRESHOLD, 10), DEFAULT_VALUE);
}

TEST(GooTest, ReadCommandReturnsSensorData) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t savedSensor = SENSOR_DATA;
  SENSOR_DATA = 321;

  EXPECT_EQ(processCommand("READ", ""), 321);

  SENSOR_DATA = savedSensor;
}

TEST(GooTest, UnknownCommandIsRejected) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t savedSpeed = MOTOR_SPEED;
  MOTOR_SPEED = 123;

  EXPECT_EQ(processCommand("UNKNOWN", "0"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);

  MOTOR_SPEED = savedSpeed;
}

TEST(GooTest, SetCommandRejectsEmptyArgument) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t savedSpeed = MOTOR_SPEED;
  MOTOR_SPEED = 123;

  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);

  MOTOR_SPEED = savedSpeed;
}

TEST(GooTest, SetCommandRejectsTrailingCharacters) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t savedSpeed = MOTOR_SPEED;
  MOTOR_SPEED = 123;

  EXPECT_EQ(processCommand("SET", "12x"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);

  MOTOR_SPEED = savedSpeed;
}

TEST(GooTest, SetCommandRejectsStrtolOverflow) {
  RecordProperty("cpptest_filename", __FILE__);
  const uint16_t savedSpeed = MOTOR_SPEED;
  MOTOR_SPEED = 123;

  EXPECT_EQ(processCommand("SET", "999999999999999999999999999999"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);

  MOTOR_SPEED = savedSpeed;
}
