#pragma once

#include <string.h>
#include <stdlib.h>

#ifndef GLOBAL_CONFIG
#define SHELL_NUM               100
#define CMD_LEN_MAX             50
#define CLI_READ_BUF_SIZE       100
#define CLI_WRITE_BUF_SIZE      500
#define CLI_ARGV_MAX            10
#endif

typedef void (*shell_func)(int argc, char* argv[]);

void cli_init(void);
void cli_update(void);
void cli_regist(const char* name, shell_func cmd);
void cli_device_write(const char* format, ...);

