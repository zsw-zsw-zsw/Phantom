#define csr_read(csr)                                           \
	({                                                      \
		register unsigned long __v;                     \
		__asm__ __volatile__("csrr %0, " __ASM_STR(csr) \
				     : "=r"(__v)                \
				     :                          \
				     : "memory");               \
		__v;                                            \
	})

#define csr_write(csr, val)                                        \
	({                                                         \
		unsigned long __v = (unsigned long)(val);          \
		__asm__ __volatile__("csrw " __ASM_STR(csr) ", %0" \
				     :                             \
				     : "rK"(__v)                   \
				     : "memory");                  \
	})

#define csr_set(csr, val)                                          \
	({                                                         \
		unsigned long __v = (unsigned long)(val);          \
		__asm__ __volatile__("csrs " __ASM_STR(csr) ", %0" \
				     :                             \
				     : "rK"(__v)                   \
				     : "memory");                  \
	})

#define csr_clear(csr, val)                                        \
	({                                                         \
		unsigned long __v = (unsigned long)(val);          \
		__asm__ __volatile__("csrc " __ASM_STR(csr) ", %0" \
				     :                             \
				     : "rK"(__v)                   \
				     : "memory");                  \
	})

#define clear_event(id) csr_write(mhpmevent##id, 0x0UL)
#define print_event(id) printf("mhpmevent%d: %lx\n", id, csr_read(mhpmevent##id))
#define clear_counter(id) csr_write(mhpmcounter##id, 0x0UL)
#define print_counter(id) printf("mhpmcounter%d: %lu\n", id, csr_read(mhpmcounter##id))
#define printd_csr(csr) printf(#csr": %ld\n", csr_read(csr))
#define printu_csr(csr) printf(#csr": %lu\n", csr_read(csr))
#define printx_csr(csr) printf(#csr": %lx\n", csr_read(csr))

// #define get_csr(csr_id) syscall(GET_PERF, EVENT_BASE + csr_id)