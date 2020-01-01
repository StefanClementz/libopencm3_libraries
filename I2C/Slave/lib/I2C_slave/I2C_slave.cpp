#include "I2C_slave.hpp"

volatile uint8_t buf[128];
volatile uint8_t bufsize = 0;
volatile uint8_t N_written_bytes = 0;


void I2C_slave_setup(uint8_t slave_addr)
{
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_I2C1); //enable I2C

    gpio_set_mode(GPIOB,GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,GPIO_I2C1_SCL);
    gpio_set_mode(GPIOB,GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,GPIO_I2C1_SDA);

    //enable interrupts on I2C1 in general
    nvic_enable_irq(NVIC_I2C1_EV_IRQ);

    // reset and disable peripheral for settings change
    i2c_reset(I2C1);
    i2c_peripheral_disable(I2C1);
    i2c_set_clock_frequency(I2C1,I2C_CR2_FREQ_36MHZ);
    i2c_set_own_7bit_slave_address(I2C1,slave_addr);
    i2c_enable_interrupt(I2C1,I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);

    //Enable peripheral again
    i2c_peripheral_enable(I2C1);

    //Enables ack because slaves bows to their masters (must be set after enabling...? Why?)
    i2c_enable_ack(I2C1);
}


extern "C" void i2c1_ev_isr(void)
{
    //EV1: ADDR=1, Clear by reading SR1 and subsequently SR2.
    if (I2C_SR1(I2C1) & I2C_SR1_ADDR)
    {
        I2C_SR2(I2C1);
    }
    //EV2: RxNE=1, throws away any bytes sent after buffer is full
    else if (I2C_SR1(I2C1) & I2C_SR1_RxNE)
    {
        if (bufsize < 128)
        {
            buf[bufsize] = i2c_get_data(I2C1);
            bufsize++;
        }
    }
    //EV3 cases:
    else if (I2C_SR1(I2C1) & I2C_SR1_TxE)
    {
        //EV3-2: TxE = 1 & AF=1, clear by writing 0 in AF register
        if (I2C_SR1(I2C1) & I2C_SR1_AF)
        {
            if (N_written_bytes < bufsize)
            {
                i2c_send_data(I2C1,buf[N_written_bytes]);
            }
            else
            {
                i2c_send_data(I2C1,0x00);
            }
            N_written_bytes++;
            I2C_SR1(I2C1) &= ~I2C_SR1_AF;
        }
        //EV3 & EV3-1: TxE=1 (EV 3-1: & no data in shift register). Sends when data is placed into DR register in both cases
        else
        {
            if (N_written_bytes < bufsize)
            {
                i2c_send_data(I2C1,buf[N_written_bytes]);
            }
            else
            {
                i2c_send_data(I2C1,0x00);
            }
            N_written_bytes++;
        }
    }
    //EV4: STOPF=1, cleared by writing anything to CR1.
    else if (I2C_SR1(I2C1) & I2C_SR1_STOPF)
    {
        i2c_peripheral_enable(I2C1); //writes last bit in CR
    }
}