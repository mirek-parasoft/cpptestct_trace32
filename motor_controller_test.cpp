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

namespace {

class AdditionalMotorControllerTests : public ::testing::Test {
 protected:
  void SetUp() override {
    saved_sensor_ = SENSOR_DATA;
    saved_speed_ = MOTOR_SPEED;
    SENSOR_DATA = 0;
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

TEST_F(AdditionalMotorControllerTests, ProcessorReadsHighSensorWithZeroDivisor) {
  RecordProperty("req", "REQ-30-FAULT-MODE");
  RecordProperty("cpptest_filename", __FILE__);
  SENSOR_DATA = 600;

  EXPECT_EQ(processor("READ", ""), 600 + DEFAULT_VALUE);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(AdditionalMotorControllerTests, SensorAtLowThresholdUsesDefault) {
  RecordProperty("req", "REQ-30-FAULT-MODE");
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeControl(SENSOR_LOW_THRESHOLD, 10), DEFAULT_VALUE);
}

TEST_F(AdditionalMotorControllerTests, UnknownCommandIsRejected) {
  RecordProperty("req", "REQ-30-FAULT-MODE");
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("UNKNOWN", "0"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(AdditionalMotorControllerTests, SetWithoutDigitsIsRejected) {
  RecordProperty("req", "REQ-30-FAULT-MODE");
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(AdditionalMotorControllerTests, SetWithTrailingCharactersIsRejected) {
  RecordProperty("req", "REQ-30-FAULT-MODE");
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("SET", "12x"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(AdditionalMotorControllerTests, SetWithPositiveOverflowIsRejected) {
  RecordProperty("req", "REQ-30-FAULT-MODE");
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("SET", "999999999999999999999999999999"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(AdditionalMotorControllerTests, SetWithNegativeOverflowIsRejected) {
  RecordProperty("req", "REQ-30-FAULT-MODE");
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("SET", "-999999999999999999999999999999"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

}  // namespace
