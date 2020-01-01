#ifndef MILLIS_HPP
#define MILLIS_HPP

#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>

extern "C" {
    void sys_tick_handler();
}

void systick_setup();

uint64_t millis();

void delay(uint64_t delay_time);

#endif