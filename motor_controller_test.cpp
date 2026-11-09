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
