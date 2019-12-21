/**
 *  .----. .----. .-. .----. .-.   .----.       .-.       
 * /  {}  \| {}  }| |/  {}  \| |   | {_        /'v'\      
 * \      /| .-. \| |\      /| `--.| {__      (/   \)     
 *  `----' `-' `-'`-' `----' `----'`----'    ='="="===<   
 *                                              |_|
 * cli.c
 *
 * v1.5
 *
 * Command line interface module
 */
#include "support.h"

#include <string.h>
#include <stdarg.h>
#include "cli.h"

/*
Must used a dynamic memory function
and a vsprintf function
like. 
#include "mm.h"
#include "debug.h"
#define PRINTF_FUNC             PRINT
#define VSPRINTF_FUNC           evsprintf
#define MALLOC_FUNC             mm_malloc
#define FREE_FUNC               mm_free
*/
#include "mm.h"
#include "debug.h"

#define PRINTF_FUNC             PRINT
#define VSPRINTF_FUNC           evsprintf
#define MALLOC_FUNC             mm_malloc
#define FREE_FUNC               mm_free


/*
intput -> string ->  cmd/arg0/arg1/... -> function(arg0,arg1,...) -> output
name -> cmd  == string -> function
string -> arg == string parse
*/

typedef struct {
    const char* name;
    shell_func cmd;
} Shell;

Shell shell[SHELL_NUM];
uint16_t shell_cnt=0;

uint8_t cli_read_buf[CLI_READ_BUF_SIZE];
uint8_t cli_write_buf[CLI_WRITE_BUF_SIZE];

char curr_cmd[CMD_LEN_MAX+1];
uint8_t cmd_parse_index=0;


/*
Hardware layer configuration

void cli_device_init(void)
uint16_t cli_input(uint8_t* data, uint16_t size)
void cli_output(char* buf, uint16_t len)
*/
#ifdef USE_RTT
#include "SEGGER_RTT.h"

void cli_device_init(void)
{
}

uint16_t cli_input(uint8_t* data, uint16_t size)
{
    return SEGGER_RTT_Read(0, data, size);
}

void cli_output(char* buf, uint16_t len)
{
    SEGGER_RTT_Write(0, buf, len);
}

#elif defined(ESP)
void cli_device_init(void)
{

}

uint16_t cli_input(uint8_t* data, uint16_t size)
{
    return 0;
}

void cli_output(char* buf, uint16_t len)
{
    os_printf("%s", buf);
}
#elif defined(ORIOLE_WORKS)
#include "serial.h"

#define RX_BUF_SIZE 300    
#define TX_BUF_SIZE 512 

Serial* cli_port;
uint8_t cli_rxBuf[RX_BUF_SIZE]; 
uint8_t cli_txBuf[TX_BUF_SIZE];

void cli_device_init(void)
{
    cli_port = serial_open(CLI_UART, 115200, cli_rxBuf, RX_BUF_SIZE, cli_txBuf, TX_BUF_SIZE);
}

uint16_t cli_input(uint8_t* data, uint16_t size) 
{
    uint16_t i;
    
    for(i=0; i<size; i++) {
        if(serial_read(cli_port, &data[i]) < 0)
            break;
    }    
    
    return i;   
}

void cli_output(char* buf, uint16_t len)
{
    serial_write(cli_port, buf, len);  
}

#elif defined(LINUX)
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define CLI_PORT  14558
static int cli_socket_fd = -1;
static struct sockaddr_in recv_addr;
static int addr_len = 0;

void cli_device_init(void)
{
    int flag = 0;
    struct sockaddr_in addr;

    if((cli_socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cli socket failed\n");
        exit(1);
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(CLI_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY) ;
    flag = fcntl(cli_socket_fd, F_GETFL, 0);
    fcntl(cli_socket_fd, F_SETFL, flag | O_NONBLOCK);

    if(bind(cli_socket_fd, (struct sockaddr*)&addr, sizeof(addr))<0) {
        perror("cli bind failed\n");
        exit(1);
    }
    else {
        printf("cli init success\n");
    }
}

uint16_t cli_input(uint8_t* data, uint16_t size) 
{
    int len = 0;

    bzero(socket_buffer, size);
    len = recvfrom(cli_socket_fd, data, size, 0, (struct sockaddr*)&recv_addr,
                   (socklen_t*)&addr_len);

    if(len > 0) {
//        char* client_ip = inet_ntoa(recv_addr.sin_addr);
//        printf("ip:%s port:%d len:%d\n", client_ip, recv_addr.sin_port, len);
//        for(uint16_t i=0; i<len; i++) {
//        	printf("%x ", data[i]);
//        }
//        printf("\n");
    }
    return len;
}

void cli_output(char* buf, uint16_t len)
{
    sendto(cli_socket_fd, buf, len, 0, (struct sockaddr*)&recv_addr, addr_len);
}

#elif defined(RTTHREAD)
#include <rtdevice.h>
#include <dfs_posix.h>

static rt_device_t cli_dev = RT_NULL;

void cli_thread_entry(void* parameter)
{
    while(1) {
        cli_updata();
        rt_thread_delay(RT_TICK_PER_SECOND/50);
    }
}

void cli_device_init(void)
{
    rt_thread_t cli_thread;

    cli_dev = rt_console_get_device();

    cli_thread = rt_thread_create("cli",
                                  cli_thread_entry, RT_NULL,
                                  2*1024, 10, 10);
    if(cli_thread != RT_NULL) {
        rt_thread_startup(cli_thread);
    }
}

uint16_t cli_input(uint8_t* data, uint16_t size)
{
    int len;
    len = rt_device_read(cli_dev, 0, buffer, size);

    if(len > 0) {
        return len;
    }
    else {
        len = rt_strlen(protocol_buf);
        if(len > 0) {
            rt_strncpy((char*)buffer, protocol_buf, len);
            protocol_buf[0] = 0;
        }
    }
    return len;
}

void cli_output(char* buf, uint16_t len)
{
    rt_printf("%s", buf);
}
#endif


///////////////////////////////////////////////////////

void help_shell(int argc, char* argv[])
{
    for(uint8_t i=0; i<shell_cnt; i++) {
        cli_device_write("%s\n", shell[i].name);
    }
    cli_device_write("\n");
}

void reboot_shell(int argc, char* argv[])
{
#ifdef STM32F3
    NVIC_SystemReset();
#elif defined(LINUX)
#elif defined(APOLLO)
    am_hal_sysctrl_aircr_reset();
#endif
}


///////////////////////////////////////////////////////

void cli_regist(const char* name, shell_func cmd)
{
    shell[shell_cnt].name = name;
    shell[shell_cnt].cmd = cmd;
    shell_cnt++;
}

uint16_t cli_device_read(uint8_t* data, uint16_t size)
{
    return cli_input(data, size);
}

void cli_device_write(const char* format, ...)
{
    va_list args;
    int len;

    va_start(args, format);
    len = VSPRINTF_FUNC((char*)cli_write_buf, format, args);
    va_end(args);

    cli_output((char*)cli_write_buf, len);

//	PRINTF_FUNC("%s", write_buffer);
}

bool cli_char_parse(char c)
{
    if(c == '\b') {
        if(cmd_parse_index > 0) {
            cmd_parse_index--;
        }
    }
    else if(c == '\n' || c == '\r') {
        curr_cmd[cmd_parse_index] = '\0';
        cmd_parse_index = 0;
        return true;
    }
    else {
        curr_cmd[cmd_parse_index++] = c;
    }

    if(cmd_parse_index >=  CMD_LEN_MAX) {
        cmd_parse_index = 0;
    }

    return false;
}

static void cli_handle_cmd(char* cmd)
{
    uint8_t len = strlen(cmd);

    if(len==0||(len==1&&!strcmp(cmd, "\n"))||(len==2&&!strcmp(cmd, "\r\n"))) {
        cli_device_write("\r>");
        return;
    }

    uint8_t argc=0;
    char* argv[CLI_ARGV_MAX];
    char* p = cmd;
    uint8_t argv_len=0;

    while(*p != '\0') {
        while(*p != ' ' && *p != '\0') {
            p++;
            argv_len++;
        }

        argv[argc] = MALLOC_FUNC(argv_len+1);
        strncpy(argv[argc], p-argv_len, argv_len);
        argv[argc][argv_len] = '\0';
        argc++;
        argv_len = 0;

        while(*p == ' ' && *p != '\0') {
            p++;
        }
    }

//    for(uint8_t i=0; i<argc;i++)
//    {
//        PRINTF_FUNC("argv%d %s\r\n", i, argv[i]);
//    }

    if((!strcmp(argv[0], "help")) || (!strcmp(argv[0], "?"))) {
        help_shell(argc, argv);
    }
    else {
        bool cmd_find= false;
        for(uint8_t i=0; i<shell_cnt; i++) {
            if(!strcmp(argv[0], shell[i].name)) {
                shell[i].cmd(argc, argv);
                cmd_find = true;
                break;
            }
        }

        if(!cmd_find) {
            cli_device_write("%s: command not found\n\n", argv[0]);
        }
    }

    while(argc--) {
        FREE_FUNC(argv[argc]);
    }

    cli_device_write("\n>");
}

void cli_update(void)
{
    uint16_t len = cli_device_read(cli_read_buf, CLI_READ_BUF_SIZE);

    for(uint16_t i=0; i<len; i++) {
        if(cli_char_parse(cli_read_buf[i])) {
            cli_handle_cmd(curr_cmd);
        }
    }
}

void cli_print_logo(void)
{
    cli_device_write("\n");
    cli_device_write(" .----. .----. .-. .----. .-.   .----.    .-.\n");       
    cli_device_write("/  {}  \\| {}  }| |/  {}  \\| |   | {_     /'v'\\\n");      
    cli_device_write("\\      /| .-. \\| |\\      /| `--.| {__   (/   \\)\n");     
    cli_device_write(" `----' `-' `-'`-' `----' `----'`----' ='=\"=\"===<\n");   
    cli_device_write("                                          |_|\n"); 
}

void cli_init(void)
{
    cli_device_init();
    cli_regist("help", help_shell);
    cli_regist("reboot", reboot_shell);

    cli_print_logo();
}
