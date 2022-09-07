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

//For serial transmission, usually debug
extern 	char		tx_idx;				//index to current byte
extern	char		tx_len;				//number of bytes to  transmit
extern	char		tx_buf[TX_BUF_LEN];	//TX_BUF_LEN is set in options.h	

//For Newhaven display
extern unsigned char row_address[4];
extern unsigned char row_bit;
extern unsigned char cursor_row, cursor_col;

extern unsigned char p;

extern unsigned char data_ready;


#define LOW_BYTE(x)     ((unsigned char)((x)&0xFF))
#define HIGH_BYTE(x)    ((unsigned char)(((x)>>8)&0xFF))

#define BYTE1(x)     (unsigned char)((top0    )&0xFF)
#define BYTE2(x)     (unsigned char)((top0>>8 )&0xFF)
#define BYTE3(x)     (unsigned char)((top0>>16)&0xFF)
#define BYTE4(x)     (unsigned char)((top0>>24)&0xFF)


#endif	/* GLOBALS_H */

