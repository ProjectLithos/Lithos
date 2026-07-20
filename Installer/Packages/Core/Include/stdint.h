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
#if defined(__INTELLISENSE__) || (defined(_MSC_VER) && !defined(__clang__))
typedef signed long long intptr_t;
typedef unsigned long long uintptr_t;
#else
typedef __INTPTR_TYPE__ intptr_t;
typedef __UINTPTR_TYPE__ uintptr_t;
#endif

#define UINT8_MAX  0xffU
#define UINT16_MAX 0xffffU
#define UINT32_MAX 0xffffffffU
#define UINT64_MAX 0xffffffffffffffffULL

#endif
