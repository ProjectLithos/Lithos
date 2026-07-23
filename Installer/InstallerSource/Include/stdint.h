#ifndef OESDK_STDINT_H
#define OESDK_STDINT_H

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
typedef int64_t intmax_t;
typedef uint64_t uintmax_t;
#if defined(__INTELLISENSE__) || (defined(_MSC_VER) && !defined(__clang__))
typedef signed long long intptr_t;
typedef unsigned long long uintptr_t;
#else
typedef __INTPTR_TYPE__ intptr_t;
typedef __UINTPTR_TYPE__ uintptr_t;
#endif

#define INT8_MIN   (-127 - 1)
#define INT16_MIN  (-32767 - 1)
#define INT32_MIN  (-2147483647 - 1)
#define INT64_MIN  (-9223372036854775807LL - 1)

#define INT8_MAX   127
#define INT16_MAX  32767
#define INT32_MAX  2147483647
#define INT64_MAX  9223372036854775807LL

#define UINT8_MAX  0xffU
#define UINT16_MAX 0xffffU
#define UINT32_MAX 0xffffffffU
#define UINT64_MAX 0xffffffffffffffffULL

#define INTPTR_MIN  ((intptr_t)(-INTPTR_MAX - 1))
#define INTPTR_MAX  ((intptr_t)((~(uintptr_t)0) >> 1))
#define UINTPTR_MAX ((uintptr_t)~(uintptr_t)0)
#define INTMAX_MIN  INT64_MIN
#define INTMAX_MAX  INT64_MAX
#define UINTMAX_MAX UINT64_MAX

#define INT8_C(Value)    Value
#define UINT8_C(Value)   Value
#define INT16_C(Value)   Value
#define UINT16_C(Value)  Value
#define INT32_C(Value)   Value
#define UINT32_C(Value)  Value##U
#define INT64_C(Value)   Value##LL
#define UINT64_C(Value)  Value##ULL
#define INTMAX_C(Value)  INT64_C(Value)
#define UINTMAX_C(Value) UINT64_C(Value)

#endif
