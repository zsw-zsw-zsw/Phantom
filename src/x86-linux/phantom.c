#include "phantom.h"
#ifdef PHANTOM_DEBUG
#define PHANTOM_SAMPLES 10
#else
#define PHANTOM_SAMPLES 4096
#endif

 phfunc al_into_phfunc(assemblyline_t al) {
    phfunc ptr = (phfunc) asm_get_code(al);
    asm_destroy_instance(al);
    return ptr;
}

uint64_t generate_JMP(assemblyline_t al, uint32_t jmp_num, uint32_t jmp_distance) {
    char inst[32];
    sprintf(inst, "JMP 0x%x", jmp_distance);
    for (int i = 0; i < jmp_num; i++) {
        asm_assemble_str(al, inst);
        for (int i = 0; i < jmp_distance; i++)
            asm_assemble_str(al, "NOP");
    }
    return jmp_num * (jmp_distance + 5);
}

uint64_t create_alias(uint64_t addr) {
    return addr ^ (1ull << 33) ^ (1ull << 21);
}

uint64_t IF_cmp() {
    uint64_t sum = 0;
    #ifdef PHANTOM_DEBUG
    printf("Sampled time: ");
    #endif
    for (int i = 0; i < PHANTOM_SAMPLES; i++) {
        uint64_t test_addr = random_addr();
        assemblyline_t test_al = create_al(test_addr, PAGE_SIZE << 2);
        phfunc test_func = al_into_phfunc(test_al);
        uint64_t monitor_addr = test_addr + PAGE_SIZE + rand() % PAGE_SIZE;
        memory_barrier
        clflush((void*)monitor_addr);
        memory_barrier
        delayloop(100000);
        memory_barrier
        uint64_t tim = memaccesstime((void*)monitor_addr);
        #ifdef PHANTOM_DEBUG
        printf("%d ", tim);
        #endif
        sum += tim;
        free_buffer(test_func, PAGE_SIZE << 3);
    }
    #ifdef PHANTOM_DEBUG
        printf("\n");
    #endif
    sum /= PHANTOM_SAMPLES;
    return sum;
}

uint64_t IF_channel(phfunc train_func, phfunc victim_func, uint64_t monitor_addr) {
    uint64_t sum = 0;
    #ifdef PHANTOM_DEBUG
    // printf("Train address [%p]\nVictim address [%p]\nMonitor address [%p]\n", train_func, victim_func, monitor_addr);
    printf("Sampled time: ");
    #endif
    for (int i = 0; i < PHANTOM_SAMPLES; i++) {
        memory_barrier
        for (int j = 0; j < 8; j++)
            train_func();
        memory_barrier
        clflush((void*)monitor_addr);
        memory_barrier
        victim_func();
        delayloop(100000);
        memory_barrier
        uint64_t tim = memaccesstime((void*)monitor_addr);
        #ifdef PHANTOM_DEBUG
        printf("%d ", tim);
        #endif
        sum += tim;
    }
    #ifdef PHANTOM_DEBUG
        printf("\n");
    #endif
    return sum / PHANTOM_SAMPLES;
}

uint64_t ID_cmp(phfunc train_func, phfunc evict_func, phfunc victim_func) {
    uint32_t fd_pfm;
    uint64_t cnt_pfm = 0, cnt_tmp = 0;
    uint64_t sum_pfm = 0;
    #ifdef PHANTOM_DEBUG
    printf("Sampled time: ");
    #endif
    BEGIN
    for (int j = 0; j < PHANTOM_SAMPLES; j++) {
        memory_barrier
        SET_BREAK
        victim_func();
        memory_barrier
        GET_SAMPLE
        sum_pfm += cnt_pfm;
        #ifdef PHANTOM_DEBUG
        printf("%d ", cnt_pfm);
        #endif
    }
    END
    #ifdef PHANTOM_DEBUG
        printf("\n");
    #endif
    return sum_pfm / PHANTOM_SAMPLES;
}
uint64_t ID_channel(phfunc train_func, phfunc evict_func, phfunc victim_func) {
    uint32_t fd_pfm;
    uint64_t cnt_pfm = 0, cnt_tmp = 0;
    uint64_t sum_pfm = 0;
    BEGIN
    #ifdef PHANTOM_DEBUG
    printf("Sampled time: ");
    #endif
    for (int j = 0; j < PHANTOM_SAMPLES; j++) {
        for (int i = 0; i < 32; i++)
            train_func();
        memory_barrier
        SET_BREAK
        victim_func();
        memory_barrier
        GET_SAMPLE
        sum_pfm += cnt_pfm;
        #ifdef PHANTOM_DEBUG
        printf("%d ", cnt_pfm);
        #endif
    }
    END
    #ifdef PHANTOM_DEBUG
        printf("\n");
    #endif
    return sum_pfm / PHANTOM_SAMPLES;
}

uint64_t EX_channel(phfunc train_func, phfunc victim_func, uint64_t monitor_addr) {
    uint32_t cnt = 0;
    uint64_t sum = 0;
    #ifdef PHANTOM_DEBUG
    printf("Sampled time: ");
    #endif
    for (int i = 0; i < PHANTOM_SAMPLES; i++) {
        memory_barrier
        for (int j = 0; j < 128; j++)
            train_func();
        memory_barrier
        clflush((void*)monitor_addr);
        memory_barrier
        victim_func();
        delayloop(100000);
        memory_barrier
        uint64_t tim = memaccesstime((void*)monitor_addr);
        #ifdef PHANTOM_DEBUG
        printf("%d ", tim);
        #endif
        sum += tim;
        // cnt += tim < IF_THRESHOLD;
    }
    #ifdef PHANTOM_DEBUG
        printf("\n");
    #endif
    return sum / PHANTOM_SAMPLES;
    // return cnt > SAMPLES / 2;

}

