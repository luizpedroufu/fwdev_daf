#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define UOS_TICKS_PER_SECOND 1000
#define UOS_MIN_STACK_SIZE_IN_WORDS 256

#define uos_syscall uos_port_syscall

typedef enum uos_syscalls_e
{
	UOS_SYSCALL_START_KERNEL = 0,
	UOS_SYSCALL_ENABLE_INTERRUPTS,
	UOS_SYSCALL_DISABLE_INTERRUPTS,
} uos_syscalls_t;

typedef void (*uos_task_entry_t)(void *);

typedef struct uos_tcb_s
{
    struct utl_lst_node node;
    uintptr_t *stack_area;
    uint32_t stack_size_in_words;
    uintptr_t *stack_pointer;
    uos_task_entry_t task_entry;
    void *arg;
} uos_tcb_t;

uint32_t uos_syscall_handler(uint32_t svc_number, uint32_t arg1, uint32_t arg2, uint32_t arg3);
// add a task to the scheduler
bool uos_task_add(uos_tcb_t *task);
// start the os kernel
void uos_kernel_start(void);
// initialize the os basic structures
void uos_init(void);

#ifdef __cplusplus
}
#endif
