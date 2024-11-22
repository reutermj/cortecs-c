# Part of the gc -> log -> string -> gc cycle
# see: //source/cortecs/gc for more info

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
        "//source/cortecs/finalizer",
        "//source/cortecs/mangle",
        "//source/cortecs/types",
    ],
)

cc_library(
    name = "sources",
    srcs = glob(["*.c"]),
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
