#ifndef UTILS_H
#define UTILS_H

#include <assemblyline.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#define PAGE_SIZE 4096
uint64_t random_address();
void* create_buffer(uint64_t addr, uint64_t size);
void free_buffer(uint64_t addr, uint64_t size);
assemblyline_t create_al(uint64_t addr, uint64_t size);

void free_al(assemblyline_t al);
#endif