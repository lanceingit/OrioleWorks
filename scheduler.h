#pragma once

#include "timer.h"

#ifndef GLOBAL_CONFIG
#define TASK_MAX            20
#define TASK_NAME_MAX       10
#define USE_TASK_SHELL      0
#endif

typedef void(*task_func)(void* argv);


typedef struct {
    char name[TASK_NAME_MAX];
    Times rate;
    Times time_use;
    Times last_run;
    task_func callback;
    bool run;
    void* argv;
} Task;

void task_init(void);
Task* task_create(char* name, Times interval, task_func cb);
Task* task_create_ex(char* name, Times interval, task_func cb, void* argv);
void task_set_rate(Task* t, Times time);
void task_disable(Task* t);

void scheduler_run(void);

////////////////////////////////////////
bool sem_check(bool* cond);
