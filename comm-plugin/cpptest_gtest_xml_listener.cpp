
#include "cpptest_gtest_xml_listener.h"

#include <gtest/gtest.h>

#include <limits>
#include <sstream>
#include <string>

extern "C" int cpptestSendTestDataPublic(
    const char* data,
    unsigned size);

namespace testing {
namespace internal {

// Implemented in the patched GoogleTest 1.17.0.
void GenerateXmlReport(
    std::ostream* stream,
    const UnitTest& unit_test);

}  // namespace internal
}  // namespace testing

namespace cpptest {
namespace {

int transport_status = 0;

class XmlTransportListener final
    : public ::testing::EmptyTestEventListener {
 public:
  void OnTestIterationEnd(
      const ::testing::UnitTest& unit_test,
      int iteration) override {
    (void)iteration;

    std::ostringstream stream;

    // Use the original GoogleTest 1.17.0
    // XML serializer.
    ::testing::internal::GenerateXmlReport(
        &stream, unit_test);

    if (!stream) {
      transport_status = -1;
      return;
    }

    const std::string xml = stream.str();

    if (xml.size() >
        std::numeric_limits<unsigned>::max()) {
      transport_status = -2;
      return;
    }

    // Exactly one transmission of the
    // complete XML document.
    transport_status = cpptestSendTestDataPublic(
        xml.data(),
        static_cast<unsigned>(xml.size()));
  }
};

}  // namespace

void InstallXmlTransportListener() {
  auto& listeners =
      ::testing::UnitTest::GetInstance()->listeners();

  listeners.Append(new XmlTransportListener());
}

int GetXmlTransportStatus() {
  return transport_status;
}

}  // namespace cpptest
