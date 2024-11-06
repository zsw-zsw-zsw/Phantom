#ifndef PHANTOM_H
#define PHANTOM_H

#include <stdlib.h>
#include <assemblyline.h>
#include <mastik/low.h>
#include <mastik/util.h>
#include "utils.h"
typedef void (*phfunc)();
#define memory_barrier asm volatile("sfence;\nmfence;\nlfence");
#define SAMPLES 5
#define IF_THRESHOLD 240
bool IF_Channel(phfunc train_func, phfunc victim_func, uint64_t monitor_addr);
bool ID_Channel(phfunc train_func, phfunc evict_func, phfunc victim_func);
bool EX_Channel(phfunc train_func, phfunc victim_func, uint64_t monitor_addr);
phfunc al_into_phfunc(assemblyline_t al);
uint32_t generate_JMP(assemblyline_t al, uint32_t jmp_num, uint32_t jmp_distance);
#endif

