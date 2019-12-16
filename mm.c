/**                                               _____           ,-.
 * _______       _____       _____                ___   _,.      /  /
 * ___    |__   ____(_)_____ __  /______________  __   ; \____,-==-._  )
 * __  /| |_ | / /_  /_  __ `/  __/  __ \_  ___/  _    //_    `----' {+>
 * _  ___ |_ |/ /_  / / /_/ // /_ / /_/ /  /      _    `  `'--/  /-'`(
 * /_/  |_|____/ /_/  \__,_/ \__/ \____//_/       _          /  /
 *                                                           `='
 *
 * mm.c
 *
 * v1.0
 *
 * Dynamic memory management, reducing memory fragmentation
 */
#include "support.h"
#include "mm.h"

#if USE_MM_SHELL
/*
If use print function, must define printf function
like. 
#include "debug.h"
#define PRINTF_FUNC             PRINT
*/
#include "cli.h"
#include "debug.h"

#define PRINTF_FUNC             PRINT
#endif

struct block_s {
    struct block_s* next;
    size_t size;
};

#define MM_BYTE_ALIGNMENT       4
#define BYTE_ALIGNMENT_MASK    (MM_BYTE_ALIGNMENT-1)
#define BLOCK_STRUCT_SIZE	   ((sizeof(struct block_s)+((size_t)(MM_BYTE_ALIGNMENT-1)))&~((size_t)BYTE_ALIGNMENT_MASK))
#define BLOCK_SIZE_MIN	       ((size_t)(BLOCK_STRUCT_SIZE << 1))

uint8_t mm_heap[MM_HEAP_SIZE] __attribute__((aligned(4)));

#if USE_MM == MM_MODULE_STATIC
    size_t mm_used = 0;

#elif USE_MM == MM_MODULE_DYNAMIC_ADDR
    struct block_s mm_start;
    struct block_s* mm_end = NULL;
    size_t mm_remaining = 0U;
#if USE_MM_STAT
    size_t mm_used_max = 0U;
#endif
#endif




void* mm_malloc(uint32_t s)
{
    void* m = NULL;
    if(s == 0) return m;
#if USE_MM == MM_MODULE_STATIC
    if(s & BYTE_ALIGNMENT_MASK) {
        s += (MM_BYTE_ALIGNMENT - (s & BYTE_ALIGNMENT_MASK));
    }

    if(((mm_used+s) < MM_HEAP_SIZE)) {
        m = heap + mm_used;
        mm_used += s;
    }
#elif USE_MM == MM_MODULE_DYNAMIC_ADDR
    struct block_s* curr, *prev, *new;

    s += BLOCK_STRUCT_SIZE;
    if((s & BYTE_ALIGNMENT_MASK) != 0x00) {
        s += (MM_BYTE_ALIGNMENT - (s & BYTE_ALIGNMENT_MASK));
    }

    if((s > 0) && (s <= mm_remaining)) {
        prev = &mm_start;
        curr = mm_start.next;

        while((curr->size < s) && (curr->next != NULL)) {
            prev = curr;
            curr = curr->next;
        }

        if(curr != mm_end) {
            m = (void*)(((uint8_t*)prev->next)+BLOCK_STRUCT_SIZE);
            prev->next=curr->next;
            curr->next = NULL;

            if((curr->size-s) > BLOCK_SIZE_MIN) {
                new = (void*)(((uint8_t*)curr)+s);
                new->size = curr->size - s;

                curr->size = s;

                // PRINT("malloc: addr:%p next:%p size:%d\n", curr, curr->next, curr->size);

                new->next=prev->next;
                prev->next=new;
            }
            mm_remaining -= curr->size;
            
        #if USE_MM_STAT
            size_t used = MM_HEAP_SIZE - mm_remaining;
            if(used > mm_used_max) {
                mm_used_max = used;
            }
        #endif    
        }
    }
#elif MM_USE == MM_MODULE_DYNAMIC_SIZE
#else
#error "must select one mm module"
#endif

    return m;
}

void mm_free(void* m)
{
#if USE_MM == MM_MODULE_STATIC
#elif USE_MM == MM_MODULE_DYNAMIC_ADDR
    struct block_s* block;
    struct block_s* iterator;

    if(m != NULL) {
        block = (void*)((uint8_t*)m-BLOCK_STRUCT_SIZE);

        // PRINT("free: addr:%p next:%p size:%d\n", block, block->next, block->size);

        if(block->next == NULL) {
            mm_remaining += block->size;

            for(iterator=&mm_start; iterator->next<block; iterator=iterator->next);

            if(((uint8_t*)iterator + iterator->size) == (uint8_t*)block) {
                iterator->size += block->size;
                block = iterator;
            }
            else {
                block->next=iterator->next;
                iterator->next=block;
            }

            if(((uint8_t*)block + block->size) == (uint8_t*)(block->next)) {
                if((uint8_t*)block->next != (uint8_t*)mm_end) {
                    block->size += block->next->size;
                    block->next = block->next->next;
                    block->next->next = NULL;
                }
            }
        }
    }
#elif MM_USE == MM_MODULE_DYNAMIC_SIZE
#else
#error "must select one mm module"
#endif
}

#if USE_MM_SHELL
void mm_print_info(void)
{
    struct block_s* block;
    uint8_t i;
    PRINTF_FUNC("---------mm info----------\n");
    for(i=0, block=mm_start.next; block->next!=NULL; block=block->next, i++) {
        PRINTF_FUNC("b%d: addr:%p next:%p size:%d\n", i, block, block->next, block->size);
    }
    PRINTF_FUNC("--------------------------\n");
}

void free_shell(int argc, char *argv[])
{
	if(argc == 1) {
        cli_device_write("total memory: %3.3fKB(%dByte)\n", MM_HEAP_SIZE/1024.0f, MM_HEAP_SIZE);
        cli_device_write("used memory : %3.3fKB(%dByte)\n", (MM_HEAP_SIZE-mm_remaining)/1024.0f, MM_HEAP_SIZE-mm_remaining);
        cli_device_write("free memory : %3.3fKB(%dByte)\n", mm_remaining/1024.0f, mm_remaining);
    #if USE_MM_STAT    
        cli_device_write("maximum allocated memory: %3.3fKB(%dByte)\n", mm_used_max/1024.0f, mm_used_max);
    #endif
        return;
	}

	cli_device_write("missing command: no need argv");    
}
#endif

void mm_shell(int argc, char* argv[]);

void mm_init(void)
{
    size_t addr;
    struct block_s* first;

    mm_start.next = (void*)mm_heap;
    mm_start.size = (size_t)0;

    addr = ((size_t)mm_heap) + MM_HEAP_SIZE;
    addr -= BLOCK_STRUCT_SIZE;
    addr &= ~((size_t)BYTE_ALIGNMENT_MASK);
    mm_end = (void*) addr;
    mm_end->size = 0;
    mm_end->next = NULL;

    first = (void*)mm_heap;
    first->size = addr - (size_t)first;
    first->next = mm_end;

    mm_remaining = first->size;

#if USE_MM_SHELL    
    cli_regist("free", free_shell);
    cli_regist("mm", mm_shell);
#endif    
}

#if USE_MM_SHELL
void mm_shell(int argc, char* argv[])
{
    if(argc == 2) {
        if(strcmp(argv[1], "list") == 0) {
            mm_print_info();
            return;
        }
    }

    cli_device_write("missing command: try 'list'");
}
#endif

