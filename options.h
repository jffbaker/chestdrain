/* 
 * File:   options.h
 * Author: jeff
 *
 * Created on May 25, 2020, 8:47 AM
 */

#ifndef OPTIONS_H
#define	OPTIONS_H

/****************************************************************
*	MEAUSUREMENT												*
*****************************************************************/
#define ADI_BUF_LEN 128

/****************************************************************
*	MEAUSUREMENT												*
*****************************************************************/
#define MEAS_BUF_LEN (4) //10s
#define YBUF_LEN 128

/****************************************************************
*	SERIAL TRANSMISSION											*
*****************************************************************/
//#define ENABLE_TX               //enable serial TX.  Else, RB2 is degug output.
#define	TX_BUF_LEN	(32)		//length of transmit buffer 
#define TERMCHAR 0x7F
#define ESCCHAR  0x1B 

/****************************************************************
*	FILTER											*
*****************************************************************/
#define BACKGROUND_SHIFT_TIME (2400)            //120 per second; 1200=10s
#define BACKGROUND_SHIFT_THRESHOLD (300)      
#define ONE (0xFFFFFFFF)

#endif	/* OPTIONS_H */

