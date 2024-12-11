#ifndef PHANTOM_H
#define PHANTOM_H

#include <stdlib.h>
#include <assemblyline.h>
#include <mastik/low.h>
#include <mastik/util.h>
#include "utils.h"
#include "perf_event_wrapper.h"
typedef void (*phfunc)();
#define memory_barrier asm volatile("sfence;\nmfence;\nlfence");

// #define PHANTOM_DEBUG
uint64_t create_alias(uint64_t addr);
uint64_t IF_cmp();
uint64_t IF_channel(phfunc train_func, phfunc victim_func, uint64_t monitor_addr);
uint64_t ID_channel(phfunc train_func, phfunc evict_func, phfunc victim_func);
uint64_t EX_channel(phfunc train_func, phfunc victim_func, uint64_t monitor_addr);
phfunc al_into_phfunc(assemblyline_t al);
uint64_t generate_JMP(assemblyline_t al, uint32_t jmp_num, uint32_t jmp_distance);

/*
    Zen3 pfm counter
*/
#define OP_CACHE_HIT_MISS_EVENT 0x28f
#define OP_CACHE_HIT_UMASK 0x3
#define OP_CACHE_MISS_UMASK 0x4
#define OP_CACHE_ACCESS_UMASK 0x7

/*
    Zen2 pfm counter
*/
#define DE_DIS_UOPS_FROM_DECODER 0xaa
#define DE_DIS_UOPS_FROM_BOTH_UMASK 0xff
#define DE_DIS_UOPS_FROM_DECODER_UMASK 0x1
#define DE_DIS_UOPS_FROM_OPCACHE_UMASK 0x2
// #define BEGIN \
//             fd_miss = setup_perf_event(PERF_TYPE_RAW, get_raw_config(OP_CACHE_HIT_MISS_EVENT, OP_CACHE_ACCESS_UMASK), -1);\
//             ioctl(fd_miss, PERF_EVENT_IOC_RESET, 0);\
//             ioctl(fd_miss, PERF_EVENT_IOC_ENABLE, 0);
#define BEGIN \
            fd_pfm = setup_perf_event(PERF_TYPE_RAW, get_raw_config(0xaa, 0xff), -1);\
            ioctl(fd_pfm, PERF_EVENT_IOC_RESET, 0);\
            ioctl(fd_pfm, PERF_EVENT_IOC_ENABLE, 0);
#define END \
            ioctl(fd_pfm, PERF_EVENT_IOC_DISABLE, 0);\
            close(fd_pfm);

#define SET_BREAK \ 
    read(fd_pfm, &cnt_tmp, sizeof(uint64_t));
#define GET_SAMPLE \
    read(fd_pfm, &cnt_pfm, sizeof(uint64_t));\
    cnt_pfm -= cnt_tmp;
#endif

