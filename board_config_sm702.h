#pragma once


#define ORIOLE_WORKS
#define GLOBAL_CONFIG

#define USE_RTT                 1

#define TASK_MAX                20
#define TASK_NAME_MAX           10
#define USE_TASK_SHELL          1

#define TIMER_RATE_HZ           1000
#define TIMER_WIDTH_BIT         64
#define USE_RTC_INC             0

#define USE_MM                  MM_MODULE_DYNAMIC_ADDR
#define MM_HEAP_SIZE            (8*1024)
#define USE_MM_SHELL            1
#define USE_MM_STAT             1

#define USE_DEBGU_SHELL         1 
#define USE_PRINT_RTC           0 

#define SHELL_NUM               50
#define CMD_LEN_MAX             50
#define CLI_READ_BUF_SIZE       100
#define CLI_WRITE_BUF_SIZE      500
#define CLI_ARGV_MAX            10

#define USE_DATA_WIN            0
#define USE_UNIT_FIFO           1
#define USE_FLOAT_FIFO          0
#define USE_FIFO                1
#if USE_FIFO
	#define USE_FIFO_MODIFY     0
	#define USE_FIFO_PRINT      0
#endif

#define USE_EVENT               0
#define USE_EVENT_SHELL         0
#define USE_STAT                0
#define USE_STAT_SHELL          0
#define USE_RATE_PERF           1
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

#define BLE_READY_INT           46
#define MTU_LEN                 128
#define BLE_RXBUF_SIZE          256    
#define BLE_TXBUF_SIZE          256    
#define BLE_TX_QUEUE_SIZE       10    

#define BATT_GPIO               16
#define BATT_GPIO_FUNC          AM_HAL_PIN_16_ADCSE0
#define BATT_TIMER_NUM          3
#define BATT_ADC_CHANNEL        AM_HAL_ADC_SLOT_CHSEL_SE0
#define BATT_SAMPLE_RATE        8
#define BATT_BUF_SIZE           20
#define BATT_AVG_WIN            8

#define MAX30001_SPI            1
#define MAX30001_CS_GPIO        8
#define MAX30001_POWER_EN_GPIO  18
#define MAX30001_READY_GPIO     2
#define MAX30001_CLK_IN_GPIO    4
#define MAX30001_RXBUF_SIZE     50    
#define MAX30001_TXBUF_SIZE     20    

#define PRT_RXBUF_SIZE          256    
#define PRT_TXBUF_SIZE          256    

typedef enum {
    DEBUG_ID_NULL = 0,
    DEBUG_ID_I2C,
    DEBUG_ID_BLE,
    DEBUG_ID_MPU6050,
    DEBUG_ID_MAX30001,
    DEBUG_ID_SERIAL,
    DEBUG_ID_SPI,
    DEBUG_ID_RTC,
    DEBUG_ID_BATT,
    DEBUG_ID_TIMER,
    DEBUG_ID_CLI,
    DEBUG_ID_SCHEDULER,
    DEBUG_ID_DEBUG,
    DEBUG_ID_FIFO,
    DEBUG_ID_PERF,
    DEBUG_ID_MM,
    DEBUG_ID_PROTOCOL,
    DEBUG_ID_MAX,
} DebugID;
