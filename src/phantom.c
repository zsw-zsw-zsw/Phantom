 #include "phantom.h"


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

void start_op_cache_counter(uint32_t* fd_access, uint32_t* fd_hit, uint32_t* fd_miss) {
    *fd_access = setup_perf_event(PERF_TYPE_RAW, get_raw_config(OP_CACHE_HIT_MISS_EVENT, OP_CACHE_ACCESS_UMASK), -1);
    *fd_hit = setup_perf_event(PERF_TYPE_RAW, get_raw_config(OP_CACHE_HIT_MISS_EVENT, OP_CACHE_HIT_UMASK), -1);
    *fd_miss = setup_perf_event(PERF_TYPE_RAW, get_raw_config(OP_CACHE_HIT_MISS_EVENT, OP_CACHE_MISS_UMASK), -1);
    ioctl(*fd_access, PERF_EVENT_IOC_RESET, 0);
    ioctl(*fd_hit, PERF_EVENT_IOC_RESET, 0);
    ioctl(*fd_miss, PERF_EVENT_IOC_RESET, 0);
    ioctl(*fd_access, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(*fd_hit, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(*fd_miss, PERF_EVENT_IOC_ENABLE, 0);
}

// uint32_t stop_and_read_counter(uint32_t fd, uint64_t* cnt) {
//     ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
//     read(fd, cnt, sizeof(uint64_t));
//     close(fd);
// }
uint32_t stop_and_read_counter(uint32_t fd_access, uint32_t fd_hit, uint32_t fd_miss, uint64_t* cnt_access, uint64_t* cnt_hit, uint64_t* cnt_miss) {
    ioctl(fd_access, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_hit, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_miss, PERF_EVENT_IOC_DISABLE, 0);
    read(fd_access, cnt_access, sizeof(uint64_t));
    read(fd_hit, cnt_hit, sizeof(uint64_t));
    read(fd_miss, cnt_miss, sizeof(uint64_t));
    close(fd_access);
    close(fd_hit);
    close(fd_miss);
}


bool IF_Channel(phfunc train_func, phfunc victim_func, uint64_t monitor_addr) {
    uint32_t cnt = 0;
    uint64_t sum = 0;
    for (int i = 0; i < SAMPLES; i++) {
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
        printf("%d ", tim);
        sum += tim;
        cnt += tim < IF_THRESHOLD;
    }
    printf("%llu\n", sum / SAMPLES);
    return cnt > SAMPLES / 2;
}

// #define BEGIN \
//             fd_miss = setup_perf_event(PERF_TYPE_RAW, get_raw_config(OP_CACHE_HIT_MISS_EVENT, OP_CACHE_MISS_UMASK), -1);\
//             ioctl(fd_miss, PERF_EVENT_IOC_RESET, 0);\
//             ioctl(fd_miss, PERF_EVENT_IOC_ENABLE, 0);
// #define END \
//             ioctl(fd_miss, PERF_EVENT_IOC_DISABLE, 0);\
//             read(fd_miss, &cnt_miss, sizeof(uint64_t));\
//             close(fd_miss);



bool ID_Channel(phfunc train_func, phfunc evict_func, phfunc victim_func) {
    uint32_t fd_access, fd_hit ,fd_miss;
    uint64_t cnt_access = 0, cnt_hit = 0, cnt_miss = 0, cnt_tmp = 0;
    uint64_t sum_access, sum_hit, sum_miss;
    BEGIN
    // printf("[%d %d %d] ", cnt_access, cnt_hit, cnt_miss);
    uint64_t cnt0 = 0, cnt1 = 0;
    for (int j = 0; j < 800; j++) {
        for (int i = 0; i < 32; i++)
            train_func();
        // for (int i = 0; i < 16; i++)
        //     evict_func();
        memory_barrier
        SET_BREAK
        victim_func();
        memory_barrier
        GET_SAMPLE
        cnt0 += cnt_miss;
    }
    for (int i = 0; i < 64; i++)
        evict_func();
    for (int j = 0; j < 800; j++) {
        for (int i = 0; i < 32; i++)
            train_func();
        for (int i = 0; i < 160; i++)
            evict_func();
        memory_barrier
        SET_BREAK
        victim_func();
        memory_barrier
        GET_SAMPLE
        cnt1 += cnt_miss;
        // if (j == 9)
        //     printf("%d ", cnt_miss);
    }
    printf("%llu %llu", cnt0 / 800, cnt1 / 800);
    printf("\n");
    END
    return true;
}

bool EX_Channel(phfunc train_func, phfunc victim_func, uint64_t monitor_addr) {
    uint32_t cnt = 0;
    for (int i = 0; i < SAMPLES; i++) {
        memory_barrier
        train_func();
        memory_barrier
        clflush((void*)monitor_addr);
        memory_barrier
        victim_func();
        delayloop(100000);
        memory_barrier
        uint64_t tim = memaccesstime((void*)monitor_addr);
        cnt += tim < IF_THRESHOLD;
    }
    return cnt > (SAMPLES - 1) / 2; 

}

