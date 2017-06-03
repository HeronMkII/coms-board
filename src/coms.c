#include <avr/io.h>

#include "spi.h"
#include "uart.h"
#include "trans.h"

int main(void) {
    init_spi();
    init_uart();
    //init_cc1120();

    char msg[] = "the quick brown fox jumped over the lazy dog";
    //send(msg, 0xAA, sizeof(msg));

    for (int i = 0; i < 10000; i++) {
        send_uart(msg);
    }

    DDRC |= _BV(PC0);
    PORTC |= _BV(PC0);
}
