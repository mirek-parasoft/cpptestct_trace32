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

TEST(BasicTests, SetMotorSpeedZero) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-POSITIVE");

  EXPECT_EQ(processCommand("SET", "0"), 0);
}

class MotorControllerCoverageTests : public ::testing::Test {
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

TEST_F(MotorControllerCoverageTests, HighSensorWithZeroDivisorUsesDefault) {
  RecordProperty("req", "REQ-28-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeControl(600, 0), DEFAULT_VALUE);
}

TEST_F(MotorControllerCoverageTests, SensorAtLowThresholdUsesDefault) {
  RecordProperty("req", "REQ-28-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeControl(SENSOR_LOW_THRESHOLD, 10), DEFAULT_VALUE);
}

TEST_F(MotorControllerCoverageTests, SetRejectsNullArgumentWithoutChangingSpeed) {
  RecordProperty("req", "REQ-28-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);

  MOTOR_SPEED = 123;
  EXPECT_EQ(processCommand("SET", nullptr), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTests, SetRejectsOverflowWithoutChangingSpeed) {
  RecordProperty("req", "REQ-28-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);

  MOTOR_SPEED = 123;
  EXPECT_EQ(processCommand("SET", "999999999999999999999999999999"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
  EXPECT_EQ(processCommand("SET", "-999999999999999999999999999999"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTests, SetRejectsEmptyArgumentWithoutChangingSpeed) {
  RecordProperty("req", "REQ-28-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);

  MOTOR_SPEED = 123;
  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTests, SetRejectsTrailingTextWithoutChangingSpeed) {
  RecordProperty("req", "REQ-28-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);

  MOTOR_SPEED = 123;
  EXPECT_EQ(processCommand("SET", "42x"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTests, ReadReturnsSensorValue) {
  RecordProperty("req", "REQ-28-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);

  SENSOR_DATA = 321;
  EXPECT_EQ(processCommand("READ", nullptr), 321);
}

TEST_F(MotorControllerCoverageTests, UnknownCommandIsRejected) {
  RecordProperty("req", "REQ-28-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);

  MOTOR_SPEED = 123;
  EXPECT_EQ(processCommand("UNKNOWN", nullptr), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTests, ProcessorCombinesCommandAndControlResults) {
  RecordProperty("req", "REQ-28-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);

  SENSOR_DATA = 600;
  EXPECT_EQ(processor("SET", "42"), 42 + DEFAULT_VALUE);
  EXPECT_EQ(MOTOR_SPEED, 42);
}
