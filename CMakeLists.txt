cmake_minimum_required(VERSION 3.18)

set(CMAKE_SYSTEM_NAME Generic)

set(ARM_CPU_FLAGS "-mcpu=cortex-A32")
set(CMAKE_C_FLAGS_INIT "${ARM_CPU_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${ARM_CPU_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${ARM_CPU_FLAGS}")

project(CovGoogleTest LANGUAGES C CXX ASM)


if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24")
	cmake_policy(SET CMP0135 NEW)
endif()

# starting from v. 1.17.0 GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GOOGLETEST_SUBPATH "googletest/googletest.zip")

set(gtest_disable_pthreads ON CACHE BOOL
    "Disable pthreads" FORCE)

add_compile_definitions(
    GTEST_HAS_PTHREAD=0
    GTEST_HAS_FILE_SYSTEM=0
    GTEST_HAS_STREAM_REDIRECTION=0
    GTEST_HAS_POSIX_RE=0
)

if(DEFINED GOOGLETEST_URL)
# use the specified URL
elseif(CPPTEST_HOME)
set(GOOGLETEST_URL "${CPPTEST_HOME}/${GOOGLETEST_SUBPATH}")
elseif(DEFINED ENV{CPPTEST_HOME})
set(GOOGLETEST_URL "$ENV{CPPTEST_HOME}/${GOOGLETEST_SUBPATH}")
elseif (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../${GOOGLETEST_SUBPATH}")
set(GOOGLETEST_URL "${CMAKE_CURRENT_LIST_DIR}/../../${GOOGLETEST_SUBPATH}")
else()
message(FATAL_ERROR "Unable to locate GoogleTest package - please set CPPTEST_HOME (or GOOGLETEST_URL).")
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL "${GOOGLETEST_URL}"
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(cmake/GoogleTestFixes.cmake)
target_compile_definitions(gtest PRIVATE CPPTEST_GTEST_XML_TRANSPORT)
enable_testing()

set(LAUTERBACH_T32_EXECUTABLE
    "/opt/lauterbach/t32/bin/pc_linux64/t32marm"
    CACHE FILEPATH "Path to the Lauterbach T32 ARM debugger executable")

if(CPPTEST_COVERAGE)
  include(cpptest-coverage.cmake)
endif()

add_subdirectory(comm-plugin)

add_executable(
  goo_test
  startup.S
  motor_controller_test.cpp
  motor_controller.cpp
  syscalls.cpp
)

set_source_files_properties(startup.S PROPERTIES
  COMPILE_OPTIONS "-Wall;-g3"
)

target_link_options(goo_test PRIVATE
  "-T${CMAKE_CURRENT_SOURCE_DIR}/gcc.ld"
  "--specs=nosys.specs"
)
set_property(TARGET goo_test APPEND PROPERTY LINK_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/gcc.ld"
)

target_link_libraries(
  goo_test
  comm-plugin
  gtest
  gmock
)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

# cpptest.cmm writes the GoogleTest XML report to .build/gtest_results.xml
add_test(
  NAME goo_test
  COMMAND "${Python3_EXECUTABLE}"
          "${CMAKE_CURRENT_SOURCE_DIR}/run_t32_tests.py"
          --t32 "${LAUTERBACH_T32_EXECUTABLE}"
          --script "${CMAKE_CURRENT_SOURCE_DIR}/cpptest.cmm"
          --xml "${CMAKE_CURRENT_SOURCE_DIR}/.build/gtest_results.xml"
)
set_tests_properties(goo_test PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
