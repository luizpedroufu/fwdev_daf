/*
 * app.c
 *
 *  Created on: Sep 7, 2025
 *      Author: pedro-bittencourt
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "main.h"
#include "utl_lst.h"
#include "uos.h"

uint32_t vals[2] = {0, 0};

void app_task_led(void *arg)
{
	uint32_t id = *((uintptr_t *)arg);

    while(true)
    {
    	vals[id-1] = HAL_GetTick();
    	HAL_GPIO_TogglePin(LED_OBD_GPIO_Port, LED_OBD_Pin);
    	HAL_Delay(100);
    }
}

void app_task_serial(void *arg)
{
	uint32_t id = *((uintptr_t *)arg);

    while(true)
    {
    	vals[id-1] = HAL_GetTick();
    	printf("Task LED: Tick %lu\n", vals[0]);
    	printf("Task SER: Tick %lu\n", vals[1]);
    	HAL_Delay(100);
    }
}

void uos_app_init(void)
{
    static uint32_t stack_a[UOS_MIN_STACK_SIZE_IN_WORDS];
    static uint32_t id_a = 1;
    static uint32_t id_b = 2;
    static uos_tcb_t tcb_a =
    {
        .stack_area = (uintptr_t *) stack_a,
        .stack_size_in_words = UOS_MIN_STACK_SIZE_IN_WORDS,
        .task_entry = app_task_led,
    };
    tcb_a.arg = (void *)&id_a;
    uos_task_add(&tcb_a);

    static uint32_t stack_b[UOS_MIN_STACK_SIZE_IN_WORDS];
    static uos_tcb_t tcb_b =
    {
        .stack_area = (uintptr_t *) stack_b,
        .stack_size_in_words = UOS_MIN_STACK_SIZE_IN_WORDS,
        .task_entry = app_task_serial,
    };
    tcb_b.arg = (void *)&id_b;
    uos_task_add(&tcb_b);
}
