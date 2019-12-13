#include "support.h"
#include "perf.h"
#include "data_wrapper.h"
#include "debug.h"
#if USE_EVENT_SHELL || USE_STAT_SHELL
#include "cli.h"
#endif

/*
If use print function, must define printf function
like. 
#include "debug.h"
#define PRINTF_FUNC             PRINT
*/
#include "debug.h"
#include "time.h"

#define PRINTF_FUNC             PRINT
#define TIMESTAMP_FUNC          time
#define LOCALTIME_FUNC          localtime


#define EVENT_MAGIC     0xE38B1037
#define STAT_MAGIC      0xA209F87C


ENUM_STRING_START(event)
ENUM_STRING_ADD(EVENT_POWER_ON)
ENUM_STRING_ADD(EVENT_POWER_OFF)   
ENUM_STRING_ADD(EVENT_POWER_RESET)  
ENUM_STRING_ADD(EVENT_KEY_PRESS)
ENUM_STRING_ADD(EVENT_KEY_LONG_PRESS)
ENUM_STRING_ADD(EVENT_KEY_RELEASE)
ENUM_STRING_END


/*
buf:
    | magic     |    
    | fifo info |
    | data      | 
*/
uint8_t event_buf[EVNET_BUF_SIZE];
UnitFifo event_fifo = {
    .head = 0,
    .tail = 0,
    .data = event_buf+sizeof(UnitFifo),
    .deepth = (EVNET_BUF_SIZE-sizeof(UnitFifo))/sizeof(Event),
    .unit_size = sizeof(Event),
};

//////////////////////////////////////////////////////////////////
#ifdef RTTHREAD
typedef struct {
    uint32_t start_addr;
    uint32_t size;

    uint32_t write_addr;
    uint32_t magic;
} Mtd;

Mtd event_mtd = {
    .start_addr = 0,
    .size = 1024,
    .magic = EVENT_MAGIC,
};

Mtd stat_mtd = {
    .start_addr = 0x400,
    .size = 1024,
    .magic = STAT_MAGIC,
};

void mtd_write(Mtd* self, uint8_t* data, uint16_t len)
{
    rt_err_t ret = RT_EOK;

    rt_device_t rom_dev = rt_device_find("rom");
    if (rom_dev == RT_NULL) {
        rt_kprintf("event_save flash device %s not found!\n", "rom");
        return;
    }

    ret = rt_device_open(rom_dev, RT_DEVICE_FLAG_RDWR);
    if (ret != RT_EOK) {
        rt_kprintf("event_save flash open faild:%d\n", ret);
        return;
    }

    //TODO:Back up the original data

    struct rom_control_erase erase_para;
    erase_para.type = 0x01;
    erase_para.addrstart = DIAGNOSIS_ADDR_START;
    erase_para.pagenums = 1;
    rt_device_control(rom_dev, RT_DEVICE_CTRL_BLK_ERASE, &erase_para);        

    ret = rt_device_write(rom_dev, DIAGNOSIS_ADDR_START+self->start_addr, data, len);
    if (ret != len) {
        rt_kprintf("event_save flash write faild:%d\n", ret);
    }

    ret = rt_device_close(rom_dev);
    if (ret != RT_EOK) {
        rt_kprintf("event_save flash close faild:%d\n", ret);
    }
}

void mtd_read(Mtd* self, uint8_t* data, uint16_t len)
{
    rt_err_t ret = RT_EOK;

    rt_device_t rom_dev = rt_device_find("rom");
    if (rom_dev == RT_NULL) {
        rt_kprintf("event_save flash device %s not found!\n", "rom");
        return;
    }

    ret = rt_device_open(rom_dev, RT_DEVICE_FLAG_RDWR);
    if (ret != RT_EOK) {
        rt_kprintf("event_save flash open faild:%d\n", ret);
        return;
    }

    ret = rt_device_read(rom_dev, DIAGNOSIS_ADDR_START+self->start_addr, data, len);
    if (ret != RT_EOK) {
        rt_kprintf("event_save flash read:%d\n", ret);
    }

    ret = rt_device_close(rom_dev);
    if (ret != RT_EOK) {
        rt_kprintf("event_save flash close faild:%d\n", ret);
    }
}

void storage_write(uint32_t addr, uint8_t* data, uint16_t len)
{
    if(addr == EVENT_ADDR) {
        mtd_write(&event_mtd, buf, event_mtd.size);
    } 
    else if(addr == STAT_ADDR) {
        mtd_write(&stat_mtd, buf, event_mtd.size);
    }
}

void storage_read(uint32_t addr, uint8_t* data, uint16_t len)
{
    if(addr == EVENT_ADDR) {
        mtd_read(&event_mtd, buf, event_mtd.size);
    } 
    else if(addr == STAT_ADDR) {
        mtd_read(&stat_mtd, buf, event_mtd.size);
    }    
}
#endif

//////////////////////////////////////////////////////////////////

#if USE_EVENT
void event_record(uint8_t event, uint8_t reason)
{
    Event e;
    e.timestamp = (uint32_t)TIMESTAMP_FUNC(RT_NULL);
    e.event = event;
    e.reason = reason;  

    UnitFifo_write_force(&event_fifo, &e);
}

Event event_get(int16_t index)
{
    Event e;

    uint16_t pos = index>0? index:event_fifo.deepth+index+1;
    UnitFifo_peek(&event_fifo, pos, &e);

    return e;
}

void event_save(void)
{
    *((UnitFifo*)(event_buf+4)) = event_fifo;
    storage_write(EVENT_ADDR, event_buf, EVNET_BUF_SIZE);
    PRINTF_FUNC("event save!\n");
}

void event_recover(uint8_t* buf, UnitFifo* fifo)
{
    storage_read(EVENT_ADDR, buf, EVNET_BUF_SIZE);
    if(*((uint32_t*)buf) != EVENT_MAGIC) {
        *((uint32_t*)buf) = EVENT_MAGIC;
        *((UnitFifo*)(buf+4)) = *fifo;
        PRINTF_FUNC("event magic corruption, reset!\n");
    } 
    else {
        *fifo = *((UnitFifo*)(buf+4));
        PRINTF_FUNC("event recover from storage!\n");
    }   
}

uint16_t event_export(void)
{
    return 0;
}

void event_print(int16_t start, int16_t end)
{
    UnitFifo_print(&event_fifo);
    Event* event_buf = malloc(event_fifo.deepth*sizeof(Event));
    if(event_buf != NULL) {
        uint16_t event_total = UnitFifo_get_count(&event_fifo);
        for(uint16_t i=0; i<event_total; i++) {
            UnitFifo_peek(&event_fifo, i, &event_buf[i]);
        }
        
        if(start < 0) {
            start = event_total + 1 + start;
        }
        if(end < 0) {
            end = event_total + end;
            if(end < 0) {
                end = 0;
            }
        }
        
        PRINTF_FUNC("No. time\tevent\treason\n");
        struct tm *t;
        uint16_t i=start;
        while(1) {
            if(start > end) {
                if(i<=end) {
                    break;
                }
            }
            else {
                if(i>=end) {
                    break;
                }
            }
            t = LOCALTIME_FUNC(&event_buf[i-1].timestamp);
            PRINTF_FUNC("%03d %d-%02d-%02d_%02d-%02d-%02d %02d#%s\t%d\n", i
                                                            ,t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec
                                                            ,event_buf[i-1].event, ENUM_TO_STRING(event, event_buf[i-1].event), event_buf[i-1].reason);
            if(start > end) {
                i--;
            }
            else {
                i++;
            }
        }
        free(event_buf);
    }
}

#if USE_EVENT_SHELL
void event_shell(int argc, char *argv[])
{
    if(strcmp(argv[1],"print") == 0) {
        if(argc == 2) {
            event_print(-1, -20);
            return;
        }
        else if(argc == 3) {
            event_print(-1, (atoi(argv[2])+1)*-1);
            return;
        }
        else if(argc == 4) {
            event_print(atoi(argv[2]), atoi(argv[3]));
            return;
        }
    }
    
	if(argc == 2) {
		if(strcmp(argv[1],"save") == 0) {
			event_save();
			return;
		}
		else if(strcmp(argv[1],"all") == 0) {
			event_print(-1, 0);
			return;
		}
	}

	cli_device_write("missing command: try 'print' 'save' 'all'");
}
#endif

void event_init(void)
{
    #if USE_EVENT_SHELL
    cli_regist("event", event_shell);
    #endif

    printf("event init!\n");
    storage_recover(EVENT_ADDR, event_buf, &event_fifo);
    event_record(EVENT_POWER_ON, 0);
}
#endif


//////////////////////////////////////////////////////////////////

#if USE_STAT
Stat _diagnosis_stat;

uint8_t stat_buf[STAT_BUF_SIZE];
UnitFifo stat_fifo = {
    .head = 0,
    .tail = 0,
    .data = stat_buf+sizeof(UnitFifo),
    .deepth = STAT_BUF_SIZE-sizeof(UnitFifo)/sizeof(Stat),
    .unit_size = sizeof(Stat),
};

void stat_update_time(void)
{
    if(_diagnosis_stat.start_timestamp < 1500000000) {
        _diagnosis_stat.start_timestamp = (uint32_t)TIMESTAMP_FUNC(RT_NULL) - T2M(rt_tick_get())/1000;
    }
}

void stat_reset(void)
{
    memset(&_diagnosis_stat, sizeof(Stat), 0);
    _diagnosis_stat.magic = STAT_MAGIC;
    _diagnosis_stat.start_timestamp = (uint32_t)TIMESTAMP_FUNC(RT_NULL);
}

void stat_save(void)
{
    *((UnitFifo*)(stat_buf+4)) = stat_fifo;
    storage_write(STAT_ADDR, stat_buf, STAT_BUF_SIZE);
    PRINTF_FUNC("stat save!\n");    
}

void stat_recover(uint8_t* buf, UnitFifo* fifo)
{
    storage_read(STAT_ADDR, buf, STAT_BUF_SIZE);
    if(*((uint32_t*)buf) != STAT_MAGIC) {
        *((uint32_t*)buf) = STAT_MAGIC;
        *((UnitFifo*)(buf+4)) = *fifo;
        PRINTF_FUNC("stat magic corruption, reset!\n");
    } 
    else {
        *fifo = *((UnitFifo*)(buf+4));
        PRINTF_FUNC("stat recover from storage!\n");
    }   
}

void stat_stop(void)
{
    _diagnosis_stat.stop_timestamp = (uint32_t)TIMESTAMP_FUNC(RT_NULL);
    UnitFifo_write_force(&stat_fifo, &_diagnosis_stat);
    stat_save();
}

void stat_unit_print(Stat* s)
{
    struct tm *t;
    t = LOCALTIME_FUNC(&s->start_timestamp);
    PRINTF_FUNC("start time:%d-%02d-%02d_%02d-%02d-%02d\n", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    t = LOCALTIME_FUNC(&s->stop_timestamp);
    PRINTF_FUNC(" stop time:%d-%02d-%02d_%02d-%02d-%02d\n", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    PRINTF_FUNC("system_reset:%d\n", s->system_reset);
}

void stat_print(int16_t start, int16_t end)
{
    uint16_t stat_total = UnitFifo_get_count(&stat_fifo);
    if(stat_total == 0) return;
    Stat* stat_buf = malloc(stat_fifo.deepth*sizeof(Stat));
    if(stat_buf != NULL) {
        
        for(uint16_t i=0; i<stat_total; i++) {
            UnitFifo_peek(&stat_fifo, i, &stat_buf[i]);
        }
        
        if(start < 0) {
            start = stat_total + start;
        }
        if(end < 0) {
            end = stat_total + end;
        }
        
        PRINTF_FUNC("--------------------------\n");
        
        uint16_t i=start;
        while(1) {
            if(start > end) {
                if(i<=end) {
                    break;
                }
            }
            else {
                if(i>=end) {
                    break;
                }
            }
            stat_unit_print(&stat_buf[i-1]);
            PRINTF_FUNC("--------------------------\n");
            if(start > end) {
                i--;
            }
            else {
                i++;
            }
        }
        free(stat_buf);
    }
}

#if USE_STAT_SHELL
void stat_shell(int argc, char *argv[])
{
    if(strcmp(argv[1],"print") == 0) {
        if(argc == 2) {
            stat_unit_print(&_diagnosis_stat);
            return;
        }
        else if(argc == 3) {
            stat_print(-1, (atoi(argv[2])+1)*-1);
            return;
        }
        else if(argc == 4) {
            stat_print(atoi(argv[2]), atoi(argv[3]));
            return;
        }
    }
    
	if(argc == 2) {
		if(strcmp(argv[1],"save") == 0) {
			stat_save();
			return;
		}
		else if(strcmp(argv[1],"all") == 0) {
			stat_print(-1, 0);
			return;
		}
	}

	cli_device_write("missing command: try 'print' 'save' 'all'");
}
#endif

void stat_init(void)
{
    #if USE_STAT_SHELL
    cli_regist("stat", stat_shell);
    #endif

    PRINTF_FUNC("stat init!\n");
    storage_recover(STAT_ADDR, stat_buf, &stat_fifo);
    stat_reset();
}
#endif


//////////////////////////////////////////////////////////////////

#if USE_RATE_PERF
void RatePerf_cnt(RatePerf* self, uint8_t cnt)
{
    self->cnt += cnt;
}

void RatePerf_print(RatePerf* self, char* tag, uint8_t rate_s)
{
    rate_s *=2;
    if(timer_check(&self->timer, rate_s*1000*1000))
    {
        self->rate = self->cnt/(rate_s*1.0f);
        self->cnt = 0;
        //PRINTF_FUNC("[%s]rate:%.2fsps\n", tag, self->rate);
    }  
}

void RatePerf_print_check(RatePerf* self, char* tag, uint8_t rate_s, float threshold)
{
    if(timer_check(&self->timer, rate_s*1000*1000))
    {
        self->rate = self->cnt/(rate_s*1.0f);
        self->cnt = 0;
//        PRINTF_FUNC("[%s]rate:%.2fsps\n", tag, self->rate);
//        if(self->rate < threshold) {
//            PRINTF_FUNC("===================WARN==%s rate low!\n", tag);
//        }        
    }  
}
#endif


//////////////////////////////////////////////////////////////////

#if USE_CNT_PERF
void CntPerf_cnt(CntPerf* self, uint8_t cnt)
{
    self->cnt += cnt; 
}

void CntPerf_print(CntPerf* self, char* tag)
{
    PRINTF_FUNC("%s cnt:%d\n", tag, self->cnt);
}
#endif


//////////////////////////////////////////////////////////////////

#if USE_TIME_PERF
void TimePerf_init(TimePerf* self)
{
    self->t0=0;
    self->t1 = 0;
    self->cnt=0;
    self->avg=0;
    self->sum=0;
    self->t_max=0;
    self->t_min=0xFFFFFFFF;
}

void TimePerf_interval(TimePerf* self)
{
    self->t1 = timer_now() - self->t0;
    if(self->t0 != 0) {
        self->sum += self->t1;
        self->cnt++;
        self->avg = self->sum / self->cnt;
        if(self->t1>self->t_max) self->t_max=self->t1;
        if(self->t1<self->t_min) self->t_min=self->t1;
    }
    self->t0 = timer_now();
}

void TimePerf_begin(TimePerf* self)
{
    self->t0 = timer_now();
}

void TimePerf_end(TimePerf* self)
{
    self->t1 = timer_now() - self->t0;
    if(self->t0 != 0) {
        self->sum += self->t1;
        self->cnt++;
        self->avg = self->sum / self->cnt;
        if(self->t1>self->t_max) self->t_max=self->t1;
        if(self->t1<self->t_min) self->t_min=self->t1;
    }
}

void TimePerf_print(TimePerf* self, char* tag)
{
    PRINTF_FUNC("%s:curr:%06lld avg:%lld min:%lld max:%lld\n", tag, self->t1, self->avg, self->t_min,
          self->t_max);
}
#endif


