cc_library(
    name = "lexer",
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
        "//source/cortecs/gc",
        "//third_party/icu4c/common:errorcode",
        "//third_party/icu4c/common:headers",
        "//third_party/icu4c/common:uchar",
        "//third_party/icu4c/common:utext",
    ],
)
