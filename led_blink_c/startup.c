#include <stdint.h>

int main(void);

#define SRAM_START 0x20000000U
#define SRAM_SIZE (786U * 1024U)
#define SRAM_END ((SRAM_START + SRAM_SIZE))

#define STACK_POINTER SRAM_END

void Reset_Handler      (void);
void nmi_handler        (void) __attribute__ ((weak, alias("default_handler")));
void hardfault_handler  (void) __attribute__ ((weak, alias("default_handler")));
// void memmanage_handler  (void) __attribute__ ((weak, alias("default_handler")));
// void busfault_handler   (void) __attribute__ ((weak, alias("default_handler")));
// void usagefault_handler (void) __attribute__ ((weak, alias("default_handler")));
// void svc_handler        (void) __attribute__ ((weak, alias("default_handler")));
// void debugmon_handler   (void) __attribute__ ((weak, alias("default_handler")));
// void pendsv_handler     (void) __attribute__ ((weak, alias("default_handler")));
void SysTick_Handler    (void) __attribute__ ((weak, alias("default_handler")));

extern uint32_t _sdata;     
extern uint32_t _edata;     
extern uint32_t _la_data;   

extern uint32_t _sbss;      
extern uint32_t _ebss;      

// const uint32_t vectors[] __attribute__((section(".isr_vectors"))) =
// {
//     STACK_POINTER,                          /* 0x0000 0000 */
//     (uint32_t)Reset_Handler,                /* 0x0000 0004 */
//     (uint32_t)nmi_handler,                  /* 0x0000 0008 */
//     (uint32_t)hardfault_handler,            /* 0x0000 000c */
//     (uint32_t)memmanage_handler,            /* 0x0000 0010 */
//     (uint32_t)busfault_handler,             /* 0x0000 0014 */
//     (uint32_t)usagefault_handler,           /* 0x0000 0018 */
//     0,                                      /* 0x0000 001c */
//     0,                                      /* 0x0000 0020 */
//     0,                                      /* 0x0000 0024 */
//     0,                                      /* 0x0000 0028 */
//     (uint32_t)svc_handler,                  /* 0x0000 002c */
//     (uint32_t)debugmon_handler,             /* 0x0000 0030 */
//     0,                                      /* 0x0000 0034 */
//     (uint32_t)pendsv_handler,               /* 0x0000 0038 */
//     (uint32_t)systick_handler,              /* 0x0000 003c */
// };

const uint32_t vectors[] __attribute__((section(".isr_vectors"))) =
{
    STACK_POINTER,                          /* 0x0000 0000 */
    (uint32_t)Reset_Handler,                /* 0x0000 0004 */
    (uint32_t)nmi_handler,                  /* 0x0000 0008 */
    (uint32_t)hardfault_handler,            /* 0x0000 000c */
    (uint32_t)SysTick_Handler,
};

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

void default_handler(void)
{
    while(1) { }
}