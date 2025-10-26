/*
 * uos_port.c
 *
 *  Created on: Sep 7, 2025
 *      Author: pedro-bittencourt
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "utl_lst.h"
#include "uos.h"
#include "uos_port.h"
#include "main.h"

#define OS_PORT_TIMER_PRIO 5
#define OS_PORT_SCHED_PRIO 5

static void (*timer_cbk)(void) = 0;
static uos_tcb_t* (*context_switch_cbk)(void) = 0;
static uint32_t ticks_per_second = 0;
static uos_tcb_t *current_tcb;

uint32_t uos_port_critical_section_enter(void)
{

}

void uos_port_critical_section_leave(uint32_t state)
{
    // implementation specific to the platform
}

void SysTick_Handler(void)
{
    static uint32_t tick_cnt = 0;

    if(++tick_cnt >= ticks_per_second)
    {
        tick_cnt = 0;
        if(timer_cbk)
            timer_cbk();
    }
}

__attribute__((naked))
void SVC_Handler(void)
{
    __asm volatile (
        /* r4 = pointer para stacked frame (MSP ou PSP dependendo de LR[2]) */
        "tst lr, #4                 \n"
        "ite eq                     \n"
        "mrseq r4, msp              \n"
        "mrsne r4, psp              \n"

        /* stacked r0 contém o syscall number (do uos_port_syscall mov r0, <syscall>) */
        "ldr r0, [r4, #0]           \n" /* syscall number */
        "cmp r0, #0                 \n"
        "bne 1f                     \n" /* se != 0 -> trata outros syscalls */

        /* ---- svc == 0 : start kernel with first task ---- */
        /* primeiro argumento (stack pointer) está em stacked r1 */
        "ldr r1, [r4, #4]           \n" /* r1 = first task stack pointer */

        /* colocar o PSP com a pilha da primeira task */
        "msr psp, r1                \n"

        /* set CONTROL bit0 = 1 (usar PSP em Thread mode), manter privilégio (bit1=0) */
        "movs r0, #2                \n"
        "msr CONTROL, r0            \n"
        "isb                        \n"

        /* recuperar r4-r11 da pilha da task (usando PSP).
           Como estamos em Handler mode, não usamos POP direto (afeta MSP).
           Fazemos: r0 = PSP; LDMIA r0!, {r4-r11}; msr psp, r0 */
        "mrs r0, psp                \n" /* r0 = PSP */
        "ldmia r0!, {r4-r11}        \n" /* restore r4-r11 */
        "msr psp, r0                \n" /* atualizar PSP (após pop dos r4-r11) */

        /* então retorne da exceção para a task (LR já contém EXC_RETURN) */
        "bx lr                      \n"

        /* ---- outros syscalls ---- */
        "1:                         \n"
        "push {lr}                  \n"
        "bl uos_syscall_handler     \n"
        "pop {lr}                   \n"

        /* grava retorno do syscall em stacked r0 */
        "str r0, [r4, #0]           \n"
        "bx lr                      \n"
    );
}


uint32_t uos_port_syscall(uos_syscalls_t syscall, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
	uint32_t ret;

	__asm volatile (
		"mov r0, %1      \n"
		"mov r1, %2      \n"
		"mov r2, %3      \n"
		"mov r3, %4      \n"
		"svc 0           \n"
		"mov %0, r0      \n"
		: "=r" (ret)
		: "r" (syscall), "r" (arg1), "r" (arg2), "r" (arg3)
		: "r0", "r1", "r2", "r3"
	);

	return ret;
}

// TODO consider float point stack !
// TODO consider multiple stacks usage (msp and psp)

__attribute__((naked))
void PendSV_Handler(void)
{
    __asm volatile (
        "mrs r0, psp                    \n" /* r0 = PSP (thread stack) */
        "stmdb r0!, {r4-r11}            \n" /* salvar r4-r11 na pilha do thread */
        /* agora r0 é o PSP atual (depois do stmdb) */

        /* salvar r0 (psp) em current_tcb->stack_pointer somente se current_tcb != NULL */
        "ldr r1, =current_tcb           \n"
        "ldr r1, [r1]                   \n"
        "cmp r1, #0                     \n"
        "beq 2f                         \n" /* se current_tcb == NULL pula salvar (primeiro agendamento) */
        "str r0, [r1]                   \n"

        "2:                              \n"
        /* chamar callback de seleção de próximo TCB (context_switch_cbk) */
        "ldr r0, =context_switch_cbk    \n"
        "ldr r0, [r0]                   \n" /* r0 = &function */
        "cmp r0, #0                     \n"
        "beq 3f                         \n" /* se callback == NULL, pula */
        "blx r0                         \n" /* chama callback (ele deve atualizar current_tcb) */

        "3:                              \n"
        /* obter current_tcb e carregar novo PSP */
        "ldr r1, =current_tcb           \n"
        "ldr r1, [r1]                   \n"
        "cmp r1, #0                     \n"
        "beq 4f                         \n" /* se current_tcb == NULL -> return */

        "ldr r0, [r1]                   \n" /* r0 = new stack_pointer (da TCB) */

        /* restaurar r4-r11 da pilha do próximo thread */
        "ldmia r0!, {r4-r11}            \n"
        "msr psp, r0                    \n"

        "4:                              \n"
        "bx lr                          \n"
    );
}


void uos_port_task_stack_init(uos_tcb_t *tcb)
{
    // round stack size to a word boundary
    if((uint32_t)tcb->stack_area & 0x03)
    {
        tcb->stack_area = (uintptr_t *)((uintptr_t)tcb->stack_area & ~0x03) + 4;
        tcb->stack_size_in_words -= 1;
    }

    // stack operations are always downwards (decrement and set)
    tcb->stack_pointer = tcb->stack_area + tcb->stack_size_in_words;

    // standard interrupt context for cortex M
    uintptr_t *sp = tcb->stack_pointer;
    *(--sp) = 0x01000000; // xPSR with thumb bit set
    *(--sp) = (uint32_t) tcb->task_entry; // pc
    *(--sp) = 0xFFFFFFFD; // lr -> retornando para thread mode
    *(--sp) = 0; // r12
    *(--sp) = 0; // r3
    *(--sp) = 0; // r2
    *(--sp) = 0; // r1
    *(--sp) = (uint32_t) tcb->arg; // r0, task argument

    // r4-r11
    for(size_t pos = 0; pos < 8; pos++)
        *(--sp) = 0;

    tcb->stack_pointer = sp;
}

void uos_port_timer_configure(uint32_t tps, void (*cbk)(void))
{
    // configure a 1ms systick or platform specific timer
	HAL_SYSTICK_Config(SystemCoreClock/tps);
	HAL_NVIC_SetPriority(SysTick_IRQn, OS_PORT_TIMER_PRIO, 0U);

    ticks_per_second = tps;
    timer_cbk = cbk;
}

void uos_port_context_switch_configure(uos_tcb_t* (*cbk)(void))
{
    // configure pendsv
	HAL_NVIC_SetPriority(PendSV_IRQn, OS_PORT_SCHED_PRIO, 0);
    context_switch_cbk = cbk;
}

inline void uos_port_context_switch_trigger(void)
{
    // trigger pendsv
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void uos_port_context_save(uos_tcb_t *tcb)
{
    // implementation specific to the platform
}

void uos_port_context_restore(uos_tcb_t *tcb)
{
    // implementation specific to the platform

	// set the stack pointer to the task's stack pointer
	__set_PSP((uint32_t)tcb->stack_pointer);

}

void uos_port_first_task_schedule(uos_tcb_t *tcb)
{
	current_tcb = tcb;
    uos_port_syscall(UOS_SYSCALL_START_KERNEL,
    		(uint32_t) tcb->stack_pointer,
    		(uint32_t) tcb->arg,
			0);
}
