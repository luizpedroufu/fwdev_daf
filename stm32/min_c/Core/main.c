// config: -nostartfiles -nodefaultlibs -nostdlib --specs=nano.specs

#define STACK_START    0x20008000

#define SYSTICK_CTRL   0xE000E010
#define SYSTICK_LOAD   0xE000E014
#define SYSTICK_VAL    0xE000E018

#define RCC_BASE       0x40021000
#define RCC_AHB2ENR   (RCC_BASE + 76)
#define GPIOB_BASE     0x48000400
#define GPIOB_MODER   (GPIOB_BASE + 0)
#define GPIO_ODR_OFFS  20

extern unsigned int __start_ram_bss;
extern unsigned int __end_ram_bss;
extern unsigned int __start_ram_data;
extern unsigned int __end_ram_data;
extern unsigned int __start_flash_data;

void Reset_Handler(void);
void SysTick_Handler(void);
int main(void);

volatile unsigned int systick_cnt = 0;

__attribute__((section(".isr_vectors")))
void (* const vectors[])(void) =
{
    (void (*)(void))STACK_START,
    Reset_Handler,
	Reset_Handler,
	Reset_Handler,
	Reset_Handler,
	Reset_Handler,
	Reset_Handler,
	0,
	0,
	0,
	0,
	Reset_Handler,
	Reset_Handler,
	0,
	Reset_Handler,
	SysTick_Handler,
};

static void init_data(void)
{
	unsigned int *dst = &__start_ram_data;
	unsigned int *end = &__end_ram_data;
	unsigned int *src = &__start_flash_data;

	while (dst < end)
	{
		*dst++ = *src++;
	}
}

static void init_bss(void)
{
	unsigned int *dst = &__start_ram_bss;
	unsigned int *end = &__end_ram_bss;

	while (dst < end)
	{
		*dst++ = 0;
	}
}

__attribute__((naked))
void Reset_Handler(void)
{
	init_data();
	init_bss();
	main();
}

void SysTick_Handler(void)
{
	systick_cnt++;
}

void delay_ms(unsigned int t_ms)
{
	unsigned int start_ms = systick_cnt;

	while((systick_cnt - start_ms) < t_ms)
	{}
}

void config_pb8_as_output(void)
{
	unsigned int *reg = (unsigned int *) RCC_AHB2ENR;
	*reg |= (1 << 1);

	reg = (unsigned int *) GPIOB_MODER;
	unsigned int tmp = *reg & ~(3 << 16);
	tmp |= (1 << 16);
	*reg = tmp;
}

void led_on(unsigned int port_base, unsigned int pin)
{
	unsigned int *reg = (unsigned int *) (port_base + GPIO_ODR_OFFS);
	*reg |= (1 << pin);
}

void led_off(unsigned int port_base, unsigned int pin)
{
    unsigned int *reg = (unsigned int *) (port_base + GPIO_ODR_OFFS);
    *reg &= ~(1 << pin);
}

void config_systick(void)
{
    unsigned int *reg = (unsigned int *) SYSTICK_LOAD;
    *reg = 15999;

    reg = (unsigned int *) SYSTICK_VAL;
    *reg = 0;

    reg = (unsigned int *) SYSTICK_CTRL;
    *reg = 0x07;

	systick_cnt = 0;
}

int main(void)
{
	config_pb8_as_output();
	config_systick();

	while (1)
	{
		led_on(GPIOB_BASE, 8);
		delay_ms(100);
		led_off(GPIOB_BASE, 8);
		delay_ms(100);
	}
}
