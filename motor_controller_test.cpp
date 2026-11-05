#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <climits>
#include <cstdio>
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
    saved_motor_speed_ = MOTOR_SPEED;
    MOTOR_SPEED = 123;
  }

  void TearDown() override {
    MOTOR_SPEED = saved_motor_speed_;
  }

 private:
  uint16_t saved_motor_speed_ = 0;
};

}  // namespace

TEST_F(MotorControllerCoverageTest, HighSensorWithZeroDivisorUsesDefault) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeControl(static_cast<int>(SENSOR_HIGH_THRESHOLD) + 1, 0),
            DEFAULT_VALUE);
}

TEST_F(MotorControllerCoverageTest, SensorAtLowThresholdUsesDefault) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeControl(SENSOR_LOW_THRESHOLD, 10), DEFAULT_VALUE);
}

TEST_F(MotorControllerCoverageTest, ReadCommandReturnsSensorWithoutChangingSpeed) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("READ", ""), SENSOR_DATA);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTest, UnknownCommandIsRejected) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("UNKNOWN", ""), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTest, SetRejectsLongOverflow) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("SET", "9999999999999999999999999999999999999999"),
            -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTest, SetRejectsEmptyArgument) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTest, SetRejectsTrailingCharacters) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(processCommand("SET", "42x"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

// On targets where long and int have the same range, these inputs trigger
// ERANGE in strtol. The later parsed < INT_MIN / parsed > INT_MAX checks
// cannot independently become true on those targets.
TEST_F(MotorControllerCoverageTest, SetRejectsValueBelowIntMinimum) {
  RecordProperty("cpptest_filename", __FILE__);

  char argument[32] = {};
  std::snprintf(argument, sizeof(argument), "%lld",
                static_cast<long long>(INT_MIN) - 1LL);
  EXPECT_EQ(processCommand("SET", argument), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST_F(MotorControllerCoverageTest, SetRejectsValueAboveIntMaximum) {
  RecordProperty("cpptest_filename", __FILE__);

  char argument[32] = {};
  std::snprintf(argument, sizeof(argument), "%lld",
                static_cast<long long>(INT_MAX) + 1LL);
  EXPECT_EQ(processCommand("SET", argument), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}
