#ifndef GCC_TCONFIG_H
#define GCC_TCONFIG_H
#ifndef USED_FOR_TARGET
# define USED_FOR_TARGET
#endif

#define UNITS_PER_WORD	(4)
#define BITS_PER_UNIT (8)

#include "auto-host.h"
#ifdef IN_GCC
# include "ansidecl.h"
#endif
#endif /* GCC_TCONFIG_H */
