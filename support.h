#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef F3_EVO
    #include "stm32f30x.h"
    #define STM32F3
#elif LINUX
#elif SM702
    #include "am_mcu_apollo.h"
    #define APOLLO
#endif

#define MM_HEAP_SIZE        (4*1024)
#define MM_BYTE_ALIGNMENT  4

#ifdef F3_EVO
    #define TASK_MAX     20

    #define US_PER_TICK         (10)
    #define TIMER_WIDTH_BIT     64

    #define MPU6050_I2C     I2C1
    #define HMC5883_I2C     I2C1
    #define MS5611_I2C      I2C1

    #define SPI_FLAHS_SPI   SPI2
    #define FLASH_PAGESIZE  256
    #define FLASH_PAGES_PER_SECTOR      256
    #define FLASH_SECTORSIZE (FLASH_PAGESIZE*FLASH_PAGES_PER_SECTOR)
    #define FLASH_SECTOR_NUM    32
    #define FLASH_SIZE      (FLASH_SECTORSIZE*FLASH_SECTOR_NUM)

    #define INERTIAL_SENSOR_ROTATION    ROTATION_ROLL_180_YAW_270

    #define MPU6050_ACCEL_DEFAULT_RATE				1000.0f
    #define MPU6050_GYRO_DEFAULT_RATE				1000.0f

    #define MPU6050_ACCEL_XY_DEFAULT_FILTER_FREQ	30.0f
    #define MPU6050_ACCEL_Z_DEFAULT_FILTER_FREQ	    30.0f
    #define MPU6050_GYRO_XY_DEFAULT_FILTER_FREQ		30.0f
    #define MPU6050_GYRO_Z_DEFAULT_FILTER_FREQ		30.0f

#elif LINUX
    #define TASK_MAX     20

    #define US_PER_TICK         (10*1000)  //in linux not need
    #define TIMER_WIDTH_BIT     64

    #define SYSTEM_CYCLE              (1000)

    #define FLASH_PAGESIZE  8096

    #define MTD_FILE_SIZE_MAX (10*1024*1024)
    #define MTD_PATH "/tmp/file.mtd"


    #define MPU6050_PATH "/dev/mpu6050"
    #define INERTIAL_SENSOR_ROTATION    ROTATION_ROLL_180_YAW_90

    #define MPU6050_ACCEL_DEFAULT_RATE				500
    #define MPU6050_GYRO_DEFAULT_RATE				500

    #define MPU6050_ACCEL_XY_DEFAULT_FILTER_FREQ	45
    #define MPU6050_ACCEL_Z_DEFAULT_FILTER_FREQ	    30
    #define MPU6050_GYRO_XY_DEFAULT_FILTER_FREQ		65
    #define MPU6050_GYRO_Z_DEFAULT_FILTER_FREQ		45

    #define SPL06_PATH "/dev/spl06"
    #define DPS280_PATH "/dev/dps280"

#elif ESP
    #define TASK_MAX     20

    #define US_PER_TICK         (10*1000)
    #define TIMER_WIDTH_BIT     64

#elif SM702
    #define TASK_MAX     20

    #define RTTHREAD
    #define TICK_RATE_MHZ       512
    #define US_PER_TICK         (1.0f*1000*1000/TICK_RATE_MHZ)
    #define TIMER_WIDTH_BIT     64
#endif



