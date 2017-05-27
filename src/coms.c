#include <avr/io.h>

#include "spi.h"
#include "trans.h"

int main(void)
{
    init_spi();
    init_cc1120();

    set_cs_low();
    send_spi(SNOP); // nop - returns status byte
    set_cs_high();

    DDRC |= _BV(PC0);
    PORTC |= _BV(PC0);
}
