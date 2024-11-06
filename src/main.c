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
    for (int i = 0; i < SAMPLES; i++) {
        uint64_t train_addr = random_address();
        uint64_t victim_addr = train_addr ^ (1ull << 33) ^ (1ull << 21);
        uint64_t dest_addr = random_address() & ((1 << 31) - 1);
        uint64_t evict_addr = dest_addr ^ (1ull << 18);
        assemblyline_t train_al = create_al(train_addr, PAGE_SIZE << 5);
        assemblyline_t victim_al = create_al(victim_addr, PAGE_SIZE << 5);
        assemblyline_t dest_al = create_al(dest_addr, PAGE_SIZE << 5);
        assemblyline_t evict_al = create_al(evict_addr, PAGE_SIZE << 5);
        generate_JMP(train_al, 2, 5);
        generate_JMP(victim_al, 2, 5);
        generate_JMP(dest_al, 7, 4091);
        generate_JMP(evict_al, 7, 4091);
        char inst[64] = {0};
        sprintf(inst, "MOV eax, 0x%llx", dest_addr);
        // printf("%s\n", inst);
        asm_assemble_str(train_al, inst);
        asm_assemble_str(train_al, "JMP eax");
        // asm_assemble_str(train_al, "RET");
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
void test_EX() {
}
int main() {
    test_ID();
    return 0;
}