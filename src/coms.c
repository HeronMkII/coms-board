#include <avr/io.h>

#include "spi.h"
#include "trans.h"

int main(void)
{
    init_spi();
    init_cc1120();

    char msg[] = "for the betterment of mankind";
    send(msg, 0xAA, sizeof(msg));

    DDRC |= _BV(PC0);
    PORTC |= _BV(PC0);
}
