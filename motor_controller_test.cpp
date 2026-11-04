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

namespace {

class MotorControllerCoverageTest : public ::testing::Test {
 protected:
  void SetUp() override {
    saved_sensor_ = SENSOR_DATA;
    saved_speed_ = MOTOR_SPEED;
    SENSOR_DATA = 0;
    MOTOR_SPEED = 123;
    RecordProperty("cpptest_filename", __FILE__);
  }

  void TearDown() override {
    SENSOR_DATA = saved_sensor_;
    MOTOR_SPEED = saved_speed_;
  }

 private:
  uint16_t saved_sensor_ = 0;
  uint16_t saved_speed_ = 0;
};

TEST_F(MotorControllerCoverageTest, HighSensorWithZeroDivisorUsesDefault) {
  EXPECT_EQ(computeControl(600, 0), DEFAULT_VALUE);
}

TEST_F(MotorControllerCoverageTest, SensorAtLowThresholdUsesDefault) {
  EXPECT_EQ(computeControl(SENSOR_LOW_THRESHOLD, 10), DEFAULT_VALUE);
}

TEST_F(MotorControllerCoverageTest, ReadCommandThroughProcessorUsesSensorAndControl) {
  SENSOR_DATA = 600;
  EXPECT_EQ(processor("READ", nullptr), 600 + DEFAULT_VALUE);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTest, UnknownCommandIsRejected) {
  EXPECT_EQ(processCommand("UNKNOWN", nullptr), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTest, SetWithoutArgumentIsRejected) {
  EXPECT_EQ(processCommand("SET", nullptr), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTest, SetWithEmptyArgumentIsRejected) {
  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTest, SetWithTrailingCharactersIsRejected) {
  EXPECT_EQ(processCommand("SET", "12x"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

// On the ARM target, long and int are both 32 bits. strtol reports ERANGE
// for these values before the parsed < INT_MIN / parsed > INT_MAX checks.
TEST_F(MotorControllerCoverageTest, SetAboveIntRangeIsRejected) {
  EXPECT_EQ(processCommand("SET", "2147483648"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTest, SetBelowIntRangeIsRejected) {
  EXPECT_EQ(processCommand("SET", "-2147483649"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

}  // namespace
