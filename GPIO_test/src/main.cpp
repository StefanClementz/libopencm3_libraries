#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

int main()
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);

    gpio_set_mode(GPIOC,GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO13);

    gpio_set(GPIOB,GPIO_ALL);

    gpio_clear(GPIOC,GPIO13);

    while (1)
    {
		for (size_t i = 0; i < 1000000; i++) {	/* Wait a bit. */
			__asm__("nop");
		}
        gpio_toggle(GPIOC,GPIO13);

    }

    return 0;
}