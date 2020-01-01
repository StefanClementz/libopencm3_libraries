#ifndef I2C_SLAVE_HPP
#define I2C_SLAVE_HPP

extern "C" {
    #include <libopencm3/stm32/rcc.h>
    #include <libopencm3/stm32/gpio.h>
    #include <libopencm3/stm32/i2c.h>
    #include <libopencm3/cm3/nvic.h>
}

void I2C_slave_setup(uint8_t slave_addr);

//extern "C" to prevent compiler from turning function call into mumbo-jumbo
extern "C" void i2c1_ev_isr();

#endif