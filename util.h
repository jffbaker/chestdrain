/* 
 * File:   util.h
 * Author: jeff
 *
 * Created on May 25, 2020, 12:21 PM
 */

#ifndef UTIL_H
#define	UTIL_H

    

/*Faults
 */
#define FAULT_SPI_BF    0x01


/****************************************************************
*	fract_mul(A,B)												*
*****************************************************************
	Multiply two unsigned ints A*B in binary fixed point
	In this number system 0xFFFF is 1 and 0x0000 is 0.  This 
	multiplication gives FFFF*FFFF=FFFF, or 1*1=1
        Entry:  A.value, A.leftover, B
	    Uses:   
	    Exit:   A.value and A.leftover
*/
unsigned long fract_mul(unsigned long A, unsigned long B);

void delay(int j);
void delay_ms(int j);
void wiggle(void);
void wiggle_high(void);
void wiggle_low(void);

unsigned long abs_long(signed long value);

unsigned int abs_int(signed int value);
unsigned char abs_char(signed char value);

signed int max_int(signed int a, signed int b);
signed int min_int(signed int a, signed int b);
unsigned char array_max_int(signed int a[], unsigned char size);
unsigned char array_min_int(signed int a[], unsigned char size);

void wait_for_BF(void);

unsigned int read_tmr5(void);
void write_tmr5(unsigned int timerVal);


#endif	/* UTIL_H */

