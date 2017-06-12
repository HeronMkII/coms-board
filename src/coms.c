#include <avr/io.h>

#include "spi.h"
#include "uart.h"
#include "trans.h"
#include "log.h"

int main(void) {
    init_uart();

    char msg[] = "the quick brown fox jumped over the lazy dog\n";

    for (;;) {
        print("%s", msg);
        _delay_ms(100);
    }
}
