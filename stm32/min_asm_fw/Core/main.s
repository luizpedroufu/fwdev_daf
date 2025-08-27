//// config: -nostartfiles -nodefaultlibs -nostdlib --specs=nano.specs

.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.equ STACK_START,	 0x20008000

.equ SYSTICK_CTRL,   0xE000E010
.equ SYSTICK_LOAD,   0xE000E014
.equ SYSTICK_VAL,    0xE000E018

.equ RCC_BASE,  	0x40021000
.equ RCC_AHB2ENR, 	(RCC_BASE + 76)
.equ GPIOB_BASE, 	0x48000400
.equ GPIOB_MODER, 	(GPIOB_BASE + 0)
.equ GPIO_ODR_OFFS, 20

.macro export_function_symbol name
.global \name
.type \name, %function
.endm

.macro export_variable_symbol name
.global \name
.type \name, %object
.endm

// Exporting global function symbols to the linker
export_function_symbol Reset_Handler
export_function_symbol SysTick_Handler
export_function_symbol config_stack
export_function_symbol config_systick
export_function_symbol main
export_function_symbol config_pb8_as_output
export_function_symbol led_on
export_function_symbol led_off
export_function_symbol delay_ms

// Exporting global variable symbols to the linker
export_variable_symbol systick_cnt
export_variable_symbol vectors

.section .text,"ax",%progbits

Reset_Handler:
	bl config_stack
	bl main
	.size	Reset_Handler, .-Reset_Handler

main:
	bl config_pb8_as_output
	bl config_systick
main_loop:
    ldr r0, =GPIOB_BASE
    ldr r1, =8
    bl led_off

    ldr r0,=100
    bl delay_ms

    ldr r0, =GPIOB_BASE
    ldr r1, =8
    bl led_on

    ldr r0,=100
    bl delay_ms

	bl main_loop
	.size	main, .-main

config_stack:
	ldr r0, =STACK_START
	msr msp, r0
	bx lr
	.size config_stack, .-config_stack

// Function parameters:
// r0: milliseconds to delay
delay_ms:
    push {r0, r1, r2, lr}
    // r1 = t0
    ldr r1, =systick_cnt
    ldr r1, [r1]
delay_ms_loop:
    // r2 = t1
    ldr r2, =systick_cnt
    ldr r2, [r2]
    // t1 - t0
    sub r2, r2, r1
    cmp r2, r0
    // If t1 - t0 < r0, continue waiting
    blt delay_ms_loop
    pop  {r0, r1, r2, pc}
  	.size delay_ms, .-delay_ms

config_pb8_as_output:
	push {r0, r1, lr}
    // Enable GPIO clock (RCC->AHB2EBR[1] = 1)
	ldr r0, =RCC_AHB2ENR
	ldr r1, [r0]
	orr r1, r1, #(1 << 1)
	str r1, [r0]

	// Configure PB8 as output (GPIO->MODER[16:17] = 01)
	ldr r0, =GPIOB_MODER
	ldr r1, [r0]
	bic r1, r1, #(3 << 16)
	orr r1, r1, #(1 << 16)
	str r1, [r0]
	pop {r0, r1, pc}
	.size config_pb8_as_output, .-config_pb8_as_output

// Function parameters:
// r0: port
// r1: pin
// [r0+odr_offset]->ODR[pin] = 1
led_on:
    push {r0, r1, r2, r3, lr}
	add r0, r0, #GPIO_ODR_OFFS
    ldr r2, [r0]
    mov r3, 1
    lsl r3, r1
    orr r2, r2, r3
    str r2, [r0]
    pop {r0, r1, r2, r3, pc}
    .size led_off, .-led_off

// Function parameters:
// r0: port
// r1: pin
// [r0+odr_offset]->ODR[pin] = 0
led_off:
    push {r0, r1, r2, r3, lr}
	add r0, r0, #GPIO_ODR_OFFS
    ldr r2, [r0]
    mov r3, 1
    lsl r3, r1
    bic r2, r2, r3
    str r2, [r0]
    pop {r0, r1, r2, r3, pc}
    .size led_off, .-led_off

config_systick:
    push {r0, r1, lr}
	// Configure SysTick ti 1ms (assuming 16MHz clock: 16MHz / 1000 - 1 = 15999)
	ldr r0, =SYSTICK_LOAD
	ldr r1, =15999
	str r1, [r0]
	// Clear SysTick current value register
	ldr r0, =SYSTICK_VAL
	mov r1, #0
	str r1, [r0]
	// Enable SysTick with processor clock and interrupts
	ldr r0, =SYSTICK_CTRL
	mov r1, #0x7
	str r1, [r0]
	// Init SysTick counter
	ldr r0, =systick_cnt
	mov r1, #0
	str r1, [r0]
	pop {r0, r1, pc}
	.size config_systick, .-config_systick

SysTick_Handler:
	ldr r0, =systick_cnt
	ldr r1, [r0]
	add r1, r1, #1
	str r1, [r0]
	bx lr
	.size	SysTick_Handler, .-SysTick_Handler

.section .isr_vectors,"a",%progbits

vectors:
	.word	STACK_START
	.word	Reset_Handler
	.word	Reset_Handler
	.word	Reset_Handler
	.word	Reset_Handler
	.word	Reset_Handler
	.word	Reset_Handler
	.word	0
	.word	0
	.word	0
	.word	0
	.word	Reset_Handler
	.word	Reset_Handler
	.word	0
	.word	Reset_Handler
	.word	SysTick_Handler
	.size	vectors, .-vectors

.section .bss, "aw", %nobits
.balign 4

systick_cnt:
 	.space 4
  	.size systick_cnt, .-systick_cnt

