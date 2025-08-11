.syntax unified
.cpu cortex-m33
.thumb

/* (Toda a seção de .equ permanece a mesma) */
.equ HSI_FREQ, 16000000
.equ STM32_RCC_BASE,       0x46020c00
.equ STM32_GPIOC_BASE,     0x42020800
.equ SYSTICK_BASE,         0xE000E010
.equ STM32_RCC_AHB1ENR_OFFSET, 0x008c
.equ STM32_GPIO_MODER_OFFSET,  0x0000
.equ STM32_GPIO_BSRR_OFFSET,   0x0018
.equ SYSTICK_CTRL_OFFSET,      0x00
.equ SYSTICK_LOAD_OFFSET,      0x04
.equ SYSTICK_VAL_OFFSET,       0x08
.equ STM32_RCC_AHB1ENR,    (STM32_RCC_BASE + STM32_RCC_AHB1ENR_OFFSET)
.equ STM32_GPIOC_MODER,    (STM32_GPIOC_BASE + STM32_GPIO_MODER_OFFSET)
.equ STM32_GPIOC_BSRR,     (STM32_GPIOC_BASE + STM32_GPIO_BSRR_OFFSET)
.equ SYSTICK_CTRL,         (SYSTICK_BASE + SYSTICK_CTRL_OFFSET)
.equ SYSTICK_LOAD,         (SYSTICK_BASE + SYSTICK_LOAD_OFFSET)
.equ SYSTICK_VAL,          (SYSTICK_BASE + SYSTICK_VAL_OFFSET)
.equ RCC_AHB1ENR_GPIOCEN,  (1 << 2)
.equ GPIO_MODER_OUTPUT,    (1)
.equ LED_PIN,              13
.equ DELAY_TIME_MS,        500

.section .data
.global g_delay_counter
g_delay_counter:
  .word 0

.section .isr_vector, "a", %progbits
  .word _estack
  .word Reset_Handler
  .word Default_Handler /* NMI_Handler */
  .word Default_Handler /* HardFault_Handler */
  .word Default_Handler, Default_Handler, Default_Handler, Default_Handler, Default_Handler, Default_Handler, Default_Handler
  .word Default_Handler /* SVC_Handler */
  .word Default_Handler, Default_Handler
  .word Default_Handler /* PendSV_Handler */
  .word SysTick_Handler

.section .text
.global Reset_Handler
Reset_Handler:
  /* CORRIGIDO: Rotina de inicialização de memória */
  /* Copia a seção .data da Flash para a RAM */
  ldr r0, =_sidata  /* Endereço de origem na Flash */
  ldr r1, =_sdata   /* Endereço de destino na RAM */
  ldr r2, =_edata   /* Endereço final na RAM */
copy_loop:
  cmp r1, r2
  bhs copy_done     /* Se destino >= fim, termina */
  ldr r3, [r0]      /* Lê da Flash */
  add r0, r0, #4
  str r3, [r1]      /* Escreve na RAM */
  add r1, r1, #4
  b copy_loop
copy_done:

  /* Zera a seção .bss */
  ldr r0, =_sbss    /* Início do .bss */
  ldr r1, =_ebss    /* Fim do .bss */
  mov r2, #0
zero_loop:
  cmp r0, r1
  bhs zero_done     /* Se endereço atual >= fim, termina */
  str r2, [r0]      /* Zera a palavra na RAM */
  add r0, r0, #4
  b zero_loop
zero_done:

  /* Agora, pula para o código principal */
  b _start

.global main
_start:
  ldr sp, =_estack
  bl setup_gpio
  bl setup_systick
  cpsie i
loop:
  /* Liga LED */
  ldr r0, =STM32_GPIOC_BSRR
  mov r1, #(1 << (LED_PIN + 16))
  str r1, [r0]
  ldr r0, =DELAY_TIME_MS
  bl delay_ms

  /* Desliga LED */
  ldr r0, =STM32_GPIOC_BSRR
  mov r1, #(1 << LED_PIN)
  str r1, [r0]
  ldr r0, =DELAY_TIME_MS
  bl delay_ms

  b loop

/* Funções setup_gpio, setup_systick e delay_ms permanecem idênticas */
setup_gpio:
  ldr r0, =STM32_RCC_AHB1ENR
  ldr r1, [r0]
  orr r1, r1, #RCC_AHB1ENR_GPIOCEN
  str r1, [r0]
  ldr r0, =STM32_GPIOC_MODER
  ldr r1, [r0]
  bic r1, r1, #(3 << (LED_PIN * 2))
  orr r1, r1, #(GPIO_MODER_OUTPUT << (LED_PIN * 2))
  str r1, [r0]
  bx lr
setup_systick:
  ldr r0, =SYSTICK_LOAD
  ldr r1, =(HSI_FREQ / 1000) - 1
  str r1, [r0]
  ldr r0, =SYSTICK_VAL
  mov r1, #0
  str r1, [r0]
  ldr r0, =SYSTICK_CTRL
  mov r1, #0b111
  str r1, [r0]
  bx lr
delay_ms:
  ldr r1, =g_delay_counter
  str r0, [r1]
wait_loop:
  ldr r2, [r1]
  cmp r2, #0
  bne wait_loop
  bx lr

.global SysTick_Handler
SysTick_Handler:
  ldr r0, =g_delay_counter
  ldr r1, [r0]
  cmp r1, #0
  beq systick_exit
  subs r1, r1, #1
  str r1, [r0]
systick_exit:
  bx lr

Default_Handler:
  b .
