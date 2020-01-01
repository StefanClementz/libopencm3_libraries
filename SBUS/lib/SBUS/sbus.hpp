#ifndef SBUS_HPP
#define SBUS_HPP

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/timer.h>

extern "C"
{
    #include <libopencm3/cm3/nvic.h>
}


void sbus_setup();
void DMA_setup();
void USART_setup();
void TIM4_setup();


// ---------- random crap functions ----------
uint16_t bitseq(int n); //Generate n ones binary sequence


#endif