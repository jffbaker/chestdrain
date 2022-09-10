/* 
 * File:   globals.h
 * Author: jeff
 *
 * Created on May 25, 2020, 8:43 AM
 */

#ifndef GLOBALS_H
#define	GLOBALS_H

/*  globals.h

	includes all external declarations for global variables
*/
#include "options.h"

extern unsigned char button_buffer;
extern unsigned char fault_code;

//For serial transmission, usually debug
extern 	char		tx_idx;				//index to current byte
extern	char		tx_len;				//number of bytes to  transmit
extern	char		tx_buf[TX_BUF_LEN];	//TX_BUF_LEN is set in options.h	

//For Newhaven display
extern unsigned char row_address[4];
extern unsigned char row_bit;
extern unsigned char cursor_row, cursor_col;

extern unsigned int adi_buffer[ADI_BUF_LEN];


//flags
typedef union{
	unsigned char byte;
	struct{
	unsigned  char  button_pressed:1;  //button is pressed and needs handling			
	unsigned  char  scrolling:1;  		//a scrolling message is active
	unsigned  char  bit2:1;  
	unsigned  char  bit3:1;  
	unsigned  char  bit4:1;  
	unsigned  char  bit5:1; 
	unsigned  char  bit6:1;  
	unsigned  char  fault:1;	
	}bits;
}flag_t;

extern flag_t flag;


extern unsigned char p;

extern unsigned char data_ready;


#define LOW_BYTE(x)     ((unsigned char)((x)&0xFF))
#define HIGH_BYTE(x)    ((unsigned char)(((x)>>8)&0xFF))

#define BYTE1(x)     (unsigned char)((top0    )&0xFF)
#define BYTE2(x)     (unsigned char)((top0>>8 )&0xFF)
#define BYTE3(x)     (unsigned char)((top0>>16)&0xFF)
#define BYTE4(x)     (unsigned char)((top0>>24)&0xFF)


#endif	/* GLOBALS_H */

