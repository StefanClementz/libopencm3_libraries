#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>

void I2C_setup();

void i2c_reg_write(uint32_t i2c, uint8_t addr, uint8_t reg_addr, uint8_t setting);
void i2c_read(uint32_t i2c, uint8_t addr, uint8_t *data, size_t n);

void i2c_write_1(uint32_t i2c, uint8_t slave_addr, uint8_t data_to_transmit);
void i2c_write(uint32_t i2c, uint8_t addr, char * data, size_t n);