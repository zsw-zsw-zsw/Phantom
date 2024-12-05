
#include "hpmdriver.h"
uint64_t get_cycle_count() {
    uint32_t low, high;
    asm volatile("csrr %0, cycle" : "=r"(low));
    asm volatile("csrr %0, cycleh" : "=r"(high));
    return ((uint64_t)high << 32) | low;
}

uint64_t M_get_cycle_count() {
    uint32_t result;
    asm volatile("csrr %0, mcycle" : "=r"(result));
    return result;
}

void set_hpc(uint32_t csr_id, uint32_t event_code) {
    se_cc_single(csr_id, MODE_M, event_code);
}
uint64_t get_hpc(uint32_t csr_id) {
    return get_counter(csr_id);
}
uint64_t test_block() {
    uint64_t sum = 1;
    for (int i = 0; i < 10; i += 1)
        sum = sum * i;
    return sum;
}

int main() {
    uint64_t cnt_st = get_cycle_count();
    test_block();
    uint64_t cnt_ed = get_cycle_count();
    printf("%llu\n", cnt_ed - cnt_st);

    cnt_st = M_get_cycle_count();
    test_block();
    cnt_ed = M_get_cycle_count();
    printf("%llu\n", cnt_ed - cnt_st);

    set_hpc(3, Frontend_icache_miss_cnt);
    cnt_st = get_hpc(3);
    test_block();
    cnt_ed = get_hpc(3);
    printf("%llu\n", cnt_ed - cnt_st);
    return 0;
    
}