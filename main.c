/**
 * Chest drain project
 * Jeff Baker
 * Started 8/28/2022
*/

#include "init.h"
#include "measure.h"
#include "process.h"
#include "util.h"
#include "device_config.h"
#include "options.h"
#include "device_config.c"
#include "qtouch.h"

unsigned char p;


char tx_idx;				//index to current byte
char tx_len;				//number of bytes to  transmit
char tx_buf[TX_BUF_LEN];	//TX_BUF_LEN is set in options.h
#ifdef ENABLE_TX
void transmit(void);
#endif
//******************************************************************************
//ISRs
void __interrupt() ISR (void)
{
    // interrupt handler
    if(TMR0IE == 1 && TMR0IF == 1){
        TMR0IF=0;      //clear flag
    }    
#ifdef ENABLE_TX
    if(PIE3bits.TX1IE && PIR3bits.TX1IF)		// Is transmit enabled, and is the buffer empty?
	{
        TX1REG=tx_buf[tx_idx];	  //load buffer with next byte
        tx_idx++;
        if(tx_idx==tx_len){ //is this interrupt the last byte?
            PIE3bits.TX1IE=0;
        }
	}
#endif
}

//******************************************************************************
void main(void)
{
    
    // Initialize the device
    SYSTEM_Initialize();
    IPEN=0; //all interrupts are same priority
    GIE=1;
    PEIE=1;

    //reset the QT
    delay_ms(2500);
    PORTAbits.RA0=0;
    delay_ms(500);
    PORTAbits.RA0=1;
    delay_ms(2500);
    
    while (1)
    {
        while(!TMR0IF);
        TMR0IF=0;

#ifndef ENABLE_TX
        
#endif
        

                
        p = spi_single(0x0F);   //should be non-zero
        //p = spi_single(0x0F);   //should be non-zero
        delay_ms(10); //just to let spi finish
        wiggle();
#ifdef ENABLE_TX
        transmit();
#endif
    }
}

/************************************************************************************
void transmit()
Serial transmission
waits until serial buffers are not in use
stuffs buffers with data
starts up interrupts to send the serial data
*************************************************************************************/	
#ifdef ENABLE_TX
void transmit()
{
    char i,j;
    unsigned char buff[TX_BUF_LEN];
    
	while(PIE3bits.TX1IE)					// Wait for last transmission 
		{
			continue;				//before writing into tx_buf
		}
    buff[0] = HIGH_BYTE(0x55);
	buff[1] = LOW_BYTE(0x55);
//    buff[2] = HIGH_BYTE(bot1);
//    buff[3] = LOW_BYTE(bot1);
//	buff[4] = HIGH_BYTE(top.mean);
//	buff[5] = LOW_BYTE(top.mean);
//	buff[6] = HIGH_BYTE(bot.mean);;
//    buff[7] = LOW_BYTE(bot.mean);
//    buff[8] = HIGH_BYTE(top_background.mean);
//	buff[9] = LOW_BYTE(top_background.mean);
//	buff[10]= HIGH_BYTE(bot_background.mean);
//    buff[11]= LOW_BYTE(bot_background.mean);
//    
//    buff[12] = HIGH_BYTE(dtop);
//	buff[13] = LOW_BYTE(dtop);
//	buff[14]= HIGH_BYTE(dbot);
//    buff[15]= LOW_BYTE(dbot);
//    
//    buff[16] = HIGH_BYTE(d1);
//	buff[17] = LOW_BYTE(d1);
//	buff[18] = HIGH_BYTE(d2);
//    buff[19] = LOW_BYTE(d2);
//    
//    buff[20] = HIGH_BYTE(z);
//	buff[21] = LOW_BYTE(z);
//	buff[22] = HIGH_BYTE(y0);
//    buff[23] = LOW_BYTE(y0);
//    
//    buff[24] = 0;
//    buff[25] = state;
//    
//    buff[26] = 0;
//    buff[27] = state_counter;
    
    tx_len=2; //one more than highest index
  
    //escape any data that is esc or term chars
    j=0;
    for (i=0;i<tx_len;i++){
        if(buff[i]==ESCCHAR || buff[i]==TERMCHAR){
            tx_buf[j]=ESCCHAR;
            tx_buf[j+1]=buff[i]^0x20; //xor with 0x20
            j=j+2;
        }
        else{
            tx_buf[j]=buff[i];
            j=j+1;
        }
    }//end loop over whole buffer

    //add a term
    tx_buf[j]=TERMCHAR;
    tx_len=j+1;
    
	//Enable transmission
	tx_idx=0;
   	PIE3bits.TXIE=1;					//do this last

}//end of transmit();
#endif //ENABLE_TX
/**
 End of File
*/