#include <span.h>
#include <stdint.h>
#include <unity.h>

static void test_span_compare(void) {
    for (uint32_t lines = 0; lines < 100; lines++) {
        for (uint32_t columns = 0; columns < 100; columns++) {
            cortecs_span_t span = {
                .lines = lines,
                .columns = columns,
            };
            TEST_ASSERT_EQUAL_INT32(0, cortecs_span_compare(span, span));
        }
    }

    for (uint32_t llines = 0; llines < 99; llines++) {
        for (uint32_t rlines = llines + 1; rlines < 100; rlines++) {
            for (uint32_t lcolumns = 0; lcolumns < 100; lcolumns++) {
                for (uint32_t rcolumns = 0; rcolumns < 100; rcolumns++) {
                    cortecs_span_t left = {
                        .lines = llines,
                        .columns = lcolumns,
                    };
                    cortecs_span_t right = {
                        .lines = rlines,
                        .columns = rcolumns,
                    };
                    TEST_ASSERT_EQUAL_INT32(-1, cortecs_span_compare(left, right));
                    TEST_ASSERT_EQUAL_INT32(1, cortecs_span_compare(right, left));
                }
            }
        }
    }

    for (uint32_t lines = 0; lines < 100; lines++) {
        for (uint32_t lcolumns = 0; lcolumns < 99; lcolumns++) {
            for (uint32_t rcolumns = lcolumns + 1; rcolumns < 100; rcolumns++) {
                cortecs_span_t left = {
                    .lines = lines,
                    .columns = lcolumns,
                };
                cortecs_span_t right = {
                    .lines = lines,
                    .columns = rcolumns,
                };
                TEST_ASSERT_EQUAL_INT32(-1, cortecs_span_compare(left, right));
                TEST_ASSERT_EQUAL_INT32(1, cortecs_span_compare(right, left));
            }
        }
    }
}

static void test_span_add(void) {
    for (uint32_t llines = 0; llines < 100; llines++) {
        for (uint32_t rlines = 1; rlines < 100; rlines++) {
            for (uint32_t lcolumns = 0; lcolumns < 100; lcolumns++) {
                for (uint32_t rcolumns = 0; rcolumns < 100; rcolumns++) {
                    cortecs_span_t left = {
                        .lines = llines,
                        .columns = lcolumns,
                    };
                    cortecs_span_t right = {
                        .lines = rlines,
                        .columns = rcolumns,
                    };

                    cortecs_span_t out = cortecs_span_add(left, right);
                    TEST_ASSERT_EQUAL_UINT32(llines + rlines, out.lines);
                    TEST_ASSERT_EQUAL_UINT32(rcolumns, out.columns);
                }
            }
        }
    }

    for (uint32_t llines = 0; llines < 100; llines++) {
        for (uint32_t lcolumns = 0; lcolumns < 99; lcolumns++) {
            for (uint32_t rcolumns = lcolumns + 1; rcolumns < 100; rcolumns++) {
                cortecs_span_t left = {
                    .lines = llines,
                    .columns = lcolumns,
                };
                cortecs_span_t right = {
                    .lines = 0,
                    .columns = rcolumns,
                };

                cortecs_span_t out = cortecs_span_add(left, right);
                TEST_ASSERT_EQUAL_UINT32(llines, out.lines);
                TEST_ASSERT_EQUAL_UINT32(lcolumns + rcolumns, out.columns);
            }
        }
    }
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_span_compare);
    RUN_TEST(test_span_add);
    return UNITY_END();
}

void setUp() {
    // required for unity
}

void tearDown() {
    // required for unity
}