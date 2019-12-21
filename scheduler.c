/**
 *  .----. .----. .-. .----. .-.   .----.       .-.       
 * /  {}  \| {}  }| |/  {}  \| |   | {_        /'v'\      
 * \      /| .-. \| |\      /| `--.| {__      (/   \)     
 *  `----' `-' `-'`-' `----' `----'`----'    ='="="===<   
 *                                              |_|
 * scheduler.c
 *
 * v1.2
 *
 * Simple scheduling system, task form
 */
#include "support.h"
#include "scheduler.h"
#include "mm.h"
#include <string.h>

/*
If use print function, must define printf function
like. 
#include "debug.h"
#define PRINTF_FUNC             PRINT
*/
#include "debug.h"

#define PRINTF_FUNC             PRINT


Task* task_tab[TASK_MAX] = {NULL};
uint8_t task_cnt=0;


void task_set_rate(Task* t, Times time)
{
    t->rate = time;
}

void task_disable(Task* t)
{
    t->run = false;
}

Task* task_create(char* name, Times interval, task_func cb)
{
    if(task_cnt >= TASK_MAX) return NULL;

    task_tab[task_cnt] = (Task*)mm_malloc(sizeof(Task));
    if(task_tab[task_cnt] == NULL) return NULL;

    task_tab[task_cnt]->callback = cb;
    task_tab[task_cnt]->rate = interval;
    task_tab[task_cnt]->last_run = 0;
    task_tab[task_cnt]->run = true;
    memcpy(task_tab[task_cnt]->name, name, TASK_NAME_MAX);
    task_tab[task_cnt]->name[TASK_NAME_MAX-1] = '\0';

    task_cnt++;

    return task_tab[task_cnt-1];
}

Task* task_create_ex(char* name, Times interval, task_func cb, void* argv)
{
    Task* task = task_create(name, interval, cb);
    if(task != NULL) {
        task->argv = argv;
    }
    return task;
}

void scheduler_run(void)
{
    for(uint8_t i=0; i<task_cnt; i++) {
        if(task_tab[i]->run) {
//            PRINT("task:%d ",i);
            if(timer_check(&task_tab[i]->last_run, task_tab[i]->rate)) {
                task_tab[i]->callback(task_tab[i]->argv);
                // PRINT("run \n");
            }
            else {
                // PRINT("wait \n");
            }
        }
    }
}

void task_print(void)
{
    PRINT("NAME\t\tCYCLE/us\tSTATUS\n");
    for(uint8_t i=0; i<task_cnt; i++) {
        if(strlen(task_tab[i]->name) > 6) {
            PRINT("[%s]\t%lld\t\t%s\n", task_tab[i]->name, task_tab[i]->rate, task_tab[i]->run? "run":"idle");
        }
        else {
            PRINT("[%s]\t\t%lld\t\t%s\n", task_tab[i]->name, task_tab[i]->rate, task_tab[i]->run? "run":"idle");
        }
    }
}

void task_shell(int argc, char* argv[]);

void task_init(void)
{
#if USE_TASK_SHELL
    cli_regist("task", task_shell);
#endif    
}

#if USE_TASK_SHELL
void task_shell(int argc, char* argv[])
{
    if(argc == 2) {
        if(strcmp(argv[1], "list") == 0) {
            task_print();
            return;
        }
    }

    cli_device_write("missing command: try 'list'");
}
#endif
