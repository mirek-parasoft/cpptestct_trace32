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

TEST(BasicTests, ZeroDivisorStopsHighSensorControl) {
  EXPECT_EQ(computeControl(600, 0), 0);
}

TEST(BasicTests, ZeroDivisorPreservesNonDivisionBranches) {
  EXPECT_EQ(computeControl(200, 0), 600);
  EXPECT_EQ(computeControl(50, 0), 10);
}

TEST(BasicTests, SetMotorSpeedZero) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-POSITIVE");

  EXPECT_EQ(processCommand("SET", "0"), 0);
}

TEST(BasicTests, SetMotorSpeedDecimal) {
  EXPECT_EQ(processCommand("SET", "123"), 123);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST(BasicTests, InvalidMotorSpeedLeavesRegisterUnchanged) {
  setMotorSpeed(42);
  EXPECT_EQ(processCommand("SET", nullptr), -1);
  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(processCommand("SET", "invalid"), -1);
  EXPECT_EQ(processCommand("SET", "12invalid"), -1);
  EXPECT_EQ(processCommand("SET", "2147483648"), -1);
  EXPECT_EQ(processCommand("SET", "-2147483649"), -1);
  EXPECT_EQ(processCommand("SET", "999999999999999999999999999999"), -1);
  EXPECT_EQ(MOTOR_SPEED, 42);
}

TEST(BasicTests, ReadCommandReturnsSensorWithoutChangingMotorSpeed) {
  const uint16_t saved_sensor = SENSOR_DATA;
  const uint16_t saved_speed = MOTOR_SPEED;
  SENSOR_DATA = 321;
  MOTOR_SPEED = 42;

  EXPECT_EQ(processCommand("READ", nullptr), 321);
  EXPECT_EQ(MOTOR_SPEED, 42);

  SENSOR_DATA = saved_sensor;
  MOTOR_SPEED = saved_speed;
}

TEST(BasicTests, UnknownCommandReturnsErrorWithoutChangingRegisters) {
  const uint16_t saved_sensor = SENSOR_DATA;
  const uint16_t saved_speed = MOTOR_SPEED;
  SENSOR_DATA = 321;
  MOTOR_SPEED = 42;

  EXPECT_EQ(processCommand("UNKNOWN", "123"), -1);
  EXPECT_EQ(SENSOR_DATA, 321);
  EXPECT_EQ(MOTOR_SPEED, 42);

  SENSOR_DATA = saved_sensor;
  MOTOR_SPEED = saved_speed;
}

TEST(BasicTests, ProcessorAddsControlToReadCommandResult) {
  const uint16_t saved_sensor = SENSOR_DATA;
  const uint16_t saved_speed = MOTOR_SPEED;
  SENSOR_DATA = 200;
  MOTOR_SPEED = 42;

  EXPECT_EQ(processor("READ", nullptr), 800);
  EXPECT_EQ(SENSOR_DATA, 200);
  EXPECT_EQ(MOTOR_SPEED, 42);

  SENSOR_DATA = saved_sensor;
  MOTOR_SPEED = saved_speed;
}
