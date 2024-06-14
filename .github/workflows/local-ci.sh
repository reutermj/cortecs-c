#!/bin/bash

bazel clean
rm compile_commands.json

CC=clang bazel build //...
CC=clang bazel run @hedron_compile_commands//:refresh_all
clang-tidy --config-file ./source/.clang-tidy -p ./ $(find source/. -name "*.c")
clang-tidy --config-file ./test/.clang-tidy -p ./ $(find test/. -name "*.c")
CC=gcc bazel coverage --instrumentation_filter //source/... --combined_report=lcov //test/...
genhtml --branch-coverage --output genhtml "$(bazel info output_path)/_coverage/_coverage_report.dat"