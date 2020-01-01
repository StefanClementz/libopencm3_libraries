#include "sbus.hpp"

static uint32_t DMA_data[13]; //reserve 13x2x2=26x2 (need 25x2) pieces of concurrent memory

static volatile uint8_t head_B; // sbus header byte, expect 0x0F
static volatile uint8_t foot_B; // sbus footer byte, expect 0x00
static volatile uint16_t sbus_chn[16]; //16 channels, 11 bits per channel
static volatile uint16_t byte_17; // for digital channels 17/18, failsafe and lost frame flags


// Sets up the SBUS
void sbus_setup()
{
    // setup basic stuff
    rcc_periph_clock_enable(RCC_USART1); // enable USART
    rcc_periph_clock_enable(RCC_DMA1); // enable DMA
    rcc_periph_clock_enable(RCC_GPIOA); // enable GPIOA
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO_USART1_RX); // enable USART1_RX pin
    rcc_periph_clock_enable(RCC_TIM4); // enable timer 4
    nvic_enable_irq(NVIC_DMA1_CHANNEL5_IRQ); //Enable DMA interrupt in nvic
    nvic_enable_irq(NVIC_TIM4_IRQ); //Enable tim4 interrupt in nvic

    DMA_setup();
    USART_setup();
    TIM4_setup();

    return;
}


// setup DMA, P 279
void DMA_setup()
{
    dma_set_peripheral_address(DMA1,DMA_CHANNEL5,(uint32_t)&USART1_DR);
    dma_set_memory_address(DMA1,DMA_CHANNEL5,(uint32_t)DMA_data);
    dma_set_number_of_data(DMA1,DMA_CHANNEL5,25);
    dma_set_priority(DMA1,DMA_CHANNEL5,DMA_CCR_PL_HIGH);
    dma_set_read_from_peripheral(DMA1,DMA_CHANNEL5);
    dma_enable_circular_mode(DMA1,DMA_CHANNEL5);
    dma_enable_memory_increment_mode(DMA1,DMA_CHANNEL5);
    dma_set_peripheral_size(DMA1,DMA_CHANNEL5,DMA_CCR_PSIZE_16BIT);
    dma_set_memory_size(DMA1,DMA_CHANNEL5,DMA_CCR_MSIZE_16BIT);
    dma_enable_transfer_complete_interrupt(DMA1,DMA_CHANNEL5); // enable interrupt on complete transfer
    dma_enable_channel(DMA1,DMA_CHANNEL5);

    return;
}


// Setup USART, P 796
void USART_setup()
{
    usart_enable(USART1); //enable USART1
    usart_set_databits(USART1,0x09); //9 data bits including parity
    usart_set_stopbits(USART1,USART_STOPBITS_2);
    usart_enable_rx_dma(USART1);
    usart_set_parity(USART1,USART_PARITY_EVEN);
    usart_set_mode(USART1,USART_MODE_RX); // enables recieve mode
    USART1_BRR = 720; //The manual is retarded. BAUD = F_clock / USART_BRR...

    return;
}


void TIM4_setup()
{
    timer_set_mode(TIM4,TIM_CR1_CKD_CK_INT,TIM_CR1_CMS_EDGE,TIM_CR1_DIR_UP);
    timer_set_prescaler(TIM4,71);
    timer_set_period(TIM4,2999);
    timer_one_shot_mode(TIM4);
    timer_enable_preload(TIM4);
    timer_enable_irq(TIM4,TIM_DIER_UIE);
}


volatile int failed = 0;
void dma1_channel5_isr()
{
    dma_clear_interrupt_flags(DMA1,DMA_CHANNEL5,DMA_TCIF);

    head_B = DMA_data[0];
    foot_B = DMA_data[12];

    // Make sure header and footer is received correctly, else turn off usart/reset DMA for 3 ms using timer 4, then restart
    if (head_B!=0x0F || foot_B!=0x00)
    {
        usart_disable(USART1);
        dma_channel_reset(DMA1,DMA_CHANNEL5);
        timer_enable_counter(TIM4);
    }
    else //first and last byte recieved
    {
        int Nb = 11, chn = 0;
        uint8_t DB;
        //Loop through each DMA_byte
        for (int idx=1; idx<23; idx++) //loop from 2 to 24 gives DMA_data[idx], idx = 0,1,1,2,2,3,3,...,10,11
        {
            DB = (DMA_data[idx/2] >> 16*(idx%2)); //shift 16 bits left if idx is odd

            if (Nb > 7)
            {
                // if Nb == 11, clear sbus channel and write
                (Nb == 11) ? sbus_chn[chn] = DB : sbus_chn[chn] |= DB<<(11-Nb);
                Nb = Nb - 8;
                if (Nb == 0)
                {
                    Nb == 11;
                    chn++;
                }
            }
            else
            {
                uint8_t nbits = bitseq(Nb);
                sbus_chn[chn] |= ((DB & nbits) << (11-Nb)); //eats Nb bits, 8-Nb bits left
                chn++;
                sbus_chn[chn] = (DB & (0xFF & ~nbits)); // clear sbus_chn and put in remaining bits
                Nb = 11-(8-Nb);
            }
        }
    }


// Placeholder for fail-safe and stuff if the number of failed bits is too large
/*
    if (failed == 100)
    {

    }
    else
    {

    }
*/
    return;
}


uint16_t bitseq(int n)
{
    uint16_t bitter = 0b1;
    for (int i=1; i < n; i++)
    {
        bitter = (bitter << 1) | 0b1;
    }
    return bitter;
}


void tim4_isr()
{
    timer_clear_flag(TIM4,TIM_SR_UIF);
    usart_enable(USART1); // reenable USART1
    DMA_setup(); // Reset DMA
}