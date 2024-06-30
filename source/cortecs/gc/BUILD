load("@bazel_skylib//rules:common_settings.bzl", "string_flag")
load("@rules_cc//cc:defs.bzl", "cc_library")

# defines a cli flag that lets you configure the gc implementation
# --//source/cortecs/gc:impl=rc
# --//source/cortecs/gc:impl=tracing
string_flag(
    name = "impl",
    build_setting_default = "tracing",
)

config_setting(
    name = "rc",
    flag_values = {
        "//source/cortecs/gc:impl": "rc",
    },
)

config_setting(
    name = "tracing",
    flag_values = {
        "//source/cortecs/gc:impl": "tracing",
    },
)

cc_library(
    name = "gc",
    # selects sources to use based on the cli flag
    srcs = select({
        "//source/cortecs/gc:rc": glob(["rc/*.c"]),
        "//source/cortecs/gc:tracing": glob(["tracing/*.c"]),
    }),
    hdrs = glob(["*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["./"],
    visibility = [
        "//source/cortecs:__subpackages__",
        "//test/cortecs:__subpackages__",
    ],
    deps = [
        "//source/cortecs/common",
        "@flecs",
    ],
)
