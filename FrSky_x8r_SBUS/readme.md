This SBUS library is written for a STM32F103C8T6 microcontroller.

## Peripherals used:
- DMA
- USART 1
- Timer 4

### GPIO pins:
- GPIO_USART1_RX

Used for NOT gate:
- 3.3V pin
- G pin

## code:
NOT gate inverts the SBUS signal into USART. DMA is triggered on byte recieved and a DMA interrupt is thrown after all bytes are recieved. Interrupt routine sorts recieved bytes to construct the appropriate channels.


## In case anyone reads this:
I'm not quite sure how to deal with the digital channels 17, 18, and the frame lost / fail safe flags that arrive in bit 17. If you know or have a source, please let me know.
