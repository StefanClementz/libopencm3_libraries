#include "header.hpp"


static void system_setup()
{
    //Define system clock, 8 mhz external crystal -> 72 MHz on CPU.
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    //Enable GPIOC peripherals
    rcc_periph_clock_enable(RCC_GPIOC);
    //Set GPIOC pin 13 to output in pushpull mode
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    //Setup systick for millis
    systick_setup();

    //setup I2C1 on GPIOB // GPIO6 and GPIO7
    I2C_setup();
}


int main()
{
    system_setup();

    while (true)
    {
        gpio_clear(GPIOC,GPIO13);
        delay(500);
        gpio_set(GPIOC,GPIO13);
        delay(500);
        i2c_write_1(I2C1,8,1);
    }

    return 0;
}
