#pragma once

#include "timer.h"

#ifndef GLOBAL_CONFIG
#define USE_TASK_SHELL      0
#define USE_TASK_PRINT      0

#define TASK_MAX            20
#define TASK_NAME_MAX       10
#endif

typedef void(*task_callback_func)(void);


typedef struct {
    char name[TASK_NAME_MAX];
    times_t rate;
    times_t time_use;
    times_t last_run;
    task_callback_func callback;
    bool run;
} Task;

void task_init(void);
Task* task_create(char* name, times_t interval, task_callback_func cb);
void task_set_rate(Task* t, times_t time);
void task_disable(Task* t);

void scheduler_run(void);


