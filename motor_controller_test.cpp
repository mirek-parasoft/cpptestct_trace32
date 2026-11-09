#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <limits.h>
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

TEST(BasicTests, ZeroDivisorUsesDefaultValue) {
  EXPECT_EQ(computeControl(600, 0), DEFAULT_VALUE);
}

TEST(BasicTests, SetMotorSpeedZero) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-POSITIVE");

  EXPECT_EQ(processCommand("SET", "0"), 0);
}

TEST(BasicTests, SetMotorSpeedValidValue) {
  EXPECT_EQ(processCommand("SET", "123"), 123);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST(BasicTests, InvalidSpeedDoesNotChangeMotor) {
  MOTOR_SPEED = 42;
  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(processCommand("SET", "abc"), -1);
  EXPECT_EQ(processCommand("SET", "123abc"), -1);
  EXPECT_EQ(processCommand("SET", "99999999999999999999999999999999"), -1);
  EXPECT_EQ(processCommand("SET", "-99999999999999999999999999999999"), -1);
  EXPECT_EQ(MOTOR_SPEED, 42);
}

class AdditionalMotorControllerTests : public ::testing::Test {
 protected:
  void SetUp() override {
    saved_sensor_ = SENSOR_DATA;
    saved_speed_ = MOTOR_SPEED;
  }

  void TearDown() override {
    SENSOR_DATA = saved_sensor_;
    MOTOR_SPEED = saved_speed_;
  }

 private:
  uint16_t saved_sensor_ = 0;
  uint16_t saved_speed_ = 0;
};

TEST_F(AdditionalMotorControllerTests, SensorAtLowThresholdUsesDefault) {
  EXPECT_EQ(computeControl(SENSOR_LOW_THRESHOLD, 10), DEFAULT_VALUE);
}

TEST_F(AdditionalMotorControllerTests, ReadCommandReturnsSensorWithoutChangingMotor) {
  SENSOR_DATA = 321;
  MOTOR_SPEED = 42;

  EXPECT_EQ(processCommand("READ", ""), 321);
  EXPECT_EQ(MOTOR_SPEED, 42);
}

TEST_F(AdditionalMotorControllerTests, UnknownCommandLeavesMotorUnchanged) {
  MOTOR_SPEED = 42;

  EXPECT_EQ(processCommand("UNKNOWN", "123"), -1);
  EXPECT_EQ(MOTOR_SPEED, 42);
}

TEST_F(AdditionalMotorControllerTests, ProcessorAddsControlToReadCommandResult) {
  SENSOR_DATA = SENSOR_LOW_THRESHOLD;

  EXPECT_EQ(processor("READ", ""),
            static_cast<int>(SENSOR_LOW_THRESHOLD) + DEFAULT_VALUE);
}

TEST_F(AdditionalMotorControllerTests, SetAcceptsIntegerBoundaries) {
  static_assert(INT_MIN == (-2147483647 - 1) && INT_MAX == 2147483647,
                "Boundary strings require 32-bit int");
  static_assert(LONG_MIN == INT_MIN && LONG_MAX == INT_MAX,
                "ARM coverage exclusions require identical long and int ranges");

  EXPECT_EQ(processCommand("SET", "-2147483648"), INT_MIN);
  EXPECT_EQ(MOTOR_SPEED, static_cast<uint16_t>(INT_MIN));
  EXPECT_EQ(processCommand("SET", "2147483647"), INT_MAX);
  EXPECT_EQ(MOTOR_SPEED, static_cast<uint16_t>(INT_MAX));
}

TEST_F(AdditionalMotorControllerTests, SetRejectsValueBelowIntegerMinimum) {
  MOTOR_SPEED = 42;

  EXPECT_EQ(processCommand("SET", "-2147483649"), -1);
  EXPECT_EQ(MOTOR_SPEED, 42);
}

TEST_F(AdditionalMotorControllerTests, SetRejectsValueAboveIntegerMaximum) {
  MOTOR_SPEED = 42;

  EXPECT_EQ(processCommand("SET", "2147483648"), -1);
  EXPECT_EQ(MOTOR_SPEED, 42);
}
