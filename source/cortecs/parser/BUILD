cc_library(
    name = "parser",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["./"],
    visibility = [
        "//source/cortecs:__subpackages__",
        "//test/cortecs:__subpackages__",
    ],
    deps = [
        "//source/common",
        "//source/cortecs/common",
        "//source/cortecs/lexer",
        "@flecs",
    ],
)
