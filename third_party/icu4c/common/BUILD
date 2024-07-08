# © 2021 and later: Unicode, Inc. and others.
# License & terms of use: http://www.unicode.org/copyright.html

# This file defines Bazel targets for a subset of ICU4C "common" library header and source files.
# The configuration of dependencies among targets is strongly assisted by the
# file in depstest that maintains such information, at
# icu4c/source/test/depstest/dependencies.txt .

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

# When compiling code in the `common` dir, the constant
# `U_COMMON_IMPLEMENTATION` needs to be defined. See
# https://unicode-org.github.io/icu/userguide/howtouseicu#c-with-your-own-build-system .

# If linker errors occur, then this may be a sign that the dependencies were
# not specified correctly. Use dependencies.txt in depstest for assistance. See
# https://stackoverflow.com/q/66111709/2077918 .

cc_library(
    name = "headers",
    hdrs = glob(
        [
            "unicode/*.h",  # public
            "*.h",  # internal
        ],
        # Instead of using these checked-in files, our Bazel build process
        # regenerates them and then uses the new versions.
        # Same list of .h files as in icu4c/source/data/unidata/clean.sh.
        exclude = [
            "norm2_nfc_data.h",
            "propname_data.h",
            "*_props_data.h",
        ],
    ),
    copts = ["-std=c++17"],
    # We need to add includes in order to preserve existing source files'
    # include directives that use traditional paths, not paths relative to
    # Bazel workspace:
    # https://stackoverflow.com/a/65635893/2077918
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
)

cc_library(
    name = "platform",
    srcs = [
        "appendable.cpp",
        "charstr.cpp",
        "cmemory.cpp",
        "cstring.cpp",
        "cwchar.cpp",
        "putil.cpp",
        "sharedobject.cpp",
        "stringpiece.cpp",
        "ucln_cmn.cpp",
        "udataswp.cpp",
        "uinvchar.cpp",
        "umath.cpp",
        "umutex.cpp",
        "unistr.cpp",
        "uobject.cpp",
        "ustrfmt.cpp",
        "ustring.cpp",
        "ustrtrns.cpp",
        "utf_impl.cpp",
        "utrace.cpp",
    ],
    copts = ["-std=c++17"],
    linkopts = ["-ldl"],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        # omit other deps b/c they are sys symbols
    ],
)

cc_library(
    name = "utrie",
    srcs = ["utrie.cpp"],
    copts = ["-std=c++17"],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [":platform"],
)

cc_library(
    name = "utrie2",
    srcs = ["utrie2.cpp"],
    copts = ["-std=c++17"],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [":platform"],
)

cc_library(
    name = "utrie2_builder",
    srcs = ["utrie2_builder.cpp"],
    copts = ["-std=c++17"],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":platform",
        ":utrie",
        ":utrie2",
    ],
)

cc_library(
    name = "ucptrie",
    srcs = ["ucptrie.cpp"],
    copts = ["-std=c++17"],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [":platform"],
)

cc_library(
    name = "umutablecptrie",
    srcs = ["umutablecptrie.cpp"],
    copts = ["-std=c++17"],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [":ucptrie"],
)

cc_library(
    name = "bytestrie",
    srcs = ["bytestrie.cpp"],
    copts = ["-std=c++17"],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [":platform"],
)

cc_library(
    name = "bytestriebuilder",
    srcs = ["bytestriebuilder.cpp"],
    copts = ["-std=c++17"],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":bytestrie",
        ":sort",
        ":stringtriebuilder",
    ],
)

cc_library(
    name = "stringtriebuilder",
    srcs = ["stringtriebuilder.cpp"],
    copts = ["-std=c++17"],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":uhash",
    ],
)

cc_library(
    name = "uhash",
    srcs = [
        "uhash.cpp",
    ],
    hdrs = [
        "uhash.h",
    ],
    copts = ["-std=c++17"],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
    ],
)

cc_library(
    name = "errorcode",
    srcs = [
        "errorcode.cpp",
    ],
    hdrs = [
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":platform",
        ":utypes",
    ],
)

cc_library(
    name = "utypes",
    srcs = [
        "utypes.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
    ],
)

cc_library(
    name = "uniset",
    srcs = [
        "bmpset.cpp",
        "unifilt.cpp",
        "unifunct.cpp",
        "uniset.cpp",
        "unisetspan.cpp",
        "usetiter.cpp",
        "util.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":patternprops",
        ":uvector",
    ],
)

cc_library(
    name = "patternprops",
    srcs = [
        "patternprops.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
    ],
)

cc_library(
    name = "propsvec",
    srcs = [
        "propsvec.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":sort",
        ":utrie2_builder",
    ],
)

cc_library(
    name = "propname",
    srcs = [
        "propname.cpp",
        "propname_data.h",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":bytestrie",
        ":headers",
    ],
)

# Note: The cc_library target names "uvector32" and "uvector64" match the
# dependencies.txt group names, but the filenames are "uvectr32.*"/"uvectr64.*".
cc_library(
    name = "uvector32",
    srcs = [
        "uvectr32.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":platform",
    ],
)

cc_library(
    name = "uvector64",
    srcs = [
        "uvectr64.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":platform",
    ],
)

cc_library(
    name = "sort",
    srcs = [
        "uarrsort.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
    ],
)

cc_library(
    name = "uvector",
    srcs = [
        "uvector.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":platform",
        ":sort",
    ],
)

cc_library(
    name = "breakiterator",
    srcs = [
        "brkeng.cpp",
        "brkiter.cpp",
        "dictbe.cpp",
        "dictionarydata.cpp",
        "filteredbrk.cpp",
        "lstmbe.cpp",
        "mlbe.cpp",
        "rbbi.cpp",
        "rbbi_cache.cpp",
        "rbbidata.cpp",
        "rbbinode.cpp",
        "rbbirb.cpp",
        "rbbiscan.cpp",
        "rbbisetb.cpp",
        "rbbistbl.cpp",
        "rbbitblb.cpp",
        "ubrk.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":bytestrie",
        ":headers",
        ":normlzr",
        ":resourcebundle",
        ":schriter",
        ":service_registration",
        ":ucharstrie",
        ":ucharstriebuilder",
        ":uhash",
        ":uniset_core",
        ":uniset_props",
        ":ustack",
        ":utext",
        ":utrie2_builder",
        ":uvector32",
    ],
)

cc_library(
    name = "bytesinkutil",
    srcs = [
        "bytesinkutil.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":bytestream",
        ":edits",
        ":headers",
    ],
)

cc_library(
    name = "bytestream",
    srcs = [
        "bytestream.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":platform",
    ],
)

cc_library(
    name = "canonical_iterator",
    srcs = [
        "caniter.cpp",
    ],
    copts = ["-std=c++17"],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":normalizer2",
        ":usetiter",
    ],
)

cc_library(
    name = "characterproperties",
    srcs = [
        "characterproperties.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":emojiprops",
        ":headers",
        ":ucptrie",
        ":umutablecptrie",
        ":uniset_core",
        ":uprops",
    ],
)

cc_library(
    name = "chariter",
    srcs = [
        "chariter.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":platform",
    ],
)

cc_library(
    name = "edits",
    srcs = [
        "edits.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":icu_utility",
        ":platform",
    ],
)

cc_library(
    name = "filterednormalizer2",
    srcs = [
        "filterednormalizer2.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":normalizer2",
    ],
)

cc_library(
    name = "hashtable",
    srcs = [
        "uhash_us.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":uhash",
    ],
)

cc_library(
    name = "icu_utility",
    srcs = [
        "util.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":patternprops",
        ":platform",
    ],
)

cc_library(
    name = "loadednormalizer2",
    srcs = [
        "loadednormalizer2impl.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":normalizer2",
    ],
)

cc_library(
    name = "locale_display_names",
    srcs = [
        "locdispnames.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":locresdata",
    ],
)

cc_library(
    name = "locresdata",
    srcs = [
        "locresdata.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":resourcebundle",
    ],
)

cc_library(
    name = "normlzr",
    srcs = [
        "normlzr.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":filterednormalizer2",
        ":headers",
        ":schriter",
        ":uniset_props",
    ],
)

cc_library(
    name = "parsepos",
    srcs = [
        "parsepos.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":platform",
    ],
)

cc_library(
    name = "resourcebundle",
    srcs = [
        "localebuilder.cpp",
        "locavailable.cpp",
        "locbased.cpp",
        "locid.cpp",
        "loclikely.cpp",
        "loclikelysubtags.cpp",
        "locmap.cpp",
        "lsr.cpp",
        "resbund.cpp",
        "resource.cpp",
        "uloc.cpp",
        "uloc_keytype.cpp",
        "uloc_tag.cpp",
        "ulocale.cpp",
        "ulocbuilder.cpp",
        "uresbund.cpp",
        "uresdata.cpp",
        "wintz.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":bytesinkutil",
        ":errorcode",
        ":headers",
        ":propname",
        ":sort",
        ":stringenumeration",
        ":ucol_swp",
        ":udata",
        ":uhash",
        ":uscript_props",
        ":uvector",
    ],
)

cc_library(
    name = "schriter",
    srcs = [
        "schriter.cpp",
        "uchriter.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":chariter",
        ":headers",
    ],
)

cc_library(
    name = "service_registration",
    srcs = [
        "locutil.cpp",
        "serv.cpp",
        "servlk.cpp",
        "servlkf.cpp",
        "servls.cpp",
        "servnotf.cpp",
        "servrbf.cpp",
        "servslkf.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":hashtable",
        ":headers",
        ":locale_display_names",
        ":resourcebundle",
        ":uvector",
    ],
)

cc_library(
    name = "stringenumeration",
    srcs = [
        "uenum.cpp",
        "ustrenum.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":platform",
    ],
)

cc_library(
    name = "ubidi_props",
    srcs = [
        "ubidi_props.cpp",
        "ubidi_props_data.h",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":utrie2",
    ],
)

cc_library(
    name = "ucase",
    srcs = [
        "ucase.cpp",
        "ucase_props_data.h",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":utrie2",
    ],
)

cc_library(
    name = "uchar",
    srcs = [
        "uchar.cpp",
        "uchar_props_data.h",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":utrie2",
    ],
)

cc_library(
    name = "emojiprops",
    srcs = [
        "emojiprops.cpp",
        "emojiprops.h",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":ucharstrie",
        ":ucharstrieiterator",
        ":ucptrie",
        ":udata",
    ],
)

cc_library(
    name = "ucharstrie",
    srcs = [
        "ucharstrie.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":platform",
    ],
)

cc_library(
    name = "ucharstriebuilder",
    srcs = [
        "ucharstriebuilder.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":sort",
        ":stringtriebuilder",
        ":ucharstrie",
    ],
)

cc_library(
    name = "ucharstrieiterator",
    srcs = [
        "ucharstrieiterator.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":ucharstrie",
        ":uvector32",
    ],
)

cc_library(
    name = "ucol_swp",
    srcs = [
        "ucol_swp.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":utrie_swap",
    ],
)

cc_library(
    name = "udata",
    srcs = [
        "restrace.cpp",
        "ucmndata.cpp",
        "udata.cpp",
        "udatamem.cpp",
        "umapfile.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":icu_utility",
        ":platform",
        ":uhash",
        "//third_party/icu4c/stubdata",
    ],
)

cc_library(
    name = "uiter",
    srcs = [
        "uiter.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":platform",
    ],
)

cc_library(
    name = "ulist",
    srcs = [
        "ulist.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":platform",
    ],
)

cc_library(
    name = "unames",
    srcs = [
        "unames.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":uchar",
        ":udata",
    ],
)

cc_library(
    name = "unifiedcache",
    srcs = [
        "unifiedcache.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":platform",
        ":uhash",
    ],
)

cc_library(
    name = "uniset_core",
    srcs = [
        "bmpset.cpp",
        "unifilt.cpp",
        "unifunct.cpp",
        "uniset.cpp",
        "unisetspan.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":icu_utility",
        ":patternprops",
        ":uvector",
    ],
)

cc_library(
    name = "uniset_closure",
    srcs = [
        "uniset_closure.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":uniset_core",
        ":unistr_case_locale",
        ":unistr_titlecase_brkiter",
    ],
)

cc_library(
    name = "uniset_props",
    srcs = [
        "ruleiter.cpp",
        "uniset_props.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":characterproperties",
        ":headers",
        ":parsepos",
        ":propname",
        ":resourcebundle",
        ":unames",
        ":uniset_core",
        ":unistr_case",
        ":uprops",
    ],
)

cc_library(
    name = "unistr_case",
    srcs = [
        "unistr_case.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":ustring_case",
    ],
)

cc_library(
    name = "unistr_case_locale",
    srcs = [
        "unistr_case_locale.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":unistr_case",
        ":ustring_case_locale",
    ],
)

cc_library(
    name = "unistr_titlecase_brkiter",
    srcs = [
        "unistr_titlecase_brkiter.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":ustr_titlecase_brkiter",
    ],
)

cc_library(
    name = "uprops",
    srcs = [
        "uprops.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":emojiprops",
        ":headers",
        ":loadednormalizer2",
        ":normalizer2",
        ":ubidi_props",
        ":ucase",
        ":uchar",
        ":unistr_case",
        ":ustring_case",
    ],
)

cc_library(
    name = "uscript_props",
    srcs = [
        "uscript_props.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":platform",
    ],
)

cc_library(
    name = "uset",
    srcs = [
        "uset.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":platform",
        ":uniset_core",
    ],
)

cc_library(
    name = "uset_props",
    srcs = [
        "uset_props.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":uniset_closure",
        ":uniset_core",
        ":uniset_props",
    ],
)

cc_library(
    name = "usetiter",
    srcs = [
        "usetiter.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":platform",
        ":uniset_core",
    ],
)

cc_library(
    name = "ustack",
    srcs = [
        "ustack.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":uvector",
    ],
)

cc_library(
    name = "ustr_titlecase_brkiter",
    srcs = [
        "ustr_titlecase_brkiter.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":breakiterator",
        ":headers",
        ":ucase",
        ":ustring_case_locale",
    ],
)

cc_library(
    name = "ustring_case",
    srcs = [
        "ustrcase.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":edits",
        ":headers",
        ":ucase",
        ":uchar",
    ],
)

cc_library(
    name = "ustring_case_locale",
    srcs = [
        "ustrcase_locale.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":resourcebundle",
        ":ustring_case",
    ],
)

cc_library(
    name = "utext",
    srcs = [
        "utext.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":ucase",
    ],
)

cc_library(
    name = "utrie_swap",
    srcs = [
        "utrie_swap.cpp",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
        ":udata",
    ],
)

# This target depends on a header file that contains NFC/NFD normalization data.
# This header file is generated by a script (generate.sh) that invokes the gennorm2 binary.
# See the Unicode update change log (changes.txt).
cc_library(
    name = "normalizer2",
    srcs = [
        "norm2_nfc_data.h",  # generated by gennorm2
        "normalizer2.cpp",
        "normalizer2impl.cpp",
    ],
    hdrs = [
        "normalizer2impl.h",
    ],
    copts = ["-std=c++17"],
    includes = ["."],
    local_defines = [
        "U_COMMON_IMPLEMENTATION",
    ],
    deps = [
        ":headers",
    ],
)
