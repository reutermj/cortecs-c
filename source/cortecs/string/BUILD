# Part of the gc -> log -> string -> gc cycle
# public-headers and sources targets are for breaking the cycles.
# these targets are only for use by targets in the cycle.
# for other targets, use the string target to depend on this.

cc_library(
    name = "public-headers",
    hdrs = glob(["public-headers/cortecs/*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["public-headers/"],
    visibility = [
        "//source/cortecs/gc:__subpackages__",
        "//source/cortecs/log:__subpackages__",
    ],
    deps = [
        "//source/cortecs/array",
        "//source/cortecs/finalizer",
    ],
)

cc_library(
    name = "sources",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    features = ["treat_warnings_as_errors"],
    visibility = [
        "//source/cortecs/gc:__subpackages__",
        "//source/cortecs/log:__subpackages__",
    ],
    deps = [
        ":public-headers",
        "//source/cortecs/gc:public-headers",
    ],
)

cc_library(
    name = "string",
    visibility = ["//visibility:public"],
    deps = [
        ":public-headers",
        ":sources",
        "//source/cortecs/gc:sources",
        "//source/cortecs/log:sources",
    ],
)
