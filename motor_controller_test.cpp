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

TEST(GooTest, ZeroDivisorUsesDefaultValue) {
  EXPECT_EQ(computeControl(600, 0), DEFAULT_VALUE);
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

TEST(GooTest, SetMotorSpeedValidNumber) {
  EXPECT_EQ(processCommand("SET", "42"), 42);
  EXPECT_EQ(MOTOR_SPEED, 42);
}

TEST(GooTest, InvalidMotorSpeedLeavesRegisterUnchanged) {
  setMotorSpeed(123);
  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(processCommand("SET", "invalid"), -1);
  EXPECT_EQ(processCommand("SET", "42invalid"), -1);
  EXPECT_EQ(processCommand("SET", "999999999999999999999999999999"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}

TEST(GooTest, SensorAtLowThresholdUsesDefaultValue) {
  EXPECT_EQ(computeControl(SENSOR_LOW_THRESHOLD, 10), DEFAULT_VALUE);
}

TEST(GooTest, ProcessorReadReturnsSensorPlusControl) {
  const uint16_t originalSensor = SENSOR_DATA;
  SENSOR_DATA = SENSOR_LOW_THRESHOLD;

  EXPECT_EQ(processor("READ", ""),
            static_cast<int>(SENSOR_DATA) + DEFAULT_VALUE);

  SENSOR_DATA = originalSensor;
}

TEST(GooTest, UnknownCommandLeavesMotorSpeedUnchanged) {
  const uint16_t originalSpeed = MOTOR_SPEED;
  setMotorSpeed(123);

  EXPECT_EQ(processCommand("UNKNOWN", "42"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);

  setMotorSpeed(originalSpeed);
}

// On this target, strtol cannot return a value outside int's range without
// setting errno. The subsequent val < INT_MIN and val > INT_MAX conditions
// therefore cannot evaluate true after the errno check succeeds.
static_assert(sizeof(long) == sizeof(int),
              "Reassess numeric bounds MC/DC coverage for wider long targets");
