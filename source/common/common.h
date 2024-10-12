#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

#define UNUSED(X) (void)(X)

#define NOOP                         \
    int CONCAT(NOOP_, __LINE__) = 0; \
    UNUSED(CONCAT(NOOP_, __LINE__));

#define MASK(BITS, SHIFT) (~(UINT64_MAX << BITS) << SHIFT)
#define MAX_VALUE(BITS) (~(UINT64_MAX << BITS))

#define COMPILE_TIME_ASSERT(predicate) \
    static char CONCAT(compile_time_assertion_, __LINE__)[!!(predicate)-1];

#define __NARG__(...) __NARG_I_(__VA_ARGS__, __RSEQ_N())
#define __NARG_I_(...) __ARG_N(__VA_ARGS__)
#define __ARG_N(                                      \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,          \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
    _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
    _61, _62, _63, N, ...                             \
) N
#define __RSEQ_N()                              \
    63, 62, 61, 60,                             \
        59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
        49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
        39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
        29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
        19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
        9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define OVERLOAD_EXPAND(macroName, number_of_args) CONCAT_INNER(macroName, number_of_args)
#define OVERLOADED_MACRO(M, ...) OVERLOAD_EXPAND(M, __NARG__(__VA_ARGS__))(__VA_ARGS__)

#define CONCAT_INNER(a, b) a##b
#define CONCAT_2(a, b) CONCAT_INNER(a, b)
#define CONCAT_3(a, ...) CONCAT_2(a, CONCAT_2(__VA_ARGS__))
#define CONCAT_4(a, ...) CONCAT_2(a, CONCAT_3(__VA_ARGS__))
#define CONCAT_5(a, ...) CONCAT_2(a, CONCAT_4(__VA_ARGS__))
#define CONCAT_6(a, ...) CONCAT_2(a, CONCAT_5(__VA_ARGS__))
#define CONCAT_7(a, ...) CONCAT_2(a, CONCAT_6(__VA_ARGS__))
#define CONCAT_8(a, ...) CONCAT_2(a, CONCAT_7(__VA_ARGS__))
#define CONCAT_9(a, ...) CONCAT_2(a, CONCAT_8(__VA_ARGS__))
#define CONCAT_10(a, ...) CONCAT_2(a, CONCAT_9(__VA_ARGS__))
#define CONCAT_11(a, ...) CONCAT_2(a, CONCAT_10(__VA_ARGS__))
#define CONCAT_12(a, ...) CONCAT_2(a, CONCAT_11(__VA_ARGS__))
#define CONCAT_13(a, ...) CONCAT_2(a, CONCAT_12(__VA_ARGS__))
#define CONCAT_14(a, ...) CONCAT_2(a, CONCAT_13(__VA_ARGS__))
#define CONCAT_15(a, ...) CONCAT_2(a, CONCAT_14(__VA_ARGS__))
#define CONCAT_16(a, ...) CONCAT_2(a, CONCAT_15(__VA_ARGS__))
#define CONCAT_17(a, ...) CONCAT_2(a, CONCAT_16(__VA_ARGS__))
#define CONCAT_18(a, ...) CONCAT_2(a, CONCAT_17(__VA_ARGS__))
#define CONCAT_19(a, ...) CONCAT_2(a, CONCAT_18(__VA_ARGS__))
#define CONCAT_20(a, ...) CONCAT_2(a, CONCAT_19(__VA_ARGS__))
#define CONCAT_21(a, ...) CONCAT_2(a, CONCAT_20(__VA_ARGS__))
#define CONCAT_22(a, ...) CONCAT_2(a, CONCAT_21(__VA_ARGS__))
#define CONCAT_23(a, ...) CONCAT_2(a, CONCAT_22(__VA_ARGS__))
#define CONCAT_24(a, ...) CONCAT_2(a, CONCAT_23(__VA_ARGS__))
#define CONCAT_25(a, ...) CONCAT_2(a, CONCAT_24(__VA_ARGS__))
#define CONCAT_26(a, ...) CONCAT_2(a, CONCAT_25(__VA_ARGS__))
#define CONCAT_27(a, ...) CONCAT_2(a, CONCAT_26(__VA_ARGS__))
#define CONCAT_28(a, ...) CONCAT_2(a, CONCAT_27(__VA_ARGS__))
#define CONCAT_29(a, ...) CONCAT_2(a, CONCAT_28(__VA_ARGS__))
#define CONCAT_30(a, ...) CONCAT_2(a, CONCAT_29(__VA_ARGS__))
#define CONCAT_31(a, ...) CONCAT_2(a, CONCAT_30(__VA_ARGS__))
#define CONCAT_32(a, ...) CONCAT_2(a, CONCAT_31(__VA_ARGS__))
#define CONCAT_33(a, ...) CONCAT_2(a, CONCAT_32(__VA_ARGS__))
#define CONCAT_34(a, ...) CONCAT_2(a, CONCAT_33(__VA_ARGS__))
#define CONCAT_35(a, ...) CONCAT_2(a, CONCAT_34(__VA_ARGS__))
#define CONCAT_36(a, ...) CONCAT_2(a, CONCAT_35(__VA_ARGS__))
#define CONCAT_37(a, ...) CONCAT_2(a, CONCAT_36(__VA_ARGS__))
#define CONCAT_38(a, ...) CONCAT_2(a, CONCAT_37(__VA_ARGS__))
#define CONCAT_39(a, ...) CONCAT_2(a, CONCAT_38(__VA_ARGS__))
#define CONCAT_40(a, ...) CONCAT_2(a, CONCAT_39(__VA_ARGS__))
#define CONCAT_41(a, ...) CONCAT_2(a, CONCAT_40(__VA_ARGS__))
#define CONCAT_42(a, ...) CONCAT_2(a, CONCAT_41(__VA_ARGS__))
#define CONCAT_43(a, ...) CONCAT_2(a, CONCAT_42(__VA_ARGS__))
#define CONCAT_44(a, ...) CONCAT_2(a, CONCAT_43(__VA_ARGS__))
#define CONCAT_45(a, ...) CONCAT_2(a, CONCAT_44(__VA_ARGS__))
#define CONCAT_46(a, ...) CONCAT_2(a, CONCAT_45(__VA_ARGS__))
#define CONCAT_47(a, ...) CONCAT_2(a, CONCAT_46(__VA_ARGS__))
#define CONCAT_48(a, ...) CONCAT_2(a, CONCAT_47(__VA_ARGS__))
#define CONCAT_49(a, ...) CONCAT_2(a, CONCAT_48(__VA_ARGS__))
#define CONCAT_50(a, ...) CONCAT_2(a, CONCAT_49(__VA_ARGS__))
#define CONCAT_51(a, ...) CONCAT_2(a, CONCAT_50(__VA_ARGS__))
#define CONCAT_52(a, ...) CONCAT_2(a, CONCAT_51(__VA_ARGS__))
#define CONCAT_53(a, ...) CONCAT_2(a, CONCAT_52(__VA_ARGS__))
#define CONCAT_54(a, ...) CONCAT_2(a, CONCAT_53(__VA_ARGS__))
#define CONCAT_55(a, ...) CONCAT_2(a, CONCAT_54(__VA_ARGS__))
#define CONCAT_56(a, ...) CONCAT_2(a, CONCAT_55(__VA_ARGS__))
#define CONCAT_57(a, ...) CONCAT_2(a, CONCAT_56(__VA_ARGS__))
#define CONCAT_58(a, ...) CONCAT_2(a, CONCAT_57(__VA_ARGS__))
#define CONCAT_59(a, ...) CONCAT_2(a, CONCAT_58(__VA_ARGS__))
#define CONCAT_60(a, ...) CONCAT_2(a, CONCAT_59(__VA_ARGS__))
#define CONCAT_61(a, ...) CONCAT_2(a, CONCAT_60(__VA_ARGS__))
#define CONCAT_62(a, ...) CONCAT_2(a, CONCAT_61(__VA_ARGS__))
#define CONCAT_63(a, ...) CONCAT_2(a, CONCAT_62(__VA_ARGS__))
#define CONCAT(...) OVERLOADED_MACRO(CONCAT_, __VA_ARGS__)

#endif