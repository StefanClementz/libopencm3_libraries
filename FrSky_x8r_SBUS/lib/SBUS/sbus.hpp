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


#define SBUS_THR 0
#define SBUS_ROLL 1
#define SBUS_PITCH 2
#define SBUS_YAW 3



void sbus_setup();
void DMA_setup();
void USART_setup();
void TIM4_setup();


// ---------- random crap functions ----------
uint16_t bitseq(int n); // Generate binary sequence of n ones
uint16_t sbus_chn(int chn); // Return sbus channel value

#endif