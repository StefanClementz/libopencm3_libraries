#include "header.hpp"


int main()
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    sbus_setup();

    while (true)
    {
        
    }
    return 0;
}