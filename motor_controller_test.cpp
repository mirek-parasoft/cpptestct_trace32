#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "motor_controller.h"
#include "comm-plugin/cpptest_gtest_xml_listener.h"

// Match the implemented overload; the header declares a const-pointer overload.
int processor(char* const cmd, char* const arg);

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

TEST(BasicTests, SetMotorSpeedValidRange) {
  EXPECT_EQ(processCommand("SET", "123"), 123);
  EXPECT_EQ(MOTOR_SPEED, 123);
  EXPECT_EQ(processCommand("SET", "65535"), 65535);
  EXPECT_EQ(MOTOR_SPEED, UINT16_MAX);
}

TEST(BasicTests, InvalidSpeedLeavesMotorUnchanged) {
  setMotorSpeed(123);
  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(processCommand("SET", "abc"), -1);
  EXPECT_EQ(processCommand("SET", "12abc"), -1);
  EXPECT_EQ(processCommand("SET", "-1"), -1);
  EXPECT_EQ(processCommand("SET", "65536"), -1);
  EXPECT_EQ(processCommand("SET", "999999999999999999999999"), -1);
  EXPECT_EQ(processCommand("SET", nullptr), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

class CoverageTests : public ::testing::Test {
 protected:
  void SetUp() override {
    saved_sensor_ = SENSOR_DATA;
    saved_motor_speed_ = MOTOR_SPEED;
  }

  void TearDown() override {
    SENSOR_DATA = saved_sensor_;
    MOTOR_SPEED = saved_motor_speed_;
  }

 private:
  uint16_t saved_sensor_ = 0;
  uint16_t saved_motor_speed_ = 0;
};

TEST_F(CoverageTests, SensorAtLowThresholdUsesDefaultValue) {
  RecordProperty("req", "REQ-29-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeControl(SENSOR_LOW_THRESHOLD, 10), DEFAULT_VALUE);
}

TEST_F(CoverageTests, ReadCommandReturnsSensorWithoutChangingMotorSpeed) {
  RecordProperty("req", "REQ-29-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);
  SENSOR_DATA = 321;
  MOTOR_SPEED = 123;

  EXPECT_EQ(processCommand("READ", nullptr), 321);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(CoverageTests, UnknownCommandIsRejectedWithoutChangingMotorSpeed) {
  RecordProperty("req", "REQ-29-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);
  MOTOR_SPEED = 123;

  EXPECT_EQ(processCommand("UNKNOWN", nullptr), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(CoverageTests, ProcessorAddsFallbackControlToReadResult) {
  RecordProperty("req", "REQ-29-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);
  SENSOR_DATA = 600;
  MOTOR_SPEED = 123;
  char command[] = "READ";
  const int expected = 600 + DEFAULT_VALUE;

  EXPECT_EQ(processor(command, nullptr), expected);
  EXPECT_EQ(MOTOR_SPEED, 123);
}
