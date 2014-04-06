#include <stdint.h>
#include <stdlib.h>

#ifndef __AEON_COMMON_H_
#define __AEON_COMMON_H_

typedef int64_t aeon_time_t;
typedef uint64_t aeon_pos_t;
typedef uint64_t aeon_size_t;
typedef uint32_t aeon_count_t;

#define AEON_CHECK_ALLOC(ptr) if (ptr == NULL) { return -1; }


#endif
