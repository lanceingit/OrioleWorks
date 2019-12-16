#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef F3_EVO
    #include "stm32f30x.h"
    #define STM32F3
#elif LINUX
#elif SM702
    #include "am_mcu_apollo.h"
    #define APOLLO
#endif



#define GLOBAL_CONFIG

#ifdef F3_EVO
    #define TASK_MAX            20
    #define TIMER_RATE_HZ       100
    #define TIMER_WIDTH_BIT     64
    #define MM_HEAP_SIZE        (4*1024)

#elif LINUX
    #define TASK_MAX            20

    #define TIMER_RATE_HZ       100  //in linux not need
    #define TIMER_WIDTH_BIT     64

#elif ESP
    #define TASK_MAX            20

    #define TIMER_RATE_HZ       100
    #define TIMER_WIDTH_BIT     64

#elif SM702
    #define TASK_MAX            20

    #define RTTHREAD
    #define TIMER_RATE_HZ       1000
    #define TIMER_WIDTH_BIT     64

    #define USE_MM              MM_MODULE_DYNAMIC_ADDR
    #define USE_MM_PRINT        1
    #define MM_HEAP_SIZE        (8*1024)

    #define SHELL_NUM               50
    #define CMD_LEN_MAX             50
    #define CLI_READ_BUF_SIZE       100
    #define CLI_WRITE_BUF_SIZE      500
    #define CLI_ARGV_MAX            10

    #define USE_DATA_WIN    0
    #define USE_UNIT_FIFO   0
    #define USE_FLOAT_FIFO  0
    #define USE_FIFO        1
    #if USE_FIFO
        #define USE_FIFO_MODIFY 0
        #define USE_FIFO_PRINT  0
    #endif

    #define USE_EVENT           0
    #define USE_EVENT_SHELL     0
    #define USE_STAT            0
    #define USE_STAT_SHELL      0
    #define USE_RATE_PERF       0
    #define USE_CNT_PERF        0
    #define USE_TIME_PERF       0

    #define EVENT_ADDR          0xFF000
    #define EVNET_BUF_SIZE      1024 

    #define STAT_ADDR           0xFF400
    #define STAT_BUF_SIZE       1024 
#endif



