#include "CppUTest/TestHarness_c.h"
#include "clock.h"

TEST_GROUP_C_SETUP(FirstTestGroup) {
};

TEST_C(FirstTestGroup, FirstTest) {
    CHECK_C(timer10Callback());
}
