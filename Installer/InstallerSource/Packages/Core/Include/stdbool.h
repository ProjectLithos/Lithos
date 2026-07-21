#ifndef OESDK_STDBOOL_H
#define OESDK_STDBOOL_H

#if defined(__INTELLISENSE__) || (defined(_MSC_VER) && !defined(__clang__))
#define bool unsigned char
#else
#define bool _Bool
#endif
#define true 1
#define false 0

#endif
