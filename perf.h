#pragma once

#ifndef GLOBAL_CONFIG
#define USE_EVENT           1
#define USE_EVENT_SHELL     0
#define USE_STAT            1
#define USE_STAT_SHELL      0
#define USE_RATE_PERF       1
#define USE_CNT_PERF        1
#define USE_TIME_PERF       1

#define EVENT_ADDR          0xFF000
#define EVNET_BUF_SIZE      1024 

#define STAT_ADDR           0xFF400
#define STAT_BUF_SIZE       1024 
#endif


typedef enum {
	EVENT_POWER_ON = 0,
	EVENT_POWER_OFF,    
	EVENT_POWER_RESET,    

	EVENT_KEY_PRESS,
	EVENT_KEY_LONG_PRESS,
	EVENT_KEY_RELEASE,
} event_e;


typedef struct {
    uint32_t timestamp;
    uint8_t event;
    uint8_t reason;
} Event;

void event_init(void);
void event_record(uint8_t event, uint8_t reason);
void event_save(void);
Event event_get(int16_t index);
uint16_t event_export(void);

//////////////////////////////////////

typedef struct {
    uint32_t magic;
    uint32_t start_timestamp;
    uint32_t stop_timestamp;

	uint32_t system_reset;
} Stat;

extern Stat _diagnosis_stat;

#define STAT_CNT(s, cnt) _diagnosis_stat.s+=cnt;

void stat_init(void);
void stat_reset(void);
void stat_save(void);
void stat_stop(void);
void stat_update_time(void);

//////////////////////////////////////

#include "timer.h"

typedef struct  
{
    times_t timer;
    float rate;
    uint32_t cnt;
} RatePerf;

#define RATE_PERF_DEF(p)  static RatePerf p = {0,0,0};

#define RATE_PERF_TAG(tag) \
    RATE_PERF_DEF(_##tag##_rate) 

#define RATE_PERF_SELF(tag) (_##tag##_rate) 

#define RATE_PERF_RUN(tag, cnt, rate) \
    RATE_PERF_DEF(_##tag##_rate) \
    RatePerf_cnt(&_##tag##_rate, cnt); \
    RatePerf_print(&_##tag##_rate, #tag, rate); 

#define RATE_PERF_RUN_CHECK(tag, cnt, rate, threshold) \
    RATE_PERF_DEF(_##tag##_rate) \
    RatePerf_cnt(&_##tag##_rate, cnt); \
    RatePerf_print_check(&_##tag##_rate, #tag, rate, threshold); 

#define RATE_PERF_CNT(tag, cnt) \
    RatePerf_cnt(&_##tag##_rate, cnt); 

#define RATE_PERF_PRINT(tag, rate) \
    RatePerf_print(&_##tag##_rate, #tag, rate);


void RatePerf_cnt(RatePerf* self, uint8_t cnt);
void RatePerf_print(RatePerf* self, char* tag, uint8_t rate_s);
void RatePerf_print_check(RatePerf* self, char* tag, uint8_t rate_s, float threshold);

//////////////////////////////////////

typedef struct 
{
    uint32_t cnt;
} CntPerf;

#define  CNT_PERF_DEF(p)  static CntPerf p = {0};

#define CNT_PERF_TAG(tag) \
    CNT_PERF_DEF(_##tag##_cnt) 

#define CNT_PERF_CNT(tag, cnt) \
    CntPerf_cnt(&_##tag##_cnt, cnt);     

#define CNT_PERF_PRINT(tag) \
    CntPerf_print(&_##tag##_cnt, #tag);      

void CntPerf_cnt(CntPerf* self, uint8_t cnt);
void CntPerf_print(CntPerf* self, char* tag);

//////////////////////////////////////

typedef struct {
    times_t t0;
    times_t t1;
    uint64_t cnt;
    times_t avg;
    times_t sum;
    times_t t_max;
    times_t t_min;
} TimePerf;

#define TIME_PERF_DEF(p) static TimePerf p = { \
    .t0 = 0, \
    .t1 = 0, \
    .cnt = 0, \
    .avg = 0, \
    .sum = 0, \
    .t_max = 0, \
    .t_min = 0xFFFFFFFF, \
};

#define TIME_PERF_TAG(tag) \
    TIME_PERF_DEF(_##tag##_timer) 

#define TIME_PERF_INTERVAL(tag) \
    TimePerf_interval(&_##tag##_timer);     

#define TIME_PERF_BEGIN(tag) \
    TimePerf_begin(&_##tag##_timer);     

#define TIME_PERF_END(tag) \
    TimePerf_end(&_##tag##_timer);     

#define TIME_PERF_PRINT(tag) \
    TimePerf_print(&_##tag##_timer, #tag);    


void TimePerf_init(TimePerf* self);
void TimePerf_interval(TimePerf* self);
void TimePerf_begin(TimePerf* self);
void TimePerf_end(TimePerf* self);
void TimePerf_print(TimePerf* self, char* tag);

