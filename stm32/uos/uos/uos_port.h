#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define __WEAK __attribute__((weak))

uint32_t uos_port_critical_section_enter(void);
void uos_port_critical_section_leave(uint32_t state);
void uos_port_task_stack_init(uos_tcb_t *tcb);
void uos_port_timer_configure(uint32_t ticks_per_second, void (*cbk)(void));
void uos_port_context_switch_configure(uos_tcb_t* (*cbk)(void));
void uos_port_context_switch_trigger(void);
void uos_port_context_save(uos_tcb_t *tcb);
void uos_port_context_restore(uos_tcb_t *tcb);
void uos_port_first_task_schedule(uos_tcb_t *tcb);
uint32_t uos_port_syscall(uos_syscalls_t syscall, uint32_t arg1, uint32_t arg2, uint32_t arg3);

#ifdef __cplusplus
}
#endif
