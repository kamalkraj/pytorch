#include <iostream>
#include <memory>

#include <gtest/gtest.h>
#include "caffe2/core/init.h"
#include "caffe2/core/logging.h"

namespace caffe2 {
namespace {
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
bool gTestInitFunctionHasBeenRun = false;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
bool gTestFailInitFunctionHasBeenRun = false;

bool TestInitFunction(int*, char***) {
  gTestInitFunctionHasBeenRun = true;
  return true;
}

bool TestFailInitFunction(int*, char***) {
  gTestFailInitFunctionHasBeenRun = true;
  return false;
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
REGISTER_CAFFE2_INIT_FUNCTION(
    TestInitFunction,
    &TestInitFunction,
    "Just a test to see if GlobalInit invokes "
    "registered functions correctly.");

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
int dummy_argc = 1;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
const char* dummy_name = "foo";
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-pro-type-const-cast)
char** dummy_argv = const_cast<char**>(&dummy_name);
} // namespace

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TEST(InitTest, TestInitFunctionHasRun) {
  caffe2::GlobalInit(&dummy_argc, &dummy_argv);
  EXPECT_TRUE(gTestInitFunctionHasBeenRun);
  EXPECT_FALSE(gTestFailInitFunctionHasBeenRun);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TEST(InitTest, CanRerunGlobalInit) {
  caffe2::GlobalInit(&dummy_argc, &dummy_argv);
  EXPECT_TRUE(caffe2::GlobalInit(&dummy_argc, &dummy_argv));
}

void LateRegisterInitFunction() {
  ::caffe2::InitRegisterer testInitFunc(
      TestInitFunction, false, "This should succeed but warn");
}

void LateRegisterEarlyInitFunction() {
  ::caffe2::InitRegisterer testSecondInitFunc(
      TestInitFunction, true, "This should fail for early init");
}

void LateRegisterFailInitFunction() {
  ::caffe2::InitRegisterer testSecondInitFunc(
      TestFailInitFunction, false, "This should fail for failed init");
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TEST(InitTest, FailLateRegisterInitFunction) {
  caffe2::GlobalInit(&dummy_argc, &dummy_argv);
  LateRegisterInitFunction();
  // NOLINTNEXTLINE(hicpp-avoid-goto,cppcoreguidelines-avoid-goto)
  EXPECT_THROW(LateRegisterEarlyInitFunction(), ::c10::Error);
  // NOLINTNEXTLINE(hicpp-avoid-goto,cppcoreguidelines-avoid-goto)
  EXPECT_THROW(LateRegisterFailInitFunction(), ::c10::Error);
  EXPECT_TRUE(gTestInitFunctionHasBeenRun);
  EXPECT_TRUE(gTestFailInitFunctionHasBeenRun);
}

} // namespace caffe2
