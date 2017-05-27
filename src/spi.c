#include "spi.h"

// SPI pins
#define CLK PB7
#define MISO PB0
#define MOSI PB1
#define CS PB2

// TODO: the CS pin should not be hard coded; function set_cs_low should
// accept a CS pin parameter

void set_cs_low() {
    PORTB &= ~(_BV(CS));
}

void set_cs_high() {
    PORTB |= _BV(CS);
}

void init_spi(void) {
    /*
     * Before SPI is enabled, the power reduction register must have the SPI bit (the third bit)
     * set to 0.
     */
    // PRR = _BV(2); // because PRRSPI is bit 2 (counting from 0)
    // set SCK, CS and MOSI as output pins
    DDRB |= _BV(CLK) | _BV(CS) | _BV(MOSI);
    // enable SPI, set mode to master, set SCK freq to f_io/64
    SPCR |= _BV(SPE) | _BV(MSTR) | _BV(SPR1);
}

char send_spi(char cmd) {
    // TODO: the slave device data mode must match the configured data mode;
    // this often works with the defaults, not not always.

    SPDR = cmd;
    while (!(SPSR & _BV(SPIF)));

    return SPDR;
}
