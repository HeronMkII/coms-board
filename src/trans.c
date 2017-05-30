#include "trans.h"

// CC1120 SPI chip select settings
#define CS_PIN PB2
#define CS_PORT PORTB
#define CS_DDR_PORT DDRB

#define DEVICE_ADDRESS 0xAA

char cmd(char);
void write(char, char);
void write_extended(char, char);

void set_cs_low_cc1120();
void set_cs_high_cc1120();

void init_cs_cc1120() {
    init_cs(CS_PIN, &CS_DDR_PORT);
}

void set_cs_low_cc1120() {
    set_cs_low(CS_PIN, &CS_PORT);
}

void set_cs_high_cc1120() {
    set_cs_high(CS_PIN, &CS_PORT);
}

char cmd(char data) {
    set_cs_low_cc1120();
    char ret = send_spi(data);
    set_cs_high_cc1120();
    return ret;
}

void write(char addr, char data) {
    set_cs_low_cc1120();
    send_spi(addr);
    send_spi(data);
    set_cs_high_cc1120();
}

void write_extended(char addr, char data) {
    set_cs_low_cc1120();
    send_spi(0x2F); // required before writing to extended registers
    send_spi(addr);
    send_spi(data);
    set_cs_high_cc1120();
}

void init_cc1120(void) {
    init_spi();
    init_cs_cc1120();

    cmd(SRES); // reset
    _delay_ms(1); // TODO: figure out why these delays are necessary

    cmd(SIDLE); // enter IDLE state
    cmd(SFRX); // flush RX
    cmd(SFTX); // flush TX

	write(SYNC3, 0x93);	//SYNC3: 0x93            Set SYNC word bits 31:24
	write(SYNC2, 0x0B);	//SYNC2: 0x0B            Set SYNC word bits 23:16
	write(SYNC1, 0x51);	//SYNC1: 0x51            Set SYNC word bits 15:8
	write(SYNC0, 0xDE);	//SYNC0: 0xDE            Set SYNC word bits 7:0
	// should tune SYNC_THR (default 0x0A = 10; allows for 5 errors in sync (bad!))
    // otherwise, SYNC_CFG defaults are good

    // set TOC_LIMIT to 0
    // set normal mode/FIFO mode of operation
    // look into RX filter BW settings? not larger than ~40 kHz?

    write(DEVIATION_M, 0x40);
    // set DEV_M to 64; along with DEV_E = 5, sets freq. dev to ~ 20kHz
    write(MODCFG_DEV_E, 0b00001101);
	// use 2-GFSK modulation scheme

	write(PREAMBLE_CFG1, 0x14);
	// use 3 byte preamble, with preamble word 0xAA
	write(PREAMBLE_CFG0, 0x25);
	// enable preanble detection, set PQT timeout to 16 symbols and
	// set PQT threshold to 5 errors

	write(SYMBOL_RATE2, 0x01);
	write(SYMBOL_RATE1, 0x42);
	write(SYMBOL_RATE0, 0x1F);
	// set the symbol rate and exponent such that we transmit at 9600 bps
    //
    write(FS_CFG, 0b00000100);
    // set freq. band to 410 - 480 MHz

	write(PKT_CFG2, 0b00000000);
    //PKT_CFG2: 0x00; set FIFO mode, set CCA to not discriminate
	write(PKT_CFG1, 0b00110000);
    //PKT_CFG1: 0x30; do no whiten data; check addr in RX and broadcast 0x00
    // and 0xFF; do not CRC, do not byte swap, do not append status (might be useful)
	write(PKT_CFG0, 0b00100000);
    // set var length packet mode

	write(RFEND_CFG1, 0b00101111);
    // disable RX timer; go to TX after receiving a good packet
	write(RFEND_CFG0, 0b00000000);
    // enter IDLE after sending a good packet; ignore bad packets;
    // no antenna diversity

	write(PA_CFG2, 0x3F);
    // set power ramp to 63; results in 14 dB output power

    write(PKT_LEN, 0xFF);
    // set max packet length to 255; perhaps should set to 128?

    // review TOC_CFG values; defaults seem good

	//frequency offset setting; i.e. none
	write_extended(FREQOFF1, 0x00);
	write_extended(FREQOFF0, 0x00);

	//Frequency setting
    //set freq. to 434 MHz.
	write_extended(FREQ2, 0x6C);
	write_extended(FREQ1, 0x80);
	write_extended(FREQ0, 0x00);

	write(DEV_ADDR, DEVICE_ADDRESS);
    // set the device address for RX addr checking

    // perform calibration
    cmd(SCAL); // freq synth
    _delay_ms(250);

    cmd(SAFC); // automatic freq control
    _delay_ms(250);

	cmd(SIDLE);
}

void fifo_write(char addr, char data) {
	set_cs_low_cc1120();
	send_spi(0x3E);
	send_spi(addr);
	send_spi(data);
	set_cs_high_cc1120();
}

void send(char* msg, char addr, uint8_t length)
{
	cmd(SIDLE);
	cmd(SFTX); // flux TX FIFO

	fifo_write(0x00, length + 1); // + 2? changed to + 1
	fifo_write(0x01, addr);

	for (int i = 0; i < length; i++) {
		fifo_write(i + 2, msg[i]);
	}

	write_extended(TXFIRST, 0x00);
	write_extended(TXLAST, length + 3);

	cmd(STX); // begin transmission
    // check to make sure there are no errors
}


