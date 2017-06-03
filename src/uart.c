#include "uart.h"

void put_char(char);

uint16_t swap( uint16_t val ) {
    return (val << 8) | (val >> 8 );
}

unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void put_char(char c) {
    while (LINSIR & _BV(LBUSY));
    LINDAT = swap(c);
}

void init_uart() {
    // below not strictly necessary
    DDRD |= _BV(UART_TX);
    DDRD &= ~(_BV(UART_RX));

    // LDIV = (F_IO/BAUD_RATE*BIT_SAMPLES) - 1
    int16_t LDIV = 12;
    LINBRRH = LDIV << 8; // top 8 bits
    LINBRRL = LDIV; // bottom 8 bits

    LINBTR = 8; // sample each bit 16 times

    LINCR = _BV(LENA) | _BV(LCMD2) | _BV(LCMD1) | _BV(LCMD0);
    // enable UART, full duples
	//LINENIR = 0b00000001;                     // Set the ISR flags for just the receive
	//LINSIR = 0b00000001;
}

void send_uart(char* msg) {
    for (int i = 0; i < strlen(msg); i++) {
        put_char(msg[i]);
    }
}
