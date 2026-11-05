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
TEST(GooTest, TestValueAboveMaxTreshold) {
  RecordProperty("cpptest_filename", __FILE__);
  RecordProperty("req", "REQ-123-ZERO");
 
  EXPECT_EQ(computeControl(600, 10), 60);
}

TEST(GooTest, ZeroDivisorProducesZeroControlAboveThreshold) {
  EXPECT_EQ(computeControl(600, 0), 0);
}

TEST(GooTest, ZeroDivisorPreservesControlBelowThreshold) {
  EXPECT_EQ(computeControl(200, 0), 600);
  EXPECT_EQ(computeControl(50, 0), 10);
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
