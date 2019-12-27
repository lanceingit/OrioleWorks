#pragma once

#ifndef GLOBAL_CONFIG
#define USE_DATA_WIN    0
#define USE_UNIT_FIFO   0
#define USE_FLOAT_FIFO  0
#define USE_FIFO        1
#if USE_FIFO
    #define USE_FIFO_MODIFY 0
    #define USE_FIFO_PRINT  0
#endif
#endif

#if USE_DATA_WIN
typedef struct {
    FloatFifo fifo;
} DataWin;

#define DATA_WIN_DEF(name, s) \
float _##name##_buf[s]={0}; \
DataWin name = { \
    .fifo = { \
        .head = 0, \
        .tail = 0, \
        .data = _##name##_buf, \
        .size = s, \
        .cnt = 0, \
    }, \
};

void datawin_init(DataWin* self, float* buf, uint16_t size);
void datawin_set(DataWin* self, float data);
float datawin_get_avg(DataWin* self);
float datawin_get_std(DataWin* self);
float datawin_get_median(DataWin* self);
float datawin_get_peakp(DataWin* self);
#endif

//////////////////////////////////////////////////////////////////////
#if USE_UNIT_FIFO
typedef struct {
    uint16_t head;
    uint16_t tail;
    void* data;
    uint16_t deepth;
    uint8_t unit_size;
} UnitFifo;

void UnitFifo_init(UnitFifo* self, void* buf, uint16_t deepth, uint8_t unit_size);
int8_t UnitFifo_write(UnitFifo* self, void* u);
void UnitFifo_write_force(UnitFifo* self, void* u);
int8_t UnitFifo_read(UnitFifo* self, void* u);
int8_t UnitFifo_peek(UnitFifo* self, uint16_t pos, void* u);
bool UnitFifo_is_empty(UnitFifo* self);
uint16_t UnitFifo_get_count(UnitFifo* self);
#endif

//////////////////////////////////////////////////////////////////////
#if USE_FLOAT_FIFO
typedef struct {
    uint16_t head;
    uint16_t tail;
    float* data;
    uint16_t size;
    uint16_t cnt;
} FloatFifo;

void FloatFifo_init(FloatFifo* self, float* buf, uint16_t size);
int8_t FloatFifo_write(FloatFifo* self, float c);
void FloatFifo_write_force(FloatFifo* self, float c);
int8_t FloatFifo_read(FloatFifo* self, float* c);
bool FloatFifo_is_empty(FloatFifo* self);
uint16_t FloatFifo_get_count(FloatFifo* self);
#endif

//////////////////////////////////////////////////////////////////////
#if USE_FIFO
typedef struct {
    uint16_t head;
    uint16_t tail;
    uint8_t* data;
    uint16_t size;
    uint16_t cnt;
} Fifo;

void fifo_init(Fifo* self, uint8_t* buf, uint16_t size);
int8_t fifo_write(Fifo* self, uint8_t c);
void fifo_write_force(Fifo* self, uint8_t c);
int8_t fifo_read(Fifo* self, uint8_t* c);

bool fifo_is_empty(Fifo* self);
uint16_t fifo_get_count(Fifo* self);

#if USE_MODIFY_FIFO
uint8_t* fifo_get_tail(Fifo* self);
void fifo_set_tail(Fifo* self, uint8_t* new_tail);

uint16_t fifo_get_tail_index(Fifo* self);
void fifo_set_tail_index(Fifo* self, uint16_t new_index);
#endif //USE_MODIFY_FIFO

#if USE_FIFO_PRINT
void fifo_print(Fifo* self);
#endif
#endif
