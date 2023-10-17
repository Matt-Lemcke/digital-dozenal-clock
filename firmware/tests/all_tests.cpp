#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness_c.h"

/** Test groups */
TEST_GROUP_C_WRAPPER(FirstTestGroup)
{

};

TEST_C_WRAPPER(FirstTestGroup, FirstTest);


int main (int ac, char ** av){
    return RUN_ALL_TESTS(ac, av);
}