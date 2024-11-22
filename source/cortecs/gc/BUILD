# Part of the gc -> log -> string -> gc cycle
# Part of the gc <-> log cycle

# General method for breaking dependency cycles
# 1) Break targets into public-headers, sources, and the public target
#   1.1) public-headers are the headers that other packages depend on
#   1.2) sources are the implementation files of the package
#   1.3) the public target (the name of the package) is the target that
#        allows other packages to depend on the package.
#   1.4) public-headers and sources should only be visible to packages in the cycle.
#        they are for breaking the cycle and not for other packages to depend on.
#   1.5) Usually dependency cycles aren't in the public-headers.
#        Usually public-headers form a DAG and the back edge is due to
#        the sources depending on public headers.
# 2) Think of these targets as layers where public-headers is at the top,
#    then sources, and then the public targets
#   2.1) the only layer that should depend on other targets in the same layer is public-headers
#     2.1.1) the public-headers layer should still be a DAG
#   2.2) sources should only depend on the public headers of other packages in the cycle
#   2.2) public targets should never depend on other public targets in the cycle
#   2.3) public targets should depend on the public-headers and sources targets of the package
#   2.3) public targets should depend on all sources targets of other packages in the cycle
#     2.3.1) this ensures that the implementation is available when linking the package

# Dependencies table for the GC/Log/String cycle breaking
# ---------------------------------------------
# |          |    GC    |   Log    |  String  |
# |-------------------------------------------|
# |  public- |   none   |  String  |   none   |
# |  headers |          |  headers |          |
# |-------------------------------------------|
# |  sources |  String  |    GC    |    GC    |
# |          |  headers |  headers |  headers |
# |          |          |          |          |
# |          |   Log    |          |          |
# |          |  headers |          |          |
# |-------------------------------------------|
# |  public  |    GC    |   Log    |  String  |
# |  target  |  headers |  headers |  headers |
# |          |          |          |          |
# |          |    GC    |   Log    |  String  |
# |          |  sources |  sources |  sources |
# |          |          |          |          |
# |          |   Log    |    GC    |    GC    |
# |          |  sources |  sources |  sources |
# |          |          |          |          |
# |          |  String  |  String  |   Log    |
# |          |  sources |  sources |  sources |
# ---------------------------------------------

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
        "//source/cortecs/finalizer",
        "//source/cortecs/types",
    ],
)

cc_library(
    name = "sources",
    srcs = glob(["*.c"]),
    features = ["treat_warnings_as_errors"],
    visibility = [
        "//source/cortecs/log:__subpackages__",
        "//source/cortecs/string:__subpackages__",
    ],
    deps = [
        ":public-headers",
        "//source/cortecs/log:public-headers",
        "//source/cortecs/string:public-headers",
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
        "//source/cortecs/string:sources",
    ],
)
