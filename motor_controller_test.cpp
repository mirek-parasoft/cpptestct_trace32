#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "motor_controller.h"
#include "comm-plugin/cpptest_gtest_xml_listener.h"

int main(int argc, char** argv) {
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

TEST(BasicTests, ZeroDivisorUsesDefaultControl) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeControl(600, 0), DEFAULT_VALUE);
}

TEST(BasicTests, SetMotorSpeedZero) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-POSITIVE");

  EXPECT_EQ(processCommand("SET", "0"), 0);
}

TEST(BasicTests, canStartMotor) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-POSITIVE");

  EXPECT_TRUE(canStartMotor(true, true, false));
  EXPECT_FALSE(canStartMotor(false, true, false));
}

TEST(BasicTests, ProcessorAddsDefaultControlToCommandResult) {
  RecordProperty("cpptest_filename", __FILE__);

  const uint16_t originalSensorData = SENSOR_DATA;
  SENSOR_DATA = 42;
  const int readResult = processor("READ", "");
  const int unknownCommandResult = processor("UNKNOWN", "");
  SENSOR_DATA = originalSensorData;

  EXPECT_EQ(readResult, 52);
  EXPECT_EQ(unknownCommandResult, 9);
}

TEST(BasicTests, StartupSpeedUsesPresetOnlyWhenEnabled) {
  RecordProperty("cpptest_filename", __FILE__);

  EXPECT_EQ(computeStartupSpeed(true, 250), 250);
  EXPECT_EQ(computeStartupSpeed(false, 250), 0);
}
