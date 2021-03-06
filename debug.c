/**
 *  .----. .----. .-. .----. .-.   .----.       .-.       
 * /  {}  \| {}  }| |/  {}  \| |   | {_        /'v'\      
 * \      /| .-. \| |\      /| `--.| {__      (/   \)     
 *  `----' `-' `-'`-' `----' `----'`----'    ='="="===<   
 *                                              |_|
 * debug.c
 *
 * v1.4
 *
 * Debug module, support module select
 */
#include "support.h"

#include "debug.h"
#include "debug_module_list.h"

#include <ctype.h>
#if USE_PRINT_RTC
#include <time.h>
#include "rtc.h"
#include "timer.h"
#endif

DebugLevel debug_level = DEBUG_LEVEL_INFO;
DebugID debug_module = DEBUG_ID_NULL;

/////////////////////////////////////////////////////////////////

/*
If use print function, must define powf function
like. 
#include <math.h>
#define POW_FUNC             powf
*/

#define POW_FUNC             powerf


#define DEF_PRECISION   6

static const char digit[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                             'A', 'B', 'C', 'D', 'E', 'F'
                            };

static char* print_buf;
static uint16_t print_cnt=0;

#ifndef powerf
float powerf(float base, int exp) 
{
    float result = base;
    for (int count = 1; count < exp; count++) result *= base;

    return result;
}  
#endif

int putcf(int c)
{
    print_buf[print_cnt++] = c;

    return print_cnt;
}

static uint8_t get_int_len(long long int value)
{
    uint8_t len = 1;

    while(value > 9) {
        len++;
        value /= 10;
    }

    return len;
}

static uint8_t itoa_dec_unsigned(unsigned long int num)
{
    uint8_t len = 0;

    if(num == 0) {
        putcf('0');
        return 1;
    }

    unsigned long int i = 1;

    while((num / i) > 9) {
        i *= 10;
    }

    do {
        putcf(digit[(num / i) % 10]);
        len++;
    }
    while(i /= 10);

    return len;
}

static uint8_t itoa_dec(long int num, uint8_t width, char pad)
{
    uint8_t len = 0;

//    if(num == 0) {
//        putcf('0');
//        return 1;
//    }

    unsigned long int n = num;
    if(num < 0) {
        n = -num;
        putcf('-');
        len++;
    }

    uint8_t num_len = get_int_len(num);
    if(num_len < width) {
        uint8_t fill_num = width - num_len;
        while(fill_num > 0) {
            putcf(pad);
            len++;
            fill_num--;
        }
    }

    return itoa_dec_unsigned(n)+len;
}

static uint8_t itoa_hex(unsigned long int num, uint8_t width, char pad)
{
    uint8_t len = 0;
    bool found_first = false;
    
    if(num == 0) {
        return itoa_dec(num, width, pad);
    }

    for(int8_t i = (32/4-1); i >= 0; i--) {
        uint8_t shift = i * 4;
        uint32_t mask = (uint32_t)0x0F << shift;
        uint32_t val = (num & mask) >> shift;

        if(val > 0) {
            found_first = true;
        }

        if(found_first || i < width) {
            if(found_first) {
                putcf(digit[val]);
            }
            else {
                putcf(pad);
            }

            len++;
        }
    }

    return len;
}

static uint8_t itoa_dec_ull(unsigned long long int num)
{
    uint8_t len = 0;

    if(num == 0) {
        putcf('0');
        return 1;
    }

    unsigned long int i = 1;

    while((num / i) > 9) {
        i *= 10;
    }

    do {
        putcf(digit[(num / i) % 10]);
        len++;
    }
    while(i /= 10);

    return len;
}

static uint8_t itoa_dec_ll(long long int num, uint8_t width, char pad)
{
    uint8_t len = 0;

//    if(num == 0) {
//        putcf('0');
//        return 1;
//    }

    unsigned long long int n = num;
    if(num < 0) {
        n = -num;
        putcf('-');
        len++;
    }

    uint8_t num_len = get_int_len(num);
    if(num_len < width) {
        uint8_t fill_num = width - num_len;
        while(fill_num > 0) {
            putcf(pad);
            len++;
            fill_num--;
        }
    }

    return itoa_dec_unsigned(n)+len;
}

static uint8_t itoa_hex_ll(unsigned long long int num, uint8_t width, char pad)
{
    uint8_t len = 0;
    bool found_first = false;

    for(int8_t i = (64/4-1); i >= 0; i--) {
        uint8_t shift = i * 4;
        uint64_t mask = (uint64_t)0x0F << shift;
        uint64_t val = (num & mask) >> shift;

        if(val > 0) {
            found_first = true;
        }

        if(found_first || i < width) {
            if(found_first) {
                putcf(digit[val]);
            }
            else {
                putcf(pad);
            }

            len++;
        }
    }

    return len;
}

int evsprintf(char* buf, const char* fmt, va_list ap)
{
    int len=0;
    float num;
    char* str;
    uint8_t precision;
    uint8_t width;
    char pad;

    print_buf = buf;
    print_cnt = 0;

    while(*fmt) {
        if(*fmt == '%') {
            precision = DEF_PRECISION;
            pad = ' ';
            width = 0;

            fmt++;
            while('0' == *fmt) {
                pad = '0';
                fmt++;
            }

            while(isdigit((unsigned)*fmt)) {
                width *= 10;
                width += *fmt - '0';
                fmt++;
            }

            while(!isalpha((unsigned) *fmt)) {
                if(*fmt == '.') {
                    fmt++;
                    if(isdigit((unsigned)*fmt)) {
                        precision = *fmt - '0';
                        fmt++;
                    }
                }
            }

            switch(*fmt++) {
            case 'c':
                putcf(va_arg(ap, int));
                len++;
                break;
            case 'd':
                len += itoa_dec(va_arg(ap, int), width, pad);
                break;
            case 'u':
                len += itoa_dec_unsigned(va_arg(ap, unsigned int));
                break;
            case 'x':
            case 'X':
                len += itoa_hex(va_arg(ap, unsigned int), width, pad);
                break;
            case 'l':
                //len += handle_long(fmt, ap, width, pad);
                switch(*fmt++) {
                case 'd':
                    len += itoa_dec(va_arg(ap, long int), width, pad);
                    break;
                case 'u':
                    len += itoa_dec_unsigned(va_arg(ap, unsigned long int));
                    break;
                case 'x':
                case 'X':
                    len += itoa_hex(va_arg(ap, unsigned long int), width, pad);
                    break;
                case 'l':
                    //len += handle_longlong(fmt, ap, width, pad);
                    switch(*fmt++) {
                    case 'd':
                        len += itoa_dec_ll(va_arg(ap, long long int), width, pad);
                        break;
                    case 'u':
                        len += itoa_dec_ull(va_arg(ap, long long unsigned int));
                        break;
                    case 'x':
                    case 'X':
                        len += itoa_hex_ll(va_arg(ap, long long unsigned int), width, pad);
                        break;
                    default:
                        break;
                    }
                    break;
                default:
                    break;
                }
                break;
            case 'f':
                num = va_arg(ap, double);
                if(num<0) {
                    putcf('-');
                    num = -num;
                    len++;
                }
                len += itoa_dec((int)num, width, pad);
                putcf('.');
                len++;
                len += itoa_dec((num - (int)num) * POW_FUNC(10.0f, precision), precision, '0');
                break;
            case 's':
                str = va_arg(ap, char*);
                if(str == NULL) {
                    str = "(NULL)";
                }                
                while(*str) {
                    putcf(*str++);
                    len++;
                }
                break;
            default:
                break;
            }
        }
        else {
            putcf(*fmt++);
            len++;
        }
    }

    return len;
}

int esprintf(char* buf, const char* fmt, ...)
{
    va_list ap;
    int len;

    va_start(ap, fmt);
    len = evsprintf(buf, fmt, ap);
    va_end(ap);

    return len;
}

/////////////////////////////////////////////////////////////////
char* enum2string(EnumString* enum_string, int16_t id)
{
   uint8_t i=0;
   while(enum_string[i].id!=0x7FFF) {
      if(enum_string[i].id==id)
          return enum_string[i].name;
      i++;
   }
   
   return NULL;
}

/////////////////////////////////////////////////////////////////
void print_time(void)
{
#if USE_PRINT_RTC    
#if defined(ORIOLE_WORKS)
    time_t now;
    struct tm t;
#if USE_RTC    
    rtc_get_UTC(&now); 
#else
    now = timer_now();
#endif
    localtime_r(&now, &t);
    PRINT("%02d-%02d-%02d.%03d    ", t.tm_hour, t.tm_min, t.tm_sec, (timer_now()/1000)%1000);
#endif    
#endif    
}

void debug_list_modules(void)
{
    uint16_t count;

    cli_device_write("modules:\r\n");
    for(count = 0 ; count < DEBUG_ID_MAX ; count++) {
        cli_device_write("[%d][%s]\r\n", count, debug_module_list[count]);
    }
}

void debug_status_show(void)
{
    char* level_str=NULL;
    switch(debug_level) {
    case DEBUG_LEVEL_ERR:
        level_str = "err";
        break;
    case DEBUG_LEVEL_WARN:
        level_str = "warn";
        break;
    case DEBUG_LEVEL_INFO:
        level_str = "info";
        break;
    case DEBUG_LEVEL_DEBUG:
        level_str = "debug";
        break;
    default:
        break;
    }
    cli_device_write("debug level:%s(%d) module:%s(%d)\r\n", level_str, debug_level,
                     debug_module_list[debug_module], debug_module);
}

void debug_level_set(DebugLevel level)
{
    debug_level = level;
}

bool debug_module_set(char* module)
{
    uint8_t count;

    debug_level_set(DEBUG_LEVEL_DEBUG);
    for(count = 0 ; count < DEBUG_ID_MAX ; count++) {
        if(strcasecmp(debug_module_list[count], module) == 0) {
            debug_module = (DebugID)count;
            cli_device_write("debug module:[%d][%s]\r\n", debug_module, debug_module_list[debug_module]);
            return true;
        }
    }
    return false;
}

void debug_shell(int argc, char* argv[]);

void debug_init(void)
{
#if USE_DEBGU_SHELL    
    cli_regist("debug", debug_shell);
#endif    
}

#if USE_DEBGU_SHELL
void debug_shell(int argc, char* argv[])
{
    if(argc == 2) {
        if(strcmp(argv[1], "list") == 0) {
            debug_list_modules();
            return;
        }
        else if(strcmp(argv[1], "status") == 0) {
            debug_status_show();
            return;
        }
        else if(strcmp(argv[1], "all") == 0) {
            debug_level_set(DEBUG_LEVEL_DEBUG);
            return;
        }
        else if(strcmp(argv[1], "off") == 0) {
            debug_module = DEBUG_ID_NULL;
            debug_level_set(DEBUG_LEVEL_INFO);
            return;
        }
        else {
            if(debug_module_set(argv[1])) {
                return;
            }
        }
    }
    else if(argc == 3) {
        if(strcmp(argv[1], "level") == 0) {
            debug_level_set((DebugLevel)atoi(argv[2]));
            return;
        }
    }

    cli_device_write("missing command: try 'list', 'status', 'all', 'off', 'level n', 'module name'");
}
#endif
