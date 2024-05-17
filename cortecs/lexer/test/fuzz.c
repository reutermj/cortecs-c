#include "fuzz.h"

#include <span.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <tokens.h>

#include "util.h"

void cortecs_lexer_test_fuzz(cortecs_lexer_test_config_t config) {
    for (uint32_t length = 5; length < 100; length++) {
        for (uint32_t offset = 0; offset < 100; offset++) {
            for (int times = 0; times < 100; times++) {
                char *in = calloc(offset + length + 1, sizeof(char));
                char *gold = calloc(length + 1, sizeof(char));
                while (true) {
                    in[offset] = config.get_first_char(rand());
                    gold[0] = in[offset];
                    for (int i = 1; i < length; i++) {
                        in[offset + i] = config.get_other_chars(rand());
                        gold[i] = in[offset + i];
                    }

                    if (config.should_skip_token(gold, length)) {
                        continue;
                    }

                    break;
                }
                in[offset + length] = config.get_finalizer_char(rand());
                cortecs_lexer_test(in, offset, gold, config.tag);
                free(in);
                free(gold);
            }
        }
    }
}
