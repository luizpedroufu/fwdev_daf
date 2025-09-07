/*
 * uos.c
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

static struct utl_lst_node task_list;
static struct utl_lst_node *current_task;
static bool kernel_started = false;
static uint64_t tick_cnt;

bool uos_task_add(uos_tcb_t *tcb)
{
    if(tcb == 0 ||
       tcb->stack_area == 0 ||
       (tcb->stack_size_in_words < UOS_MIN_STACK_SIZE_IN_WORDS) ||
       (tcb->task_entry == 0))
        return false;

    uos_port_task_stack_init(tcb);

    uint32_t state = uos_port_critical_section_enter();
    utl_lst_add(&task_list,&tcb->node,true);
    uos_port_critical_section_leave(state);

    return true;
}

static void uos_timer_cbk(void)
{
    if(kernel_started)
    {
    	tick_cnt++;
        uint32_t state = uos_port_critical_section_enter();
        uos_port_context_switch_trigger();
        uos_port_critical_section_leave(state);
    }
}

static uos_tcb_t* uos_context_switch_cbk(void)
{
    if(kernel_started)
    {
        current_task = current_task->next;
        if(current_task == &task_list)
			current_task = current_task->next;
        uos_tcb_t* tcb = utl_lst_container_of(current_task, uos_tcb_t, node);
        return tcb;
    }
    else
    	return 0;
}

uint32_t uos_syscall_handler(uint32_t svc_number, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
	uint32_t ret = 0;

    switch(svc_number)
    {
//        case UOS_SYSCALL_START_KERNEL:
//        {
//        	uos_tcb_t* tcb = utl_lst_container_of(current_task, uos_tcb_t, node);
//        	ret = (uint32_t) tcb->arg;
//            break;

        case UOS_SYSCALL_ENABLE_INTERRUPTS:
        	ret = 2;
            break;

        case UOS_SYSCALL_DISABLE_INTERRUPTS:
        	ret = 3;
        	break;

        default:
            break;
    }

    return ret;
}

void uos_task_default(void *arg)
{
    while(true)
    {}
}

__WEAK void uos_app_init(void)
{
    static uint32_t stack[UOS_MIN_STACK_SIZE_IN_WORDS];
    static uos_tcb_t tcb =
    {
        .stack_area = (uintptr_t *) stack,
        .stack_size_in_words = UOS_MIN_STACK_SIZE_IN_WORDS,
        .task_entry = uos_task_default
    };
    uos_task_add(&tcb);
}

void uos_init(void)
{
	tick_cnt = 0;
	utl_lst_init(&task_list);
    uos_port_timer_configure(UOS_TICKS_PER_SECOND,uos_timer_cbk);
    uos_port_context_switch_configure(uos_context_switch_cbk);
    uos_app_init();
}

void uos_kernel_start(void)
{
    current_task = task_list.next;
    kernel_started = true;
    uos_port_first_task_schedule(utl_lst_container_of(current_task, uos_tcb_t, node));
}
