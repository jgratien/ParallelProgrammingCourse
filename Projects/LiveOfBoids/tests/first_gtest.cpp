#include <cmath>
#include <iostream>
#include "gtest/gtest.h"

// blank namespace to avoid implementation collision
namespace {

TEST(FirstGTest, AssertCanary) {
  // ASSERT_* is a strong assertion: must be true or abort the execution
  ASSERT_EQ(1, 1) << "Canary could not fail!";

  // this point will never be executed if an ASSERT failed before
}

TEST(FirstGTest, ExpectCanary) {
  // EXPECT_* is a weak assertion: should be true or report an error
  EXPECT_EQ(1, 1) << "Canary could not fail!";  // with an additional message

  // this point will be executed even if an EXPECT failed before
}

}  // namespace
