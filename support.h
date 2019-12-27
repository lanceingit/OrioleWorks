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
    #include "board_config_sm702.h"
#elif defined(ESP)
    #include "osapi.h"
    #include "ets_sys.h"
    #include "board_config_esp8266.h"
#endif
