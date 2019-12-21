#pragma once

#include <stdint.h>
#include <stddef.h>

#define SM702

#ifdef F3_EVO
    #include <stdbool.h>
    #include "stm32f30x.h"
    #define STM32F3
#elif defined(LINUX)
    #include <stdbool.h>
#elif defined(SM702)
    #include <stdbool.h>
    #include "am_mcu_apollo.h"
    #define APOLLO
#elif defined(ESP)
    #include "osapi.h"
    #include "ets_sys.h"
#endif


#ifdef F3_EVO
    #define TASK_MAX            20
    #define TIMER_RATE_HZ       100
    #define TIMER_WIDTH_BIT     64
    #define MM_HEAP_SIZE        (4*1024)

#elif defined(LINUX)
    #define TASK_MAX            20

    #define TIMER_RATE_HZ       100  //in linux not need
    #define TIMER_WIDTH_BIT     64

#elif defined(ESP)
    #define ORIOLE_WORKS
    #define GLOBAL_CONFIG

    #define TASK_MAX                20
    #define TASK_NAME_MAX           10
    #define USE_TASK_SHELL          1

    #define TIMER_RATE_HZ           100
    #define TIMER_WIDTH_BIT         64
    #define USE_RTC                 0

    #define USE_MM                  MM_MODULE_DYNAMIC_ADDR
    #define MM_HEAP_SIZE            (8*1024)
    #define USE_MM_SHELL            1
    #define USE_MM_STAT             1

    #define USE_DEBGU_SHELL         1 

    #define SHELL_NUM               50
    #define CMD_LEN_MAX             50
    #define CLI_READ_BUF_SIZE       100
    #define CLI_WRITE_BUF_SIZE      500
    #define CLI_ARGV_MAX            10

    #define USE_DATA_WIN            0
    #define USE_UNIT_FIFO           0
    #define USE_FLOAT_FIFO          0
    #define USE_FIFO                0
    #if USE_FIFO
        #define USE_FIFO_MODIFY     0
        #define USE_FIFO_PRINT      0
    #else    
        #define USE_FIFO_MODIFY     0
        #define USE_FIFO_PRINT      0
    #endif

    #define USE_EVENT               0
    #define USE_EVENT_SHELL         0
    #define USE_STAT                0
    #define USE_STAT_SHELL          0
    #define USE_RATE_PERF           0
    #define USE_CNT_PERF            0
    #define USE_TIME_PERF           0
    
    #define EVENT_ADDR              0xFF000
    #define EVNET_BUF_SIZE          1024 
    
    #define STAT_ADDR               0xFF400
    #define STAT_BUF_SIZE           1024  

    #include "board.h"    

#elif defined(SM702)
    #define ORIOLE_WORKS
    #define GLOBAL_CONFIG
    
    #define USE_RTT                 1

    #define TASK_MAX                20
    #define TASK_NAME_MAX           10
    #define USE_TASK_SHELL          1

    #define TIMER_RATE_HZ           1000
    #define TIMER_WIDTH_BIT         64

    #define USE_MM                  MM_MODULE_DYNAMIC_ADDR
    #define MM_HEAP_SIZE            (8*1024)
    #define USE_MM_SHELL            1
    #define USE_MM_STAT             1

    #define USE_DEBGU_SHELL         1 

    #define SHELL_NUM               50
    #define CMD_LEN_MAX             50
    #define CLI_READ_BUF_SIZE       100
    #define CLI_WRITE_BUF_SIZE      500
    #define CLI_ARGV_MAX            10

    #define USE_DATA_WIN            0
    #define USE_UNIT_FIFO           0
    #define USE_FLOAT_FIFO          0
    #define USE_FIFO                0
    #if USE_FIFO
        #define USE_FIFO_MODIFY     0
        #define USE_FIFO_PRINT      0
    #endif

    #define USE_EVENT               0
    #define USE_EVENT_SHELL         0
    #define USE_STAT                0
    #define USE_STAT_SHELL          0
    #define USE_RATE_PERF           0
    #define USE_CNT_PERF            0
    #define USE_TIME_PERF           0
    
    #define EVENT_ADDR              0xFF000
    #define EVNET_BUF_SIZE          1024 
    
    #define STAT_ADDR               0xFF400
    #define STAT_BUF_SIZE           1024 
    
    #define KEY_GPIO                19
    #define KEY_PRESS_LEVEL         0
    #define KEY_ACTION_TIME_MS      50
    #define KEY_LONG_PRESS_MS       1500
    
    #define RED_LED_GPIO            44
    #define BLUE_LED_GPIO           13
    #define LED_LIGHT_LEVEL         0
#endif



