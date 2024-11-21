#include "utils.h"

uint64_t random_addr() {
    return ((uint64_t)rand() << 16) | rand();
}

void* create_buffer(uint64_t addr, uint64_t size) {
    uint64_t pg_addr = addr & ((uint64_t)(-1) ^ (0xfff));
    uint64_t pg_offset = addr & (0xfff);
    uint64_t page_size = (size + PAGE_SIZE - 1) / PAGE_SIZE * PAGE_SIZE;
    // printf("%llu\n", pg_addr);
    void* pg_ptr = (void*) mmap((void*)(pg_addr), page_size,
                        PROT_READ | PROT_WRITE | PROT_EXEC,
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE | MAP_POPULATE,
                        -1, 0);
    
    if (pg_ptr == MAP_FAILED) {
        fprintf(stderr, "Cannot allocate memory!");
        exit(0);
    }
    // printf("%llu\n", pg_ptr);
    return pg_ptr + pg_offset;
}

void free_buffer(uint64_t addr, uint64_t size) {
    uint64_t pg_addr = addr & ((uint64_t)(-1) ^ (0xfff));
    uint64_t page_size = (size + PAGE_SIZE - 1) / PAGE_SIZE * PAGE_SIZE;
    munmap(pg_addr, page_size);
}

assemblyline_t create_al(uint64_t addr, uint64_t size) {
    void *buf = create_buffer(addr, size);
    return asm_create_instance(buf, size);
}

void free_al(assemblyline_t al) {
    asm_destroy_instance(al);
}