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

TEST(BasicTests, SetMotorSpeedZero) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-POSITIVE");

  EXPECT_EQ(processCommand("SET", "0"), 0);
}

TEST(BasicTests, SetMotorSpeedMaximum) {
  EXPECT_EQ(processCommand("SET", "65535"), 65535);
  EXPECT_EQ(MOTOR_SPEED, 65535);
}

TEST(BasicTests, RejectInvalidMotorSpeed) {
  setMotorSpeed(123);
  EXPECT_EQ(processCommand("SET", ""), -1);
  EXPECT_EQ(processCommand("SET", "abc"), -1);
  EXPECT_EQ(processCommand("SET", "12abc"), -1);
  EXPECT_EQ(processCommand("SET", "-1"), -1);
  EXPECT_EQ(processCommand("SET", "65536"), -1);
  EXPECT_EQ(processCommand("SET", "999999999999999999999999"), -1);
  EXPECT_EQ(MOTOR_SPEED, 123);
}
