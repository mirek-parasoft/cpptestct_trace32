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

class MotorControllerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    RecordProperty("cpptest_filename", __FILE__);
    saved_sensor_ = SENSOR_DATA;
    saved_speed_ = MOTOR_SPEED;
    MOTOR_SPEED = 123;
  }

  void TearDown() override {
    SENSOR_DATA = saved_sensor_;
    MOTOR_SPEED = saved_speed_;
  }

 private:
  uint16_t saved_sensor_ = 0;
  uint16_t saved_speed_ = 0;
};

TEST_F(MotorControllerTest, HighSensorWithZeroDivisorReturnsDefault) {
  EXPECT_EQ(computeControl(SENSOR_HIGH_THRESHOLD + 1, 0), DEFAULT_VALUE);
}

TEST_F(MotorControllerTest, SensorAtLowThresholdReturnsDefault) {
  EXPECT_EQ(computeControl(SENSOR_LOW_THRESHOLD, 10), DEFAULT_VALUE);
}

TEST_F(MotorControllerTest, SetWithoutArgumentPreservesMotorSpeed) {
  EXPECT_EQ(processCommand("SET", nullptr), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerTest, SetAboveIntMaximumPreservesMotorSpeed) {
  // On this ARM target, INT_MAX == LONG_MAX, so strtol reports ERANGE.
  EXPECT_EQ(processCommand("SET", "2147483648"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerTest, SetBelowIntMinimumPreservesMotorSpeed) {
  // On this ARM target, INT_MIN == LONG_MIN, so strtol reports ERANGE.
  EXPECT_EQ(processCommand("SET", "-2147483649"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerTest, SetWithNoDigitsPreservesMotorSpeed) {
  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerTest, SetWithTrailingCharactersPreservesMotorSpeed) {
  EXPECT_EQ(processCommand("SET", "12x"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerTest, ProcessorReadIncludesDefaultControl) {
  SENSOR_DATA = 42;

  EXPECT_EQ(processor("READ", nullptr), 42 + DEFAULT_VALUE);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerTest, UnknownCommandPreservesMotorSpeed) {
  EXPECT_EQ(processCommand("UNKNOWN", nullptr), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}
