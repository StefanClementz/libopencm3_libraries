#include "header.hpp"


int main()
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    sbus_setup();

    volatile uint16_t thr;
    volatile uint16_t roll;
    volatile uint16_t pitch;
    volatile uint16_t yaw;

    while (true)
    {
        thr = sbus_chn(SBUS_THR);
        roll = sbus_chn(SBUS_ROLL);
        pitch = sbus_chn(SBUS_PITCH);
        yaw = sbus_chn(SBUS_YAW);



    }
    return 0;
}