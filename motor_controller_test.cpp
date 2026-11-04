#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <climits>
#include <string>
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

class MotorControllerMcdcTest : public ::testing::Test {
 protected:
  void SetUp() override {
    RecordProperty("cpptest_filename", __FILE__);
    sensor_ = SENSOR_DATA;
    speed_ = MOTOR_SPEED;
    MOTOR_SPEED = 123;
  }

  void TearDown() override {
    SENSOR_DATA = sensor_;
    MOTOR_SPEED = speed_;
  }

  void ExpectRejectedSet(const char* argument) {
    EXPECT_EQ(processCommand("SET", argument), -1);
    EXPECT_EQ(MOTOR_SPEED, 123);
  }

 private:
  uint16_t sensor_ = 0;
  uint16_t speed_ = 0;
};

TEST_F(MotorControllerMcdcTest, AboveHighThresholdWithZeroDivisor) {
  SENSOR_DATA = SENSOR_HIGH_THRESHOLD + 1;
  // processor supplies a zero divisor and then dispatches the READ command.
  EXPECT_EQ(processor("READ", ""), SENSOR_DATA);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerMcdcTest, AtLowThresholdUsesDefaultControl) {
  EXPECT_EQ(computeControl(SENSOR_LOW_THRESHOLD, 10), DEFAULT_VALUE);
}

TEST_F(MotorControllerMcdcTest, UnknownCommandIsRejected) {
  EXPECT_EQ(processCommand("UNKNOWN", ""), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerMcdcTest, SetWithoutDigitsIsRejected) {
  ExpectRejectedSet("");
}

TEST_F(MotorControllerMcdcTest, SetWithTrailingCharactersIsRejected) {
  ExpectRejectedSet("12x");
}

TEST_F(MotorControllerMcdcTest, SetWithLongOverflowIsRejected) {
  ExpectRejectedSet("9999999999999999999999999999999999999999");
}

TEST_F(MotorControllerMcdcTest, SetAboveIntMaximumIsRejected) {
  // On targets with wider long, this reaches parsed > INT_MAX. On this
  // 32-bit ARM target, strtol sets ERANGE first because LONG_MAX == INT_MAX.
  RecordProperty("target_long_max", std::to_string(LONG_MAX));
  RecordProperty("target_int_max", std::to_string(INT_MAX));
  const std::string argument =
      std::to_string(static_cast<long long>(INT_MAX) + 1);
  ExpectRejectedSet(argument.c_str());
}

TEST_F(MotorControllerMcdcTest, SetBelowIntMinimumIsRejected) {
  // Likewise, parsed < INT_MIN is reachable only when long is wider than int.
  RecordProperty("target_long_min", std::to_string(LONG_MIN));
  RecordProperty("target_int_min", std::to_string(INT_MIN));
  const std::string argument =
      std::to_string(static_cast<long long>(INT_MIN) - 1);
  ExpectRejectedSet(argument.c_str());
}
