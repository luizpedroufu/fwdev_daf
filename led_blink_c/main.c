#include <stdint.h>
#include <stdbool.h>

#define SYSTICK_CTRL     0xE000E010
#define SYSTICK_LOAD     0xE000E014
#define SYSTICK_VAL      0xE000E018

#define STM32_RCC_BASE   0x46020C00
#define STM32_GPIOC_BASE 0x42020800

#define STM32_AHB1ENR_OFFSET     0x008C

#define STM32_GPIO_MODER_OFFSET  0x0000  
#define STM32_GPIO_OTYPER_OFFSET 0x0004  
#define STM32_GPIO_PUPDR_OFFSET  0x000c  
#define STM32_GPIO_BSRR_OFFSET   0x0018 

#define SRAM_START 0x20000000U
#define SRAM_SIZE (786U * 1024U)
#define SRAM_END ((SRAM_START + SRAM_SIZE))

#define STACK_POINTER SRAM_END

#define STM32_RCC_AHB1ENR  (STM32_RCC_BASE + STM32_AHB1ENR_OFFSET)

#define STM32_GPIOC_MODER  (STM32_GPIOC_BASE + STM32_GPIO_MODER_OFFSET)
#define STM32_GPIOC_OTYPER (STM32_GPIOC_BASE + STM32_GPIO_OTYPER_OFFSET)
#define STM32_GPIOC_PUPDR  (STM32_GPIOC_BASE + STM32_GPIO_PUPDR_OFFSET)
#define STM32_GPIOC_BSRR   (STM32_GPIOC_BASE + STM32_GPIO_BSRR_OFFSET)

#define INIT_STM32_REG_POINTER(name, address) volatile uint32_t *name = (uint32_t*)address;

void stm32_configure_clock(void);
void stm32_configure_pc13(void);
bool stm32_set_pc13(void);
bool stm32_reset_pc13(void);
void systick_init(void);

void Reset_Handler      (void);
void SysTick_Handler    (void);
void nmi_handler        (void) __attribute__ ((weak, alias("default_handler")));
void hardfault_handler  (void) __attribute__ ((weak, alias("default_handler")));
void memmanage_handler  (void) __attribute__ ((weak, alias("default_handler")));
void busfault_handler   (void) __attribute__ ((weak, alias("default_handler")));
void usagefault_handler (void) __attribute__ ((weak, alias("default_handler")));
void svc_handler        (void) __attribute__ ((weak, alias("default_handler")));
void debugmon_handler   (void) __attribute__ ((weak, alias("default_handler")));
void pendsv_handler     (void) __attribute__ ((weak, alias("default_handler")));


void app_setup(void);
void app_loop(void);
void app_led_toggle(void);

volatile uint32_t ctr = 0;
volatile bool pc13_state = false;

extern uint32_t _sdata;     
extern uint32_t _edata;     
extern uint32_t _la_data;   

extern uint32_t _sbss;      
extern uint32_t _ebss;   

const uint32_t vectors[] __attribute__((section(".isr_vectors"))) =
{
    STACK_POINTER,                          /* 0x0000 0000 */
    (uint32_t)Reset_Handler,                /* 0x0000 0004 */
    (uint32_t)nmi_handler,                  /* 0x0000 0008 */
    (uint32_t)hardfault_handler,            /* 0x0000 000c */
    (uint32_t)memmanage_handler,            /* 0x0000 0010 */
    (uint32_t)busfault_handler,             /* 0x0000 0014 */
    (uint32_t)usagefault_handler,           /* 0x0000 0018 */
    0,                                      /* 0x0000 001c */
    0,                                      /* 0x0000 0020 */
    0,                                      /* 0x0000 0024 */
    0,                                      /* 0x0000 0028 */
    (uint32_t)svc_handler,                  /* 0x0000 002c */
    (uint32_t)debugmon_handler,             /* 0x0000 0030 */
    0,                                      /* 0x0000 0034 */
    (uint32_t)pendsv_handler,               /* 0x0000 0038 */
    (uint32_t)SysTick_Handler,              /* 0x0000 003c */
};

int main(void)
{
    app_setup();

    while(1)
        app_loop();
        
    return 1; // should never get here
}

void stm32_configure_clock(void)
{
    INIT_STM32_REG_POINTER(p_rcc_ahb1enr, STM32_RCC_AHB1ENR);
    INIT_STM32_REG_POINTER(p_rcc_cr, STM32_RCC_BASE)

    uint32_t aux = *p_rcc_ahb1enr;
    aux |= (1 << 2); //ENABLE CLOCK GPIOC
    *p_rcc_ahb1enr = aux;

    aux = *p_rcc_cr;
    aux |= (1 << 8);
    *p_rcc_cr = aux;
}

void stm32_configure_pc13(void)
{
    INIT_STM32_REG_POINTER(p_gpioc_moder, STM32_GPIOC_MODER);
    INIT_STM32_REG_POINTER(p_gpioc_otyper, STM32_GPIOC_OTYPER);
    INIT_STM32_REG_POINTER(p_gpioc_pupdr, STM32_GPIOC_PUPDR);

    // MODER config
    uint32_t aux = *p_gpioc_moder;
    aux &= ~(3 << 26); // CLEAN BITS 26/27
    aux |= (1 << 26); // general purpose output mode
    *p_gpioc_moder = aux;

    // OTYPER
    //none
    *p_gpioc_otyper = *p_gpioc_otyper;
    
    // PUPDR
    //none
    *p_gpioc_pupdr = *p_gpioc_pupdr; 
}

bool stm32_set_pc13(void)
{
    INIT_STM32_REG_POINTER(p_pc13_set, STM32_GPIOC_BSRR);
    uint32_t aux = (1 << 13);
    *p_pc13_set = aux;

    return true;
}

bool stm32_reset_pc13(void)
{
    INIT_STM32_REG_POINTER(p_pc13_reset, STM32_GPIOC_BSRR);
    uint32_t aux = (1 << (13 + 16));
    *p_pc13_reset = aux;

    return false;
}

void systick_configure(uint32_t ticks)
{
    INIT_STM32_REG_POINTER(p_stk_load, SYSTICK_LOAD);
    INIT_STM32_REG_POINTER(p_stk_ctrl, SYSTICK_CTRL);
    INIT_STM32_REG_POINTER(p_stk_val, SYSTICK_VAL);

    uint32_t aux = ticks - 1;
    *p_stk_load = aux;

    aux = 0;
    *p_stk_val = aux;

    aux = (7); // == 7 | CLKSOURCE -> CPU | TICKINT -> enabled | ENABLE -> SysTick
    *p_stk_ctrl = aux;
}

void app_setup(void)
{
    stm32_configure_clock();
    stm32_configure_pc13();
    
    systick_configure(16000); //depende do seu clock 
    pc13_state = stm32_reset_pc13();
}

void app_loop(void)
{
    if(!(ctr % 1000))
    {    
        app_led_toggle();
        ctr = 0;
    }
}

void app_led_toggle(void)
{
    pc13_state = pc13_state ? stm32_reset_pc13() : stm32_set_pc13();
}

void Reset_Handler(void)
{
    uint32_t i; 
    
    uint32_t size = (uint32_t)&_edata - (uint32_t)&_sdata;
    uint8_t *p_dst = (uint8_t*)&_sdata;                      
    uint8_t *p_src = (uint8_t*)&_la_data;                    
    
    for(i = 0; i < size; i++)
        *p_dst++ = *p_src++;

    size = (uint32_t)&_ebss - (uint32_t)&_sbss;
    p_dst = (uint8_t*)&_sbss;

    for(i = 0 ; i < size; i++)
        *p_dst++ = 0;

    main();
}

void SysTick_Handler(void)
{
    ctr++;
}

void default_handler(void)
{
    while(1) { }
}

// #include <stdint.h>

// #define RCC_AHB1ENR   (*(volatile uint32_t*)0x40023830u)
// #define GPIOC_MODER   (*(volatile uint32_t*)0x40020800u)
// #define GPIOC_BSRR    (*(volatile uint32_t*)0x40020818u)

// #define PC13 13u

// static volatile uint32_t trosobeira;

// static void delay(uint32_t t) {
//     while (t--) {
//         trosobeira = t;
//     }
// }

// int main(void)
// {
//     //set do clock do gpioC
//     RCC_AHB1ENR |= (1u << 2);

//     //seta o moder do pino 13
//     GPIOC_MODER = (GPIOC_MODER & ~(3u << (PC13 * 2))) | (1u << (PC13 * 2));

//     while(1)
//     {
//         //set do pino do led
//         GPIOC_BSRR = (1u << (PC13 + 16));
//         delay(800000);

//         //reset do pino do led
//         GPIOC_BSRR = (1u << PC13);
//         delay(800000);
//     }
// }