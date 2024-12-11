#include "perf_event_wrapper.h"
typedef struct perf_event_attr perf_event_attr;
uint32_t perf_event_open(perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

uint32_t setup_perf_event(uint32_t event_type, uint64_t config, uint32_t group_fd) {
    perf_event_attr pe;
    memset(&pe, 0, sizeof(pe));
    pe.type = event_type;            // Event type
    pe.size = sizeof(perf_event_attr);
    pe.config = config;
    pe.disabled = 1;                 // Initially disabled
    pe.exclude_kernel = 1;           // Exclude kernel events
    pe.exclude_hv = 1;               // Exclude hypervisor events
    // pe.inherit = 1;
    // pe.enable_on_exec = 1;
    // pe.exclude_guest = 1;
    // int pid = getpid();
    // printf("%d\n", pid);
    int fd = perf_event_open(&pe, 0, -1, group_fd, 0);
    if (fd == -1) {
        perror("perf_event_open");
        exit(EXIT_FAILURE);
    }
    return fd;
}

uint64_t get_raw_config(uint64_t event_id, uint64_t umask) {
    return (((event_id >> 8) & ((1 << 4) - 1)) << 32) | (event_id & ((1 << 8) - 1)) | (umask << 8);
}


// #define PERF_EVENT_TEST
#ifdef PERF_EVENT_TEST

uint32_t start_op_cache_counter(uint32_t* fd_access, uint32_t* fd_hit, uint32_t* fd_miss) {
    *fd_access = setup_perf_event(PERF_TYPE_RAW, get_raw_config(OP_CACHE_HIT_MISS_EVENT, OP_CACHE_ACCESS_UMASK), -1);
    *fd_hit = setup_perf_event(PERF_TYPE_RAW, get_raw_config(OP_CACHE_HIT_MISS_EVENT, OP_CACHE_HIT_UMASK), *fd_access);
    *fd_miss = setup_perf_event(PERF_TYPE_RAW, get_raw_config(OP_CACHE_HIT_MISS_EVENT, OP_CACHE_MISS_UMASK), *fd_access);
    ioctl(*fd_access, PERF_EVENT_IOC_RESET, 0);
    ioctl(*fd_hit, PERF_EVENT_IOC_RESET, 0);
    ioctl(*fd_miss, PERF_EVENT_IOC_RESET, 0);
    ioctl(*fd_access, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(*fd_hit, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(*fd_miss, PERF_EVENT_IOC_ENABLE, 0);
}

uint32_t stop_and_read_counter(uint32_t fd, uint64_t* cnt) {
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    read(fd, cnt, sizeof(uint64_t));
}
// int main() {
//     // Setup the performance counter for CPU cycles
//     int cpu = 0;  // Monitor the first CPU
//     // printf("%llx", get_raw_config(OP_CACHE_HIT_MISS_EVENT, OP_CACHE_HIT_UMASK));
//     // int fd = setup_perf_event(PERF_TYPE_RAW, get_raw_config(OP_CACHE_HIT_MISS_EVENT, OP_CACHE_HIT_UMASK), -1);
//     // ioctl(fd, PERF_EVENT_IOC_RESET, 0);
//     // ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
//     uint32_t fd1, fd2, fd3;
//     uint64_t cnt1, cnt2, cnt3;
//     start_op_cache_counter(&fd1, &fd2, &fd3);
//     int mod = 998244353, sum = 0;
//     for (int i = 0; i < 100000000; i++)
//         sum = (sum + 1ll * i * i %mod) % mod;
//     printf("%d\n", sum);

//     stop_and_read_counter(fd1, &cnt1);
//     stop_and_read_counter(fd2, &cnt2);
//     stop_and_read_counter(fd3, &cnt3);


//     printf("Event counters: %llu %llu %llu\n", cnt1, cnt2, cnt3);

//     close(fd1);
//     close(fd2);
//     close(fd3);
//     return 0;
// }
#endif