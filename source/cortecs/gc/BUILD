cc_library(
    name = "gc",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["./"],
    visibility = [
        "//source:__subpackages__",
        "//test:__subpackages__",
    ],
    deps = [
        "//source/common",
        "//source/cortecs/common",
        "@flecs",
    ],
)
