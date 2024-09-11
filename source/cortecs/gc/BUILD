# Part of the gc -> log -> string -> gc cycle
# Part of the gc <-> log cycle
# public-headers and sources targets are for breaking the cycles.
# these targets are only for use by targets in the cycle.
# for other targets, use the gc target to depend on this.

cc_library(
    name = "public-headers",
    hdrs = glob(["public-headers/cortecs/*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["public-headers/"],
    visibility = [
        "//source/cortecs/log:__subpackages__",
        "//source/cortecs/string:__subpackages__",
    ],
    deps = [
        "//source/common",
        "//source/cortecs/array",
    ],
)

cc_library(
    name = "sources",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    features = ["treat_warnings_as_errors"],
    visibility = [
        "//source/cortecs/log:__subpackages__",
        "//source/cortecs/string:__subpackages__",
    ],
    deps = [
        ":public-headers",
        "//source/cortecs/log:public-headers",
        "//source/cortecs/world",
        "@flecs",
    ],
)

cc_library(
    name = "gc",
    visibility = ["//visibility:public"],
    deps = [
        ":public-headers",
        ":sources",
        "//source/cortecs/log:sources",
    ],
)
