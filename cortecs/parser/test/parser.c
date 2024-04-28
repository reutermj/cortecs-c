#include <lexer.h>
#include <stdint.h>
#include <string.h>
#include <unity.h>

void setUp() {
    // required for unity
}

void tearDown() {
    // required for unity
}

void test_noop() {
    // noop
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_noop);
    return UNITY_END();
}