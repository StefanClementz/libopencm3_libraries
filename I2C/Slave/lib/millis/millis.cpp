#include "millis.hpp"

// Storage for our monotonic system clock.
// Note that it needs to be volatile since we're modifying it from an interrupt.
static volatile uint64_t _millis = 0;

void systick_setup()
{
    //Set clock source to use CPU clock source
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);

    //Set timer frequency to interrupt ~every 1 millisecond
    systick_set_frequency(1000,rcc_ahb_frequency);

    //Clear systick
    systick_clear();

    //Enable interrupt
    systick_interrupt_enable();

    //Start running counter
    systick_counter_enable();
}

//This is fine because it takes many lifetimes to reach overflow in a uint64_t
void sys_tick_handler()
{
    _millis++;
}

uint64_t millis()
{
    return _millis;
}

void delay(uint64_t delay_time)
{
    const uint64_t time_to_stop = millis()+delay_time;
    while (millis() < time_to_stop){};
}