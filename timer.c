/**                                               _____           ,-.
 * _______       _____       _____                ___   _,.      /  /
 * ___    |__   ____(_)_____ __  /______________  __   ; \____,-==-._  )
 * __  /| |_ | / /_  /_  __ `/  __/  __ \_  ___/  _    //_    `----' {+>
 * _  ___ |_ |/ /_  / / /_/ // /_ / /_/ /  /      _    `  `'--/  /-'`(
 * /_/  |_|____/ /_/  \__,_/ \__/ \____//_/       _          /  /
 *                                                           `='
 *
 * timer.c
 *
 * v2.0
 *
 * simple timer, delay and time block function
 */

#include "support.h"

#include "timer.h"
#ifdef LINUX
    #include <time.h>
#endif

#if USE_RTC
    #include "rtc.h"
#endif


static volatile Times timer_cnt = 0;
#ifdef LINUX
    static struct timespec boot_time;
#elif defined(ESP)
    static os_timer_t work_timer;
#endif


Times timer_now(void)
{
#ifdef LINUX
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    return (Times)((now.tv_sec-boot_time.tv_sec)*1000000+(now.tv_nsec-boot_time.tv_nsec) / 1000);
#else
    return timer_cnt*US_PER_TICK;
#endif
}

Times timer_new(uint32_t us)
{
    return timer_now()+us;
}

bool timer_is_timeout(Times* t)
{
    if(*t >= timer_now()) {
        return false;
    }
    else {
        return true;
    }
}

static Times timer_passed(Times* since)
{
    Times now = timer_now();

    if(now >= *since) {
        return now-*since;
    }

    return now+(TIME_MAX-*since+1);
}

Times timer_elapsed(Times* t)
{
    return timer_passed(t);
}

bool timer_check(Times* t, Times us)
{
    if(timer_passed(t) > us) {
        *t = timer_now();
        return true;
    }
    return false;
}

float timer_get_dt(Times* t, float max, float min)
{
    float dt = (*t > 0) ? ((timer_now()-*t) / 1000000.0f) : min;
    *t = timer_now();

    if(dt > max) {
        dt = max;
    }
    if(dt < min) {
        dt = min;
    }
    return dt;
}

void delay(float s)
{
    volatile Times wait;

    wait = timer_new((uint32_t)(s*1000*1000));
    while(!timer_is_timeout((Times*)&wait));
}

void delay_ms(uint32_t ms)
{
    volatile Times wait;

    wait = timer_new(ms*1000);
    while(!timer_is_timeout((Times*)&wait));
}

void delay_us(uint32_t us)
{
    volatile Times wait;

    wait = timer_new(us);
    while(!timer_is_timeout((Times*)&wait));
}

#ifndef LINUX
void sleep(float s)
{
    volatile Times wait;

    wait = timer_new((uint32_t)(s*1000*1000));
    while(!timer_is_timeout((Times*)&wait));
}
#endif

void timer_disable(void)
{
#ifdef STM32F3
    NVIC_DisableIRQ(TIM7_IRQn);

#elif defined(ESP)
    os_timer_disarm(&work_timer);

#elif defined(APOLLO)
    am_hal_ctimer_stop(2, AM_HAL_CTIMER_TIMERA);
#endif
}

#ifdef STM32F3
static void timer_isr(void)
{
    TIM_ClearFlag(TIM7, TIM_IT_Update);
    timer_cnt++;
}

#elif defined(ESP)
void timer_isr(void* arvg)
{
    timer_cnt++;

#if USE_RTC
    if(timer_cnt%(1000*1000/US_PER_TICK) == 0) {
        rtc_second_inc();
    }
#endif
}

#elif defined(APOLLO)
static void timer_isr(void)
{
    uint32_t status;

    status = am_hal_ctimer_int_status_get(true);

    am_hal_ctimer_int_clear(status);    
    
    if(status & AM_HAL_CTIMER_INT_TIMERA2) {
        timer_cnt++;
    }
}
#endif

#ifdef STM32F3
void TIM7_IRQHandler(void)
{
    timer_isr();
}

#elif defined(APOLLO)
void am_ctimer_isr(void)
{
    timer_isr();
}

#endif

void timer_init()
{
#ifdef STM32F3
    TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    TIM_DeInit(TIM7);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 10-1;                //10us
    TIM_TimeBaseStructure.TIM_Prescaler = 72;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ARRPreloadConfig(TIM7, DISABLE);

    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM7, ENABLE);
#elif defined(LINUX)
    clock_gettime(CLOCK_MONOTONIC, &boot_time);

#elif defined(ESP)
    os_timer_setfn(&work_timer, timer_isr, NULL);
    os_timer_arm(&work_timer, US_PER_TICK/1000, 1);

#elif defined(APOLLO)
    am_hal_ctimer_config_single(2, AM_HAL_CTIMER_TIMERA,
                                AM_HAL_CTIMER_HFRC_12MHZ |
                                AM_HAL_CTIMER_FN_REPEAT |
                                AM_HAL_CTIMER_INT_ENABLE
                               );

    am_hal_ctimer_period_set(2, AM_HAL_CTIMER_TIMERA, (12*1000*1000/(TIMER_RATE_HZ) - 1), 0);

    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA2);
    am_hal_interrupt_enable(AM_HAL_INTERRUPT_CTIMER);

    am_hal_ctimer_start(2, AM_HAL_CTIMER_TIMERA);
#endif
}
