/* 
 * File:   accel.h
 * Author: Jeff
 *
 * Created on April 18, 2021, 8:26 AM
 */

#ifndef QTOUCH_H
#define QTOUCH_H

void qtouch_init(void);
unsigned char spi_single(unsigned char command);
void spi_write_burst(unsigned char address,  unsigned char payload[], unsigned char n);

// call with test= spi_read_single(a,&p); 
unsigned char spi_read_single(unsigned char address);

//call with test=spi_read_burst(a,payload,n);
void spi_read_burst(unsigned char address, unsigned char* payload, unsigned char n);

#endif	/* QTOUCH_H */

