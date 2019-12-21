/**
 *  .----. .----. .-. .----. .-.   .----.       .-.       
 * /  {}  \| {}  }| |/  {}  \| |   | {_        /'v'\      
 * \      /| .-. \| |\      /| `--.| {__      (/   \)     
 *  `----' `-' `-'`-' `----' `----'`----'    ='="="===<   
 *                                              |_|
 * utils.h
 *
 * v1.1
 *
 * Something help program
 */
#pragma once



#define BIG32(a)         (((uint8_t*)(a))[0]<<24| \
                          ((uint8_t*)(a))[1]<<16| \
                          ((uint8_t*)(a))[2]<<8| \
                          ((uint8_t*)(a))[3])

#define BIG16(a)          (((uint8_t*)(a))[0]<<8|((uint8_t*)(a))[1])


#define NUM2BIN(n)          ((n>>21 & 0x80) | \
                             (n>>18 & 0x40) | \
                             (n>>15 & 0x20) | \
                             (n>>12 & 0x10) | \
                             (n>> 9 & 0x08) | \
                             (n>> 6 & 0x04) | \
                             (n>> 3 & 0x02) | \
                             (n     & 0x01))
#define BIN(n) NUM2BIN(0x##n##l)

#define MAX(a, b)   ((a)>(b)?(a):(b))

#define RET_OK                          0
#define RET_ERROR                       1
#define RET_TIMEOUT                     2
