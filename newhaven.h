/* 
 * File:   accel.h
 * Author: Jeff
 *
 * Created on April 18, 2021, 8:26 AM
 */

#ifndef NEWHAVEN_H
#define NEWHAVEN_H

#define ROWS 4
#define COLS 20

void nhd_init(void);
void nhd_splash_screen(void);
void nhd_scroll_start(void);
unsigned char nhd_command(unsigned char byte);
unsigned char nhd_data(unsigned char byte);
void nhd_setCursor(unsigned char col, unsigned char row);
void nhd_writeChar(unsigned char c, unsigned char col, unsigned char row);
void nhd_writeMessage(unsigned char m[], unsigned char length, unsigned char row, unsigned char col);
void nhd_whiteScreen(void);
unsigned char reverse_byte(unsigned char x);


void spi_write_burst(unsigned char address,  unsigned char payload[], unsigned char n);

// call with test= spi_read_single(a,&p); 
unsigned char spi_read_single(unsigned char address);

//call with test=spi_read_burst(a,payload,n);
void spi_read_burst(unsigned char address, unsigned char* payload, unsigned char n);

#endif	/* NEWHAVEN_H */

