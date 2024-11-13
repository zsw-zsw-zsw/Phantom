#include "utils.h"
#include "phantom.h"
void test_IF() {
    srand(time(0));
    // for (int step = -3; step < 3; step ++) {
    //     printf("step: %d\n", step);
    for (int i = 0; i < SAMPLES; i++) {
        uint64_t jmp_distance = (rand() % PAGE_SIZE + 128);
        uint64_t train_addr = random_address();
        uint64_t victim_addr = train_addr ^ (1ull << 33) ^ (1ull << 21);
        assemblyline_t train_al = create_al(train_addr, PAGE_SIZE << 3);
        assemblyline_t victim_al = create_al(victim_addr, PAGE_SIZE << 3);
        uint32_t train_length = generate_JMP(train_al, 8, 128);
        generate_JMP(victim_al, 8, 128);
        char str_distance[20];
        sprintf(str_distance, "%d", jmp_distance);
        char str_jmp[20] = "JMP ";
        strcat(str_jmp, str_distance);
        asm_assemble_str(train_al, str_jmp);
        for (int i = 0; i < jmp_distance; i++)
            asm_assemble_str(train_al, "NOP"); 
        asm_assemble_str(train_al, "RET");
        asm_assemble_str(victim_al, "RET");
        phfunc train_func = al_into_phfunc(train_al);
        phfunc victim_func = al_into_phfunc(victim_al);
        bool tg = IF_Channel(train_func, victim_func, victim_addr + train_length + 5 + jmp_distance);
        if (tg)
            printf("IF Detected\n");
        else 
            printf("IF Not\n");
    } 
    //     printf("\n"); 
    // }
    
}
void test_ID() {
    srand(time(0));
    for (int i = 0; i < SAMPLES; i++) {
        uint64_t train_addr = random_address();
        uint64_t victim_addr = train_addr ^ (1ull << 33) ^ (1ull << 21);
        uint64_t dest_addr = random_address() & ((1 << 31) - 1);
        uint64_t evict_addr = dest_addr ^ (1ull << 33);
        assemblyline_t train_al = create_al(train_addr, PAGE_SIZE << 6);
        assemblyline_t victim_al = create_al(victim_addr, PAGE_SIZE << 6);
        assemblyline_t dest_al = create_al(dest_addr, PAGE_SIZE << 6);
        assemblyline_t evict_al = create_al(evict_addr, PAGE_SIZE << 6);
        generate_JMP(train_al, 2, 5);
        uint32_t pos = generate_JMP(victim_al, 2, 5) + victim_addr;
        generate_JMP(dest_al, 15, 4091);
        generate_JMP(evict_al, 15, 4091);
        char inst[64] = {0};
        sprintf(inst, "MOV eax, 0x%llx", dest_addr);
        // printf("%s\n", inst);
        asm_assemble_str(train_al, inst);
        asm_assemble_str(train_al, "JMP eax");
        asm_assemble_str(train_al, "RET");
        sprintf(inst, "MOV eax, 0x%llx", pos + 10 + 2);
        asm_assemble_str(victim_al, inst);
        asm_assemble_str(victim_al, "JMP 0");
        asm_assemble_str(victim_al, "RET");
        asm_assemble_str(dest_al, "RET");
        asm_assemble_str(evict_al, "RET");
        phfunc train_func = al_into_phfunc(train_al);
        phfunc victim_func = al_into_phfunc(victim_al);
        al_into_phfunc(dest_al);
        phfunc evict_func = al_into_phfunc(evict_al);
        IF_Channel(train_func, victim_func, dest_addr);
        ID_Channel(train_func, evict_func, victim_func);
        
    }
}

#define BEGIN \
            fd_miss = setup_perf_event(PERF_TYPE_RAW, get_raw_config(OP_CACHE_HIT_MISS_EVENT, OP_CACHE_MISS_UMASK), -1);\
            ioctl(fd_miss, PERF_EVENT_IOC_RESET, 0);\
            ioctl(fd_miss, PERF_EVENT_IOC_ENABLE, 0);
#define END \
            ioctl(fd_miss, PERF_EVENT_IOC_DISABLE, 0);\
            read(fd_miss, &cnt_miss, sizeof(uint64_t));\
            close(fd_miss);

void test_OP_cache() {
    srand(time(0));
    freopen("testdata.out", "w", stdout);


    uint32_t fd_miss;
    uint64_t cnt_miss;
    for (int i = 0; i < 16; i++)
        for (int j = 1; j < 16; j++) {
            printf("[%d, %d]: ", i, j);

            uint64_t main_addr = random_address();
            uint64_t evict_addr = main_addr ^ (1 << 24);
            assemblyline_t main_al = create_al(main_addr, PAGE_SIZE << 6);
            assemblyline_t evict_al = create_al(evict_addr, PAGE_SIZE << 6);
            generate_JMP(main_al, 15, 4091);
            generate_JMP(evict_al, 15, 4091);
            asm_assemble_str(main_al, "RET");
            asm_assemble_str(evict_al, "RET");
            phfunc main_func = al_into_phfunc(main_al);
            phfunc evict_func = al_into_phfunc(evict_al);
            printf("(%llx) ", main_addr);
            for (int k = 0; k < 32; k++)
                evict_func();
                // main_func();
            memory_barrier
            BEGIN
            main_func();
            END
            printf("%d ", cnt_miss);
            // for (int k = 0; k < i; k++)
            //     evict_func();
            memory_barrier
            BEGIN
            main_func();
            END
            printf("%d ", cnt_miss);
            memory_barrier
            BEGIN
            main_func();
            END
            printf("%d ", cnt_miss);
            memory_barrier
            BEGIN
            main_func();
            END
            printf("%d ", cnt_miss);
            // for (int k = 0; k < j; k++) {
            //     memory_barrier
            //     BEGIN
            //     main_func();
            //     END
            //     printf("%d ", cnt_miss);
            // }
            printf("\n");
        }
    fclose(stdout);

}

void test_EX() {
    for (int i = 0; i < SAMPLES; i++) {
        uint64_t jmp_distance = (rand() % PAGE_SIZE + 128);
        uint64_t train_addr = random_address();
        uint64_t victim_addr = train_addr ^ (1ull << 33) ^ (1ull << 21);
        assemblyline_t train_al = create_al(train_addr, PAGE_SIZE << 3);
        assemblyline_t victim_al = create_al(victim_addr, PAGE_SIZE << 3);
        uint32_t train_length = generate_JMP(train_al, 8, 128);
        generate_JMP(victim_al, 8, 128);
        char str_distance[20];
        sprintf(str_distance, "%d", jmp_distance);
        char str_jmp[20] = "JMP ";
        strcat(str_jmp, str_distance);
        asm_assemble_str(train_al, str_jmp);
        for (int i = 0; i < jmp_distance; i++)
            asm_assemble_str(train_al, "NOP"); 
        asm_assemble_str(train_al, "RET");
        asm_assemble_str(victim_al, "RET");
        phfunc train_func = al_into_phfunc(train_al);
        phfunc victim_func = al_into_phfunc(victim_al);
        bool tg = IF_Channel(train_func, victim_func, victim_addr + train_length + 5 + jmp_distance);
        if (tg)
            printf("EX Detected\n");
        else 
            printf("EX Not\n");
    } 
}
int main() {
    test_ID();
    // test_OP_cache();
    return 0;
}