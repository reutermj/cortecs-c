cc_library(
    name = "lsp",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["./"],
    visibility = ["//:__subpackages__"],
    deps = [
        "//source/common",
        "@cJSON",
    ],
)
