#ifndef CORTECS_LEXER_TEST_TEST_H
#define CORTECS_LEXER_TEST_TEST_H

#include "util.h"

void cortecs_lexer_test_fuzz(cortecs_lexer_test_config_t config);
void cortecs_lexer_test_exhaustive(cortecs_lexer_test_config_t config);

#endif