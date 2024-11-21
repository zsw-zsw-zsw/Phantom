#ifndef PERF_EVENT_WRAPPER_H
#define PERF_EVENT_WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <linux/hw_breakpoint.h>
#include <unistd.h>


typedef struct perf_event_attr perf_event_attr;
uint32_t perf_event_open(perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags);
uint32_t setup_perf_event(uint32_t event_type, uint64_t config, uint32_t group_fd);
uint64_t get_raw_config(uint64_t event_id, uint64_t umask);

#endif
