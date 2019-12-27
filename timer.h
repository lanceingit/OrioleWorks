#pragma once

#ifndef GLOBAL_CONFIG
#define TIMER_RATE_HZ       (1000)
#define TIMER_WIDTH_BIT     64
#define USE_RTC_INC         0
#endif

#define US_PER_TICK         (1.0f*1000*1000/TIMER_RATE_HZ)

#if TIMER_WIDTH_BIT==64
    typedef uint64_t Times;
    #define TIME_MAX 0xFFFFFFFFFFFFFFFF
#elif TIMER_WIDTH_BIT==32
    typedef uint32_t Times;
    #define TIME_MAX 0xFFFFFFFF
#else
    #error "timer width too low or not align"
#endif

#define TIMER_DEF(name) static Times name = 0;
#define TIMER_NEW(name, t) static Times name =0; name = timer_new(t);

void timer_init(void);
void timer_disable(void);

Times timer_new(uint32_t us);
bool timer_is_timeout(Times* t);

Times timer_now(void);
Times timer_elapsed(Times* t);

bool timer_check(Times* t, Times us);
float timer_get_dt(Times* t, float max, float min);

void delay(float s);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
#ifndef LINUX
    void sleep(float s);
#endif







