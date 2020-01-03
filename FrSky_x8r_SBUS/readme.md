This SBUS library is written for a STM32F103C8T6 microcontroller.

## Peripherals used:
- DMA
- USART 1
- Timer 4

### GPIO pins:
- GPIO_USART1_RX (my muC; GPIOA, pin10)

Used for NOT gate:
- 3.3V pin
- G pin

## code:
NOT gate inverts the SBUS signal into USART. DMA is triggered on byte recieved and interrupt is thrown after all 25 bytes are recieved. 