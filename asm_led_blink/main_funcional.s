.syntax unified
.cpu cortex-m33
.thumb

/****************************************************************************
 * Pre-processor Definitions (Equivalente Assembly)
 ****************************************************************************/

/* AHB1 Base Addresses */
.equ STM32_RCC_BASE,       0x46020c00  /* Reset and Clock control RCC */

/* GPIOC Base Addresses */
.equ STM32_GPIOC_BASE,     0x42020800  /* GPIO Port C */

/* Register Offsets */
.equ STM32_RCC_AHB1ENR_OFFSET, 0x008C  /* AHB1 Peripheral Clock enable register */
.equ STM32_GPIO_MODER_OFFSET,  0x0000  /* GPIO port mode register */
.equ STM32_GPIO_OTYPER_OFFSET, 0x0004  /* GPIO port output type register */
.equ STM32_GPIO_PUPDR_OFFSET,  0x000c  /* GPIO port pull-up/pull-down register */
.equ STM32_GPIO_BSRR_OFFSET,   0x0018  /* GPIO port bit set/reset register */

/* Register Addresses */
.equ STM32_RCC_AHB1ENR,    (STM32_RCC_BASE + STM32_RCC_AHB1ENR_OFFSET)
.equ STM32_GPIOC_MODER,    (STM32_GPIOC_BASE + STM32_GPIO_MODER_OFFSET)
.equ STM32_GPIOC_OTYPER,   (STM32_GPIOC_BASE + STM32_GPIO_OTYPER_OFFSET)
.equ STM32_GPIOC_PUPDR,    (STM32_GPIOC_BASE + STM32_GPIO_PUPDR_OFFSET)
.equ STM32_GPIOC_BSRR,     (STM32_GPIOC_BASE + STM32_GPIO_BSRR_OFFSET)

/* AHB1 Peripheral Clock enable register */
.equ RCC_AHB1ENR_GPIOCEN,  (1 << 2)  /* Bit 2: IO port C clock enable */

/* --- CORREÇÃO: Macros com parâmetros foram removidas --- */
/* Os valores serão calculados diretamente no código.   */
.equ GPIO_MODER_OUTPUT,    (1)
.equ GPIO_OTYPER_PP,       (0)
.equ GPIO_PUPDR_NONE,      (0)

/* Configuration */
.equ LED_DELAY,            1000000
.equ LED_PIN,              13

/****************************************************************************
 * Vetor de Reset
 ****************************************************************************/

.section .isr_vector, "a", %progbits
  .word _estack         /* Topo da pilha */
  .word Reset_Handler   /* Ponto de entrada após reset */

/****************************************************************************
 * Código Principal
 ****************************************************************************/

.section .text
.global Reset_Handler
Reset_Handler:
  b _start

.global main
_start:
  ldr sp, =_estack

  /* Habilita clock GPIOC */
  ldr r0, =STM32_RCC_AHB1ENR
  ldr r1, [r0]
  orr r1, r1, #RCC_AHB1ENR_GPIOCEN
  str r1, [r0]

  /* Configura PC13 como saida pull-up off e pull-down off */
  ldr r4, =STM32_GPIOC_BASE

  /* Configura MODER para saída */
  ldr r1, [r4, #STM32_GPIO_MODER_OFFSET]
  /* CORRIGIDO: Cálculo direto da máscara para o pino 13 */
  bic r1, r1, #(3 << (LED_PIN * 2))
  /* CORRIGIDO: Cálculo direto do valor para o pino 13 */
  orr r1, r1, #(GPIO_MODER_OUTPUT << (LED_PIN * 2))
  str r1, [r4, #STM32_GPIO_MODER_OFFSET]

  /* Configura OTYPER para push-pull */
  ldr r1, [r4, #STM32_GPIO_OTYPER_OFFSET]
  /* CORRIGIDO: Cálculo direto da máscara para o pino 13 */
  bic r1, r1, #(1 << LED_PIN)
  /* CORRIGIDO: Cálculo direto do valor para o pino 13 */
  orr r1, r1, #(GPIO_OTYPER_PP << LED_PIN)
  str r1, [r4, #STM32_GPIO_OTYPER_OFFSET]

  /* Configura PUPDR para sem pull-up/pull-down */
  ldr r1, [r4, #STM32_GPIO_PUPDR_OFFSET]
  /* CORRIGIDO: Cálculo direto da máscara para o pino 13 */
  bic r1, r1, #(3 << (LED_PIN * 2))
  /* CORRIGIDO: Cálculo direto do valor para o pino 13 */
  orr r1, r1, #(GPIO_PUPDR_NONE << (LED_PIN * 2))
  str r1, [r4, #STM32_GPIO_PUPDR_OFFSET]

loop:
  /* Liga LED (Reset no pino 13) */
  ldr r0, =STM32_GPIOC_BSRR
  /* CORRIGIDO: Cálculo direto do valor */
  mov r1, #(1 << (LED_PIN + 16))
  str r1, [r0]
  bl delay

  /* Desliga LED (Set no pino 13) */
  ldr r0, =STM32_GPIOC_BSRR
  /* CORRIGIDO: Cálculo direto do valor */
  mov r1, #(1 << LED_PIN)
  str r1, [r0]
  bl delay

  b loop

delay:
  ldr r3, =LED_DELAY
delay_loop:
  subs r3, r3, #1
  bne delay_loop
  bx lr
