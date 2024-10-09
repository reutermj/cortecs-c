cc_library(
    name = "hashmap",
    hdrs = glob([
        "public-headers/cortecs/stdlib/*.c",
        "public-headers/cortecs/stdlib/*.h",
    ]),
    features = ["treat_warnings_as_errors"],
    includes = ["public-headers/"],
    visibility = ["//visibility:public"],
    deps = [
        "//source/common",
        "//source/cortecs/array",
        "//source/cortecs/gc",
    ],
)
