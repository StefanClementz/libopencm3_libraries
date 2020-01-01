#include "header.hpp"

void system_setup()
{
    rcc_periph_clock_enable(RCC_GPIOC);

    // Enable on board LED
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    //For millis
    systick_setup();

    gpio_clear(GPIOC,GPIO13);
    delay(200);
    gpio_set(GPIOC,GPIO13);
    delay(500);

    //Setup I2C in slave config
    I2C_slave_setup(0x32);

    return;
}

int main()
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    system_setup();

    while (true)
    {
        delay(100);
    }

    return 0;
}