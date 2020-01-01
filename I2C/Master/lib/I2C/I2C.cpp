#include "I2C.hpp"

void I2C_setup()
{
    // ======================================  setup i2c  ======================================
    /* Enable clocks for I2C2 and AFIO. */
    rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_I2C1); //enable I2C1

	// Alternate function setup for pins PB6 (I2C_SCL) PB7 (I2C_SDA)
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO6);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO7);

	//Disable to allow modifications
	i2c_peripheral_disable(I2C1);
    i2c_reset(I2C1);
    i2c_set_clock_frequency(I2C1,I2C_CR2_FREQ_36MHZ);
    
    // ======================================  100 kHz  ======================================
/*
    i2c_set_standard_mode(I2C1);
    i2c_set_ccr(I2C1,180);
    i2c_set_trise(I2C1,37);
    i2c_set_dutycycle(I2C1,I2C_CCR_DUTY_DIV2);
*/
    // ======================================  400 kHz  ======================================
    i2c_set_fast_mode(I2C1);
    //Datasheet suggests 0x801e for I2C fast mode
	i2c_set_ccr(I2C1, 0x1e);
	//Maximum rise time: 300 ns, cycle time at 36 MHz; rise time in 10 cycles. i2c_set_trise suggests 10 + 1 (0x0b) cycles
	i2c_set_trise(I2C1, 0x0b);
    //i2c_set_dutycycle(I2C1,I2C_CCR_DUTY_16_DIV_9); This should be incompatible with CCR = 0x1e...

	//Assume everything is configured.
	i2c_peripheral_enable(I2C1);
}


//Writes to a slave register address
void i2c_reg_write(uint32_t i2c, uint8_t addr, uint8_t reg_addr, uint8_t setting)
{
    //Wait for line to get unbusy
    while (I2C_SR2(i2c) & I2C_SR2_BUSY) {}

    //Start a send session
    i2c_send_start(i2c);

    // Wait until bus is busy and microcontroller is in send mode
    while (!((I2C_SR1(i2c) & I2C_SR1_SB) & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

    i2c_send_7bit_address(i2c,addr,I2C_WRITE);

    //Wait for slave to acknowledge its existance?
    while (!( I2C_SR1(i2c) & I2C_SR1_ADDR )) {}

    //Remember to clear addr for later by reading SR_2
    (void)I2C_SR2(i2c);

    //Find reg_addr for modification in next write
    i2c_send_data(i2c,reg_addr);

    //Wait until ack is returned
    while(!(I2C_SR1(i2c) & I2C_SR1_BTF));

    //Find reg_addr for modification in next write
    i2c_send_data(i2c,setting);

    //Wait until ack is returned
    while(!(I2C_SR1(i2c) & I2C_SR1_BTF));

    // Release the bus
    i2c_send_stop(i2c);
}


//Writes a single byte into I2C1
void i2c_write_1(uint32_t i2c, uint8_t slave_addr, uint8_t data_to_transmit)
{
    while (I2C_SR2(i2c) & I2C_SR2_BUSY) {};

    i2c_send_start(i2c);

    while (!((I2C_SR1(i2c) & I2C_SR1_SB) && (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY)))) {}

    //EV5: SB=1, clear by reading SR1 and writing address of target to DR
    I2C_SR1(i2c);
    
    i2c_send_7bit_address(i2c,slave_addr,I2C_WRITE);

    //EV6: ADDR=1, clear by reading SR1 and then reading SR2
    while (!(I2C_SR1(i2c) & I2C_SR1_ADDR)) {}
    (void) I2C_SR2(i2c);

    //EV8_1: TxE=1 & shift register empty, clear by writing to DR
    i2c_send_data(i2c,data_to_transmit);

    i2c_send_stop(i2c);

    while (!(I2C_SR1(i2c) & I2C_SR1_BTF)) {}
}


// Writes n bytes of data defined in the array data
void i2c_write(uint32_t i2c, uint8_t addr, char * data, size_t n)
{

    //Wait if signal is broadcasted on I2C
    while (I2C_SR2(i2c) & I2C_SR2_BUSY) {}

    //Start sending
    i2c_send_start(i2c);

    //Wait for send condition to set in and the bus to be busy or master
    // Why not & here: (I2C_SR2_MSL | I2C_SR2_BUSY)?
    while (!((I2C_SR1(i2c) & I2C_SR1_SB) && (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY)))) {}
    
    //Contact sensor with 7 bit address addr
    i2c_send_7bit_address(i2c,addr,I2C_WRITE);

    //Wait for slave to acknowledge its existance?
    while (!( I2C_SR1(i2c) & I2C_SR1_ADDR )) {}

    //Remember to clear addr for later by reading SR_2
    (void)I2C_SR2(i2c);

    for (size_t i = 0; i < n; i++)
    {
        // Send byte i
        i2c_send_data(i2c,data[i]);

        // wait for acknowledgement that byte was recieved
        while (!(I2C_SR1(i2c) & I2C_SR1_BTF)) {}
    }

    // Release the bus
    i2c_send_stop(i2c);

    return;
}



// Reads n consecutive bytes of data
void i2c_read(uint32_t i2c, uint8_t addr, uint8_t *data, size_t n)
{
    // Wait for bus to unbusy itself
    while (I2C_SR2(i2c) & I2C_SR2_BUSY) {}

    // Start sending
    i2c_send_start(i2c);
    i2c_enable_ack(i2c); //Need ack for sensor to know byte was recieved!

    //Wait for send to set in and bus to become busy
    while (!((I2C_SR1(i2c) & I2C_SR1_SB) && (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY)))) {}

    //Contact object with 7 bit address addr and notify read
    i2c_send_7bit_address(i2c,addr,I2C_READ);

    //Wait for slave to acknowledge its existance?
    while (!( I2C_SR1(i2c) & I2C_SR1_ADDR )) {}

    //Remember to clear addr for later by reading SR2
    (void)I2C_SR2(i2c);

    for (size_t i = 0; i < n; i++)
    {
        //Get byte i
        data[i] = i2c_get_data(i2c);

        // Wait for more data to be placed in the I2C_DR register.
        while (!(I2C_SR1(i2c) & I2C_SR1_RxNE));
    }
    i2c_disable_ack(i2c);

    return;
}