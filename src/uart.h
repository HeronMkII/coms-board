#include <avr/io.h>
#include <string.h>

#define F_IO 8000000 // 8 MHz

#define BAUD_RATE 9600
#define BIT_SAMPLES 16

#define UART_TX PD3
#define UART_RX PD4

void put_char(char);
void init_uart();
void send_uart(char *);
