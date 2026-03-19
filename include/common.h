#ifndef _COMMON_H
#define _COMMON_H

#include "include_asm.h"
#include "types.h"

#define PSX_SCRATCH ((void*)0x1F800000)

#define PSX_ALIGN (4)

#define ALIGN(x, a) \
    (((u32)(x) + ((a)-1)) & ~((a)-1))

#define SECTION(x) \
    __attribute__((section(x)))

#define SA_JOIN2(a, b) a##b
#define SA_JOIN(a, b) SA_JOIN2(a, b)
#define static_assert(cond) \
    __extension__ typedef char SA_JOIN(static_assert_line_,__LINE__)[(cond) ? 1 : -1]

#define offsetof(type, member) \
    ((unsigned long)&(((type*)0)->member))

#define member_type(type, member) \
    typeof(((type*)0)->member)

#define align(x) \
    (((x) + (PSX_ALIGN - 1)) & ~(PSX_ALIGN - 1))

#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))

#endif
