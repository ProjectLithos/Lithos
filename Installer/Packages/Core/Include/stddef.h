#ifndef OESDK_STDDEF_H
#define OESDK_STDDEF_H

#if defined(__INTELLISENSE__) || (defined(_MSC_VER) && !defined(__clang__))
typedef unsigned long long size_t;
typedef signed long long ptrdiff_t;
#else
typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
#endif
#define NULL ((void *)0)

#endif
