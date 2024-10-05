#!/bin/bash

bazel clean
rm compile_commands.json

# TODO: these will eventually overflow the shell line length
clang-format --dry-run --Werror $(find source/ -iname '*.h' -o -iname '*.c')
clang-format --dry-run --Werror $(find test/ -iname '*.h' -o -iname '*.c')
CC=clang bazel build //...
CC=clang bazel run @hedron_compile_commands//:refresh_all
clang-tidy --config-file ./source/.clang-tidy -p ./ $(find source/. -name "*.c")
clang-tidy --config-file ./test/.clang-tidy -p ./ $(find test/. -name "*.c")
CC=gcc bazel coverage --instrumentation_filter //source/... --combined_report=lcov //test/...
genhtml --branch-coverage --output genhtml "$(bazel info output_path)/_coverage/_coverage_report.dat"