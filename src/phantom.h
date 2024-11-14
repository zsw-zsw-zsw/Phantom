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
#define SAMPLES 5
#define IF_THRESHOLD 240
bool IF_Channel(phfunc train_func, phfunc victim_func, uint64_t monitor_addr);
bool ID_Channel(phfunc train_func, phfunc evict_func, phfunc victim_func);
bool EX_Channel(phfunc train_func, phfunc victim_func, uint64_t monitor_addr);
phfunc al_into_phfunc(assemblyline_t al);
uint64_t generate_JMP(assemblyline_t al, uint32_t jmp_num, uint32_t jmp_distance);

#define BEGIN \
            fd_miss = setup_perf_event(PERF_TYPE_RAW, get_raw_config(OP_CACHE_HIT_MISS_EVENT, OP_CACHE_ACCESS_UMASK), -1);\
            ioctl(fd_miss, PERF_EVENT_IOC_RESET, 0);\
            ioctl(fd_miss, PERF_EVENT_IOC_ENABLE, 0);
#define END \
            ioctl(fd_miss, PERF_EVENT_IOC_DISABLE, 0);\
            close(fd_miss);

#define SET_BREAK \ 
    read(fd_miss, &cnt_tmp, sizeof(uint64_t));
#define GET_SAMPLE \
    read(fd_miss, &cnt_miss, sizeof(uint64_t));\
    cnt_miss -= cnt_tmp;
#endif

