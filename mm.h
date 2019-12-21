#pragma once

#include <stdlib.h>

#define MM_MODULE_STATIC            1
#define MM_MODULE_DYNAMIC_ADDR      2
#define MM_MODULE_DYNAMIC_SIZE      3

#ifndef GLOBAL_CONFIG
#define USE_MM              MM_MODULE_DYNAMIC_ADDR
#define MM_HEAP_SIZE        (8*1024)
#define USE_MM_SHELL        0
#define USE_MM_STAT         0 
#endif

void mm_init(void);
void* mm_malloc(uint32_t s);
void mm_free(void* m);
#if USE_MM_SHELL
void mm_print_info(void);
#endif


