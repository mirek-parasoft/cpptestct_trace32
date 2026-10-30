
#ifndef CPPTEST_GTEST_XML_LISTENER_H
#define CPPTEST_GTEST_XML_LISTENER_H

namespace cpptest {

void InstallXmlTransportListener();

// Returns the most recent transport status.
// Zero means success, assuming that convention
// is used by cpptestSendTestDataPublic().
int GetXmlTransportStatus();

}  // namespace cpptest

#endif
