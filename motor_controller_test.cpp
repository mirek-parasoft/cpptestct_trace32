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

TEST(BasicTests, ZeroDivisorAboveThreshold) {
  EXPECT_EQ(computeControl(600, 0), 0);
}

TEST(BasicTests, ZeroDivisorBelowThreshold) {
  EXPECT_EQ(computeControl(200, 0), 600);
}

TEST(BasicTests, SetMotorSpeedZero) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-POSITIVE");

  EXPECT_EQ(processCommand("SET", "0"), 0);
}

TEST(BasicTests, SetMotorSpeedMaximum) {
  EXPECT_EQ(processCommand("SET", "65535"), 65535);
  EXPECT_EQ(MOTOR_SPEED, 65535);
}

TEST(BasicTests, InvalidMotorSpeedLeavesRegisterUnchanged) {
  const char* const invalid_args[] = {
      "", "abc", "12abc", "-1", "65536",
      "999999999999999999999999999999999999", nullptr};
  for (const char* const arg : invalid_args) {
    MOTOR_SPEED = 123;
    EXPECT_EQ(processCommand("SET", arg), -1);
    EXPECT_EQ(MOTOR_SPEED, 123);
  }
}

class MotorControllerCoverageTests : public ::testing::Test {
 protected:
  void TearDown() override {
    SENSOR_DATA = saved_sensor_;
    MOTOR_SPEED = saved_speed_;
  }

 private:
  const uint16_t saved_sensor_ = SENSOR_DATA;
  const uint16_t saved_speed_ = MOTOR_SPEED;
};

TEST_F(MotorControllerCoverageTests, ProcessorReadAtLowThresholdUsesDefaultControl) {
  RecordProperty("req", "REQ-33-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);

  SENSOR_DATA = SENSOR_LOW_THRESHOLD;
  MOTOR_SPEED = 321;

  EXPECT_EQ(processor("READ", nullptr),
            static_cast<int>(SENSOR_LOW_THRESHOLD) + DEFAULT_VALUE);
  EXPECT_EQ(SENSOR_DATA, SENSOR_LOW_THRESHOLD);
  EXPECT_EQ(MOTOR_SPEED, 321);
}

TEST_F(MotorControllerCoverageTests, UnknownCommandLeavesMotorSpeedUnchanged) {
  RecordProperty("req", "REQ-33-FAULT-TESTING");
  RecordProperty("cpptest_filename", __FILE__);

  MOTOR_SPEED = 321;

  EXPECT_EQ(processCommand("UNKNOWN", nullptr), -1);
  EXPECT_EQ(MOTOR_SPEED, 321);
}
