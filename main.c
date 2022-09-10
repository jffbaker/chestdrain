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
#include "newhaven.h"
#include "adi.h"

unsigned char p;
unsigned char button_buffer;
unsigned char fault_code;

unsigned int adi_buf[ADI_BUF_LEN];

char tx_idx;				//index to current byte
char tx_len;				//number of bytes to  transmit
char tx_buf[TX_BUF_LEN];	//TX_BUF_LEN is set in options.h

void check_butt(); //check button and affect flag

flag_t flag;

unsigned int x;

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
        //wiggle();
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

    nhd_init(); //display

    //    adi_init(); //cap sensing chip
    
    //initialize variables;
    flag.byte=0x00;
    button_buffer=0x00;
    

    //nhd_whiteScreen();
      nhd_splash_screen();

    //adi_write_single(0x0007,0x0AAA);
    //delay_ms(1000);
    //x=adi_read_single(0x0017);

    adi_init();  
      
    while (1)
    {
        while(!TMR6IF);
        TMR6IF=0;
        check_butt();

#ifndef ENABLE_TX
        //transmit();
#endif
    }
}

/*****************************************************************************************
 *Function Name:check_butt
 *
 *Description:  Checks if button is pressed.  Stores button presses in bits of 
 * button_buffer.  Checks to see if button_buffer contains a press and release.
 *
 * Affects: flags.bits.button_pressed
 * Returns:  nothing
 *
****************************************************************************************/
void check_butt() 
{
    unsigned char temp;

    temp=PORTA&0x01; //low if pressed 

    button_buffer=button_buffer<<1;
    
    if(!temp){
        button_buffer|=0x01;
    }

    if((button_buffer&0x0F) == 0b00001100){
        flag.bits.button_pressed=1;
        button_buffer=0x00;
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