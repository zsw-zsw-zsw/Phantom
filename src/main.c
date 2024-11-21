#include "utils.h"
#include "phantom.h"


void test_IF() {
    #define IF_SAMPLES 5
    #define IF_THRESHOLD 240
    srand(time(0));
    uint32_t cnt = 0;
    printf("IF Channel:\n");
    printf("Uncached access time: %d\n", IF_cmp());
    for (int i = 0; i < IF_SAMPLES; i++) {
        uint64_t jmp_distance = (rand() % PAGE_SIZE + 128) + 128;
        uint64_t train_addr = random_addr();
        uint64_t victim_addr = create_alias(train_addr);
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
        uint64_t access_time = IF_channel(train_func, victim_func, victim_addr + train_length + 5 + jmp_distance);
        cnt += access_time <= IF_THRESHOLD;
        printf("[testcase %d]: %llu \n",i, access_time);
        free_buffer(train_func, PAGE_SIZE << 3);
        free_buffer(victim_func, PAGE_SIZE << 3);
    } 
    if (cnt > IF_SAMPLES / 2)
        printf("IF Detected\n\n");
    else 
        printf("IF Not Detected\n\n");
    
}
void test_ID() {
    #define ID_SAMPLES 100
    srand(1919810);
    printf("ID Channel:\n");
    uint64_t sum1 = 0, sum2 = 0;
    for (int i = 0; i < ID_SAMPLES * 2; i++) {
                                       
        uint64_t train_addr = random_addr();
        
        uint64_t victim_addr = create_alias(train_addr);
        uint64_t dest_addr = random_addr() & ((1ull << 30) - 1) ;
        uint64_t evict_addr = dest_addr ^ (1ull << 26);

        assemblyline_t train_al = create_al(train_addr, PAGE_SIZE << 4);
        assemblyline_t victim_al = create_al(victim_addr, PAGE_SIZE << 4);
        assemblyline_t dest_al = create_al(dest_addr, PAGE_SIZE << 4);
        assemblyline_t evict_al = create_al(evict_addr, PAGE_SIZE << 4);

        generate_JMP(train_al, 2, 5);
        uint64_t pos = generate_JMP(victim_al, 2, 5) + victim_addr;

        generate_JMP(dest_al, 7, 4091);
        generate_JMP(evict_al, 7, 4091);
        char inst[64] = {0};
        sprintf(inst, "MOV eax, 0x%llx", dest_addr);
        asm_assemble_str(train_al, inst);
        asm_assemble_str(train_al, "JMP eax");
        // asm_assemble_str(train_al, "RET");
        // sprintf(inst, "MOV eax, 0x%llx", pos + 10 + 2);
        // asm_assemble_str(victim_al, inst);
        // asm_assemble_str(victim_al, "JMP eax");
        asm_assemble_str(victim_al, "JMP 0");
        asm_assemble_str(victim_al, "RET");
        asm_assemble_str(dest_al, "RET");
        asm_assemble_str(evict_al, "RET");
        phfunc train_func = al_into_phfunc(train_al);
        phfunc victim_func = al_into_phfunc(victim_al);
        phfunc dest_func = al_into_phfunc(dest_al);
        phfunc evict_func = al_into_phfunc(evict_al);

        // IF_channel(train_func, victim_func, dest_addr);
        // uint64_t cnt_pfm1 = ID_cmp(train_func, evict_func, victim_func);
        // uint64_t cnt_pfm2 = ID_channel(train_func, evict_func, victim_func);
        // printf("[testcase %d]: %llu %llu\n",i, cnt_pfm1, cnt_pfm2);
        if (!(i & 1)) {
            uint64_t cnt_pfm = ID_cmp(train_func, evict_func, victim_func);
            // printf("[testcase %d]: %llu\n",i, cnt_pfm);
            sum1 += cnt_pfm;
        }
        else {
            uint64_t cnt_pfm = ID_channel(train_func, evict_func, victim_func);
            // printf("[testcase %d]: %llu\n",i, cnt_pfm);
            sum2 += cnt_pfm;
        }
        free_buffer(train_func, PAGE_SIZE << 4);
        free_buffer(victim_func, PAGE_SIZE << 4);
        free_buffer(dest_func, PAGE_SIZE << 4);
        free_buffer(evict_func, PAGE_SIZE << 4);
    }
    printf("%llu %llu\n\n", sum1 / ID_SAMPLES, sum2 / ID_SAMPLES);
}

int array[1024];

void test_EX() {
    #define EX_SAMPLES 5
    #define EX_THRESHOLD 240
    srand(1919810);
    printf("EX Channel:\n");
    uint32_t cnt = 0;
    for (int i = 0; i < EX_SAMPLES; i++) {
        int memory_addr = random_addr() & ((1 << 29) - 1) & ((uint64_t)(-1) ^ (0xfff));
        void* pg_ptr = (void*) mmap((void*)memory_addr, 128,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE | MAP_POPULATE,
                        -1, 0);
        if (pg_ptr == MAP_FAILED) {
            fprintf(stderr, "Cannot allocate memory!");
            exit(0);
        }
        uint64_t train_addr = random_addr();
        uint64_t dest_addr = random_addr() & ((1 << 31) - 1);
        uint64_t victim_addr = create_alias(train_addr);
        assemblyline_t train_al = create_al(train_addr, PAGE_SIZE << 3);
        assemblyline_t dest_al = create_al(dest_addr, PAGE_SIZE << 3);
        assemblyline_t victim_al = create_al(victim_addr, PAGE_SIZE << 3);
        char inst[128];
        sprintf(inst, "MOV eax, 0x%llx", dest_addr);
        asm_assemble_str(train_al, inst);
        asm_assemble_str(victim_al, inst);
        generate_JMP(train_al, 8, 128);
        generate_JMP(victim_al, 8, 128);

        asm_assemble_str(train_al, "JMP eax");

        sprintf(inst, "mov eax, [%p]", memory_addr);
        // printf("%p\n%s\n", pg_ptr, inst);
        // asm_assemble_str(train_al, "PUSH eax");
        asm_assemble_str(dest_al, inst);
        // asm_assemble_str(train_al, "POP eax");
        asm_assemble_str(train_al, "RET");
        asm_assemble_str(victim_al, "NOP");
        asm_assemble_str(victim_al, "RET");
        asm_assemble_str(dest_al, "RET");
        phfunc train_func = al_into_phfunc(train_al);
        phfunc victim_func = al_into_phfunc(victim_al);
        phfunc dest_func = al_into_phfunc(dest_al);
        uint64_t tim = EX_channel(train_func, victim_func, (uint64_t)(memory_addr));
        printf("[testcase %d]: %llu \n",i, tim);
        cnt += tim <= EX_THRESHOLD;
        munmap(pg_ptr, 128);
        free_buffer(train_func, PAGE_SIZE << 3);
        free_buffer(victim_func, PAGE_SIZE << 3);
        free_buffer(dest_func, PAGE_SIZE << 3);
    } 
    if (cnt > EX_SAMPLES / 2)
        printf("EX Detected\n\n");
    else 
        printf("EX Not Detected\n\n");
}

void test_OP_cache_size() {
    srand(time(0));
    freopen("testdata.out", "w", stdout);

    uint32_t fd_pfm;
    uint64_t cnt_pfm;
    uint64_t cnt_tmp;
    BEGIN
    for (int pos = 1; pos < 15; pos++) {
        printf("[%d]:", pos);
        uint32_t sum = 0;
        uint32_t sum1 = 0;
        for (int j = 0; j < 100; j++) {

            uint32_t main_addr = random_addr();
            assemblyline_t main_al = create_al(main_addr, PAGE_SIZE << 4);
            for (int i = 0; i < pos; i++)
                asm_assemble_str(main_al, "NOP");
            asm_assemble_str(main_al, "RET");
            phfunc main_func = al_into_phfunc(main_al);
            memory_barrier
            SET_BREAK
            // test_sum();
            GET_SAMPLE
            sum1 += cnt_pfm;
            free_buffer(main_addr, PAGE_SIZE << 4);
        }
        printf(" %u %u", sum / 100, sum1 / 100);
        printf("\n");
    }
    END
    fclose(stdout);
}

void test_OP_cache() {
    srand(time(0));
    freopen("testdata.out", "w", stdout);


    uint32_t fd_pfm;
    uint64_t cnt_pfm;
    uint64_t cnt_tmp;
    BEGIN
    for (int pos = 18; pos < 29; pos++) {
        printf("[%d]:\n", pos);
        for (int i = 0; i < 1; i++)
            for (int j = 0; j < 10; j++) {
                // printf("[%d, %d]: ", i, j);

                uint64_t main_addr = random_addr();
                uint64_t evict_addr = main_addr ^ (1 << pos);
                // uint64_t evict_addr = random_addr();
                assemblyline_t main_al = create_al(main_addr, PAGE_SIZE << 5);
                assemblyline_t evict_al = create_al(evict_addr, PAGE_SIZE << 5);
                generate_JMP(main_al, 15, 4091);
                generate_JMP(evict_al, 15, 4091);
                asm_assemble_str(main_al, "RET");
                asm_assemble_str(evict_al, "RET");
                phfunc main_func = al_into_phfunc(main_al);
                phfunc evict_func = al_into_phfunc(evict_al);
                printf("(%llx) ", main_addr);
                // for (int k = 0; k < 32; k++) {
                //     main_func();
                // }
                memory_barrier
                SET_BREAK
                main_func();
                GET_SAMPLE
                memory_barrier
                printf("%d ", cnt_pfm);
                for (int k = 0; k < 16; k++)
                    main_func();
                memory_barrier
                SET_BREAK
                main_func();
                GET_SAMPLE
                memory_barrier
                printf("%d ", cnt_pfm);
                for (int k = 0; k < 160; k++)
                    evict_func();
                memory_barrier
                SET_BREAK
                main_func();
                GET_SAMPLE
                printf("%d ", cnt_pfm);
                // for (int k = 0; k < i; k++)
                //     evict_func();
                memory_barrier
                SET_BREAK
                main_func();
                GET_SAMPLE
                printf("%d ", cnt_pfm);
                memory_barrier
                printf("\n");
            }
    }
    END
    fclose(stdout);

}


int main() {
    test_IF();
    test_ID();
    test_EX();
    return 0;
}