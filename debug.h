#pragma once

#include <stdio.h>

#ifndef GLOBAL_CONFIG
#define USE_DEBGU_SHELL         0    
#endif

#ifdef ORIOLE_WORKS
#include "cli.h"

#define DEBUG_WRITE(format,...)\
        do {\
            cli_device_write(""format"",##__VA_ARGS__ );\
        } while (0)

#elif defined(RTTHREAD)
#define DEBUG_WRITE(format,...)\
        do {\
            rt_kprintf(""format"",##__VA_ARGS__ );\
        } while (0)

#else
#define DEBUG_WRITE(format,...)
#endif


typedef enum {
    DEBUG_LEVEL_ERR = 0,
    DEBUG_LEVEL_WARN,
    DEBUG_LEVEL_INFO,
    DEBUG_LEVEL_DEBUG,
} DebugLevel;

typedef enum {
    DEBUG_ID_NULL = 0,
    DEBUG_ID_HMC5883,
    DEBUG_ID_I2C,
    DEBUG_ID_MPU6050,
    DEBUG_ID_MS5611,
    DEBUG_ID_SERIAL,
    DEBUG_ID_SPI,
    DEBUG_ID_SPI_FLASH,
    DEBUG_ID_TIMER,
    DEBUG_ID_MOTOR,
    DEBUG_ID_MAVLINK,
    DEBUG_ID_WWLINK,
    DEBUG_ID_EST,
    DEBUG_ID_MIXER,
    DEBUG_ID_CMD,
    DEBUG_ID_ATTC,
    DEBUG_ID_NAV,
    DEBUG_ID_SENS,
    DEBUG_ID_ALTC,
    DEBUG_ID_LOG,
    DEBUG_ID_MTD,
    DEBUG_ID_PARAM,
    DEBUG_ID_CLI,
    DEBUG_ID_SCHEDULER,
    DEBUG_ID_DEBUG,
    DEBUG_ID_FIFO,
    DEBUG_ID_PERF,
    DEBUG_ID_LPF,
    DEBUG_ID_PID,
    DEBUG_ID_MM,
    DEBUG_ID_LIST,
    DEBUG_ID_MATH,
    DEBUG_ID_MATRIX,
    DEBUG_ID_Q,
    DEBUG_ID_VECTOR,
    DEBUG_ID_DCM,
    DEBUG_ID_ATT_CF,
    DEBUG_ID_ATT_Q,
    DEBUG_ID_STAB,
    DEBUG_ID_ALTHOLD,
    DEBUG_ID_LAND,
    DEBUG_ID_MAX,
} DebugID;

extern DebugLevel debug_level;
extern DebugID debug_module;
extern char* debug_module_list[DEBUG_ID_MAX];

#ifdef LINUX
#define PRINT(format,...)\
        do {\
            printf(""format"",##__VA_ARGS__ );\
            DEBUG_WRITE(""format"",##__VA_ARGS__ );\
        } while (0)
#else
#define PRINT(format,...)\
        do {\
            DEBUG_WRITE(""format"",##__VA_ARGS__ );\
        } while (0)
#endif

#define PRINT_BUF(str, buf, len)\
    do {\
        PRINT("%s",str);\
        for(uint8_t i=0; i<len; i++) { \
            PRINT("%02x ", buf[i]);  \
        } \
        PRINT("\n"); \
    } while(0)

#define DEBUG(module,format,...)\
	do {\
		if(debug_module == DEBUG_ID_##module && debug_level >= DEBUG_LEVEL_DEBUG){\
			PRINT("[DEBUG][%s]"format, debug_module_list[DEBUG_ID_##module], ##__VA_ARGS__ );\
		}\
	} while (0)

#define DEBUG_BUF(module, str, buf, len)\
    do {\
        if(debug_module == DEBUG_ID_##module && debug_level >= DEBUG_LEVEL_DEBUG){\
            PRINT("[DEBUG][%s]%s",debug_module_list[DEBUG_ID_##module], str);\
            for(uint8_t i=0; i<len; i++) { \
                PRINT("%02x ", buf[i]);  \
            } \
            PRINT("\n"); \
        } \
    } while(0)

#define DEBUG_BUF_DEC(module, str, buf, len)\
    do {\
        if(debug_module == DEBUG_ID_##module && debug_level >= DEBUG_LEVEL_DEBUG){\
            PRINT("[DEBUG][%s]%s",debug_module_list[DEBUG_ID_##module], str);\
            for(uint8_t i=0; i<len; i++) { \
                PRINT("%d ", buf[i]);  \
            } \
            PRINT("\n"); \
        } \
    } while(0)

#define INFO(module,format,...)\
	do {\
		if(debug_level >= DEBUG_LEVEL_INFO){ \
            PRINT("[INFO][%s]"format, debug_module_list[DEBUG_ID_##module], ##__VA_ARGS__);\
		} \
	} while (0)

#define WARN(module,format,...)\
	do {\
		if(debug_level >= DEBUG_LEVEL_WARN){ \
            PRINT("[WARN][%s]"format, debug_module_list[DEBUG_ID_##module], ##__VA_ARGS__);\
		} \
	} while (0)

#define ERR(module,format,...)\
	do {\
		PRINT("[ERR][%s]"format,  debug_module_list[DEBUG_ID_##module], ##__VA_ARGS__);\
	} while (0)
    

void debug_init(void);

///////////////////////////////////////////////////////

#include <stdarg.h>

int esprintf(char* buf, const char* fmt, ...);
int evsprintf(char* buf, const char* fmt, va_list ap);

///////////////////////////////////////////////////////
typedef struct {               
    uint16_t id;
    char* name;
} EnumString;                            
 
#define ENUM_STRING_START(type) static EnumString ENUM_##type[]={ 
#define ENUM_STRING_ADD(id) {id, #id},  
#define ENUM_STRING_END  {0xFFFF,"end"} };

#define ENUM_TO_STRING(type, id) enum2string(ENUM_##type, id)  
char* enum2string(EnumString* enum_string, uint8_t id);

