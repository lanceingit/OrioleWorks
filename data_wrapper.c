/**
 *  .----. .----. .-. .----. .-.   .----.       .-.       
 * /  {}  \| {}  }| |/  {}  \| |   | {_        /'v'\      
 * \      /| .-. \| |\      /| `--.| {__      (/   \)     
 *  `----' `-' `-'`-' `----' `----'`----'    ='="="===<   
 *                                              |_|
 * data_wrapper.c
 *
 * v1.0
 *
 * Implement data structure
 */
#include "support.h"

#include "data_wrapper.h"

#if USE_FIFO_PRINT
/*
If use print function, must define printf function
like. 
#include "debug.h"
#define PRINTF_FUNC             PRINT
*/
#include "debug.h"

#define PRINTF_FUNC             PRINT
#endif

#if USE_DATA_WIN
void datawin_init(DataWin* self, float* buf, uint16_t size)
{
    fifo_f_init(&self->fifo, buf, size);
}

void datawin_set(DataWin* self, float data)
{
    fifo_f_write_force(&self->fifo, data);
}

float datawin_get_avg(DataWin* self)
{
    float sum=0;
    for(uint8_t i=0; i<self->fifo.cnt; i++) {
        sum += self->fifo.data[i];
    }
    return sum/self->fifo.cnt;
}

float datawin_get_std(DataWin* self)
{
    return std(self->fifo.data, self->fifo.cnt);
}

float datawin_get_median(DataWin* self)
{
    return median(self->fifo.data, self->fifo.cnt);
}

float datawin_get_peakp(DataWin* self)
{
    float min = self->fifo.data[0];
    float max = self->fifo.data[0];
    for(uint16_t i=0; i<self->fifo.cnt; i++) {
        if(self->fifo.data[i] < min) {
            min = self->fifo.data[i];
        }
        if(self->fifo.data[i] > max) {
            max = self->fifo.data[i];
        }
    }
    return max-min;    
}
#endif


//////////////////////////////////////////////////////////////////////

#if USE_UNIT_FIFO
uint16_t UnitFifo_get_next(UnitFifo* self, uint16_t index)
{
    return (index>=((self->deepth-1)*self->unit_size)? 0: (index+self->unit_size));
}

uint16_t UnitFifo_get_count(UnitFifo* self)
{
    uint16_t head = self->head;
    uint16_t tail = self->tail;
    return (head >= tail? (head-tail)/self->unit_size: self->deepth-(tail-head)/self->unit_size);
}

bool UnitFifo_is_empty(UnitFifo* self)
{
    return (UnitFifo_get_count(self) == 0);
}
 
int8_t UnitFifo_write(UnitFifo* self, void* u)
{
    uint16_t next = UnitFifo_get_next(self, self->head);

    if(next != self->tail) {
        memcpy(&((uint8_t*)(self->data))[self->head], u, self->unit_size);
        self->head = next;
        return 0;
    }
    else {
        return -1;
    }
}

void UnitFifo_write_force(UnitFifo* self, void* u)
{
    uint16_t next = UnitFifo_get_next(self, self->head);
    
    memcpy(&((uint8_t*)(self->data))[self->head], u, self->unit_size);
    if(UnitFifo_get_count(self) >= self->deepth-1) {
        self->tail = UnitFifo_get_next(self, self->tail);
    }
    self->head = next;    
}

int8_t UnitFifo_read(UnitFifo* self, void* u)
{
    if(UnitFifo_get_count(self) == 0) {
        return -1;
    }

    uint16_t next = UnitFifo_get_next(self, self->tail);
    
    memcpy(u, &((uint8_t*)(self->data))[self->tail], self->unit_size);
    self->tail = next;

    return 0;
}

int8_t UnitFifo_peek(UnitFifo* self, uint16_t pos, void* u)
{
    if(UnitFifo_get_count(self) <= pos) {
        return -1;
    }

    uint16_t read=self->tail;
    for(uint16_t i=0; i<pos; i++) {
        read = UnitFifo_get_next(self, read);
    }

    memcpy(u, &((uint8_t*)(self->data))[read], self->unit_size);

    return 0;
}

void UnitFifo_init(UnitFifo* self, void* buf, uint16_t deepth, uint8_t unit_size)
{
    self->head = 0;
    self->tail = 0;
    self->data = buf;
    self->deepth = deepth;
    self->unit_size = unit_size;
}
#endif


//////////////////////////////////////////////////////////////////////

#if USE_FLOAT_FIFO
void FloatFifo_init(FloatFifo* self, float* buf, uint16_t size)
{
    self->head = 0;
    self->tail = 0;
    self->data = buf;
    self->size = size;
    self->cnt = 0;
}

int8_t FloatFifo_write(FloatFifo* self, float c)
{
    if(self->cnt == self->size) {
        return -1;
    }

    self->data[self->head] = c;
    self->head++;
    self->cnt++;
    if(self->cnt > self->size) {
        self->cnt = self->size;
    }
    if(self->head >= self->size) {
        self->head = 0;
    }

    return 0;
}

void FloatFifo_write_force(FloatFifo* self, float c)
{
    self->data[self->head] = c;
    self->head++;
    self->cnt++;
    if(self->cnt > self->size) {
        self->cnt = self->size;
    }
    if(self->head >= self->size) {
        self->head = 0;
    }
    if(self->cnt == self->size) {
        self->tail = self->head;
    }
}

int8_t FloatFifo_read(FloatFifo* self, float* c)
{
    if(self->cnt == 0) {
        return -1;
    }

    *c = self->data[self->tail];
    self->tail++;
    self->cnt--;
    if(self->tail >= self->size) {
        self->tail = 0;
    }

    return 0;
}

bool FloatFifo_is_empty(FloatFifo* self)
{
    return (self->cnt == 0);
}

uint16_t FloatFifo_get_count(FloatFifo* self)
{
    return self->cnt;
}
#endif


//////////////////////////////////////////////////////////////////////

#if USE_FIFO
void fifo_init(Fifo* self, uint8_t* buf, uint16_t size)
{
    self->head = 0;
    self->tail = 0;
    self->data = buf;
    self->size = size;
    self->cnt = 0;
}

int8_t fifo_write(Fifo* self, uint8_t c)
{
    // PRINTF_FUNC("fifo_write");
    // fifo_print(fifo);

    if(self->cnt == self->size) {
        return -1;
    }

    self->data[self->head] = c;
    self->head++;
    self->cnt++;
    if(self->cnt > self->size) {
        self->cnt = self->size;
    }
    if(self->head >= self->size) {
        self->head = 0;
    }

    return 0;
}

void fifo_write_force(Fifo* self, uint8_t c)
{
    self->data[self->head] = c;
    self->head++;
    self->cnt++;
    if(self->cnt > self->size) {
        self->cnt = self->size;
    }
    if(self->head >= self->size) {
        self->head = 0;
    }
    if(self->cnt == self->size) {
        self->tail = self->head;
    }

}

int8_t fifo_read(Fifo* self, uint8_t* c)
{
    // PRINTF_FUNC("fifo_read");
    // fifo_print(fifo);

    if(self->cnt == 0) {
        // self->cnt = 0;
        return -1;
    }

    *c = self->data[self->tail];
    self->tail++;
    self->cnt--;
    if(self->tail >= self->size) {
        self->tail = 0;
    }

    return 0;
}

bool fifo_is_empty(Fifo* self)
{
    return (self->cnt == 0);
}

uint16_t fifo_get_count(Fifo* self)
{
    return self->cnt;
}

#if USE_MODIFY_FIFO
uint16_t fifo_get_tail_index(Fifo* self)
{
    return self->tail;
}

void fifo_set_tail_index(Fifo* self, uint16_t new_index)
{
    self->tail = new_index;
}

uint8_t* fifo_get_tail(Fifo* self)
{
    return self->data+self->tail;
}


#define IS_TAIL_BEHAND_HEAD (self->tail < self->head)
#define IS_TAIL_FRONT_HEAD  (self->tail > self->head)
#define IS_BEYOND_HEAD(x)   (((x)>self->head && (x)<self->tail && IS_TAIL_FRONT_HEAD) \
                          || ((x)>self->head && (x)>self->tail && IS_TAIL_BEHAND_HEAD))

void fifo_set_tail(Fifo* self, uint8_t* new_tail)
{
    uint16_t new_index;

    new_index = new_tail - self->data;
    if(new_index > self->size) {
        new_index -= (self->size-1);
    }

    if(IS_TAIL_BEHAND_HEAD) {
        if(new_index > self->head) {
            self->tail = self->head;
        }
        else {
            self->tail = new_index;
        }
    }
    else if(IS_TAIL_FRONT_HEAD) {
        if(IS_BEYOND_HEAD(new_index)) {
            self->tail = self->head;
        }
        else {
            self->tail = new_index;
        }
    }
    else {   //end to end
        self->tail = self->head;
    }
}

#endif //USE_MODIFY_FIFO

#if USE_FIFO_PRINT
void fifo_print(Fifo* self)
{
    PRINTF_FUNC("-------fifo--------\n");
    PRINTF_FUNC("addr:%p\n", self);
    PRINTF_FUNC("head:%u\n", self->head);
    PRINTF_FUNC("tail:%u\n", self->tail);
    PRINTF_FUNC("data:");
    for(uint8_t i=0; i<10; i++) {
        PRINTF_FUNC("%x ", self->data[i]);
    }
    PRINTF_FUNC("\n");
    PRINTF_FUNC("size:%u\n", self->size);
    PRINTF_FUNC("cnt:%u\n", self->cnt);
    PRINTF_FUNC("-------------------\n");
}
#endif
#endif


