/**
 * Chest drain project
 * Jeff Baker
 * Started 8/28/2022
*/
#include <stdio.h>
#include "globals.h"
#include "init.h"
#include "measure.h"
#include "process.h"
#include "util.h"
#include "device_config.h"
#include "options.h"
#include "device_config.c"
#include "newhaven.h"
#include "adi.h"

unsigned int tempint;

unsigned char p;
unsigned char button_buffer;
unsigned char fault_timer,fault_code;

unsigned int adi_buff[ADI_BUF_LEN];
unsigned int c[3][13]; //3 columns, 13 zones each
unsigned char pos_offset[12];
unsigned char differential_pos_offset[12];
unsigned char differential_neg_offset[12];

#if defined(TRANSMIT_ENABLE) || defined(TRANSMIT_STRING_ENABLE)
unsigned char tx_idx;
unsigned char tx_len;
unsigned char tx_buff[TX_BUF_LEN];		//tx_len is set in options.h
#endif

void check_butt(void); //check button and affect flag
void transmit(void);
void transmit_string(void);

flag_t flag;

unsigned int x;

//******************************************************************************
//ISRs
void __interrupt() ISR (void)
{
    // interrupt handler
    if(TMR0IE == 1 && TMR0IF == 1){
        TMR0IF=0;      //clear flag
        //wiggle();
    }
            
    if(PIE3bits.TX1IE && PIR3bits.TX1IF)		// Is transmit enabled, and is the buffer empty?
	{
        TX1REG=tx_buff[tx_idx];	  //load buffer with next byte
        tx_idx++;
        if(tx_idx==tx_len){ //is this interrupt the last byte?
            PIE3bits.TX1IE=0;
        }
	}
}

//******************************************************************************
void main(void)
{
    
    // Initialize the device
    SYSTEM_Initialize();
    IPEN=0; //all interrupts are same priority
    GIE=1;
    PEIE=1;

    //nhd_init(); //display

    //    adi_init(); //cap sensing chip
    
    //initialize variables;
    flag.byte=0x00;
    button_buffer=0x00;
    

    //nhd_whiteScreen();
    //nhd_splash_screen();

//    adi_write_single(0x0007,0x1234);
//    x=adi_read_single(0x0007);


//    adi_setup_conversions(single_ended_setup);
    adi_setup_conversions(differential_setup);
    adi_init();  
    adi_start();
    //adi_adjust_pos_offsets();
    adi_adjust_differential_offsets();
    
    //adi_adjust_AF
    //adi_start();
    
//    unsigned char counter,tempchar;
//    counter=0; 
    
//    unsigned char tempchar;
//    tempchar=0;
//    while(1){
//        tempchar=PORTAbits.RA1;
//        if(!tempchar){
//            adi_read_burst(0x0008,3); //clears INT pin
//        }
//    }
    
//    IOCAFbits.IOCAF1=0;
//    adi_read_burst(0x0008,3); //clears INT pin
//    while(1){
//        if(IOCAFbits.IOCAF1){
//            IOCAFbits.IOCAF1=0;
//            adi_read_burst(0x0008,3); //clears INT pin
//            adi_read_conversions();
//            transmit_string();
//            wiggle();
//        }
//    }
//    
    //tempint=adi_read_single(0x0007);
    
//    while(1)
//    {
//        //tempint=adi_read_single(0x0008);
//        //tempint=adi_read_single(0x0009);
//        tempint=adi_read_single(0x00A0);
//        
//        adi_read_conversions();
//        //tempint=adi_read_single(0x00A0);
//        //if(tempint==0x0800){
//        //    wiggle();
//        //}
//            
//    }

    while (1)
    {
           //TO Do- put button on an IOC ISR so it does not have to be polled
            //and we can sleep
        
        //change transmit to binary and graph in labview
        //add a low pass filter?  Or just average ?
        //add a "mark" button to labview
        // add accelerometer
        // 
        
        //TMR6IF=0;
         //check_butt();

        while(!TMR0IF);
        TMR0IF=0;
        measure();
        //process();
        transmit();
        //display();

//        counter++;
//        if(4==counter){
//            counter=0;
//            adi_read_conversions();
//            tempint=adi_read_single(0x00A0);
//        } //end every 100ms
////    
//        if(IOCAFbits.IOCAF1){
//           //adi_stop();
//           adi_read_conversions();
//           tempint=adi_read_single(0x00A0);
//           IOCAFbits.IOCAF1=0;
//       } 

        
#ifndef ENABLE_TX
        //transmit();
#endif
    }//end while 1
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
#ifdef TRANSMIT_ENABLE
void transmit()
{
    char i,j;
    unsigned char buff[TX_BUF_LEN];
    
	while(PIE3bits.TX1IE)					// Wait for last transmission 
		{
			continue;				//before writing into tx_buf
		}
    for (i=0;i<13;i++){
        buff[2*i] = HIGH_BYTE(adi_buff[i]);
        buff[2*i+1] = LOW_BYTE(adi_buff[i]);
    }
    tx_len=28; //one more than highest index
  
    //escape any data that is esc or term chars
    j=0;
    for (i=0;i<tx_len;i++){
        if(buff[i]==ESCCHAR || buff[i]==TERMCHAR){
            tx_buff[j]=ESCCHAR;
            tx_buff[j+1]=buff[i]^0x20; //xor with 0x20
            j=j+2;
        }
        else{
            tx_buff[j]=buff[i];
            j=j+1;
        }
    }//end loop over whole buffer

    //add a term
    tx_buff[j]=TERMCHAR;
    tx_len=j+1;
    
	//Enable transmission
	tx_idx=0;
   	TX1IE=1;					//do this last

}//end of transmit();
#endif //TRANSMIT_ENABLE


/************************************************************************************
void transmit_string()
 * uses sprintf to put data into a string array, then queus string array for transmission
starts up interrupts to send the serial data
*************************************************************************************/	
void transmit_string()
{
#ifdef TRANSMIT_STRING_ENABLE   
    unsigned char i;
	while(PIE3bits.TX1IE)					// Wait for last transmission 
		{
			continue;				//before writing into tx_buf
		}
//    tx_len=sprintf(tx_buff,"%6d\t%6d\t%6d\t%6d.%3s\t%6d\t%6u\n",
//            ax,
//            ay,
//            az,
//            t_whole,
//            t_frac_string,
//            button,
//            vbat);
    
    tx_len=0;
    for(i=0;i<9;i++){
        tx_len+=sprintf(tx_buff+tx_len,"%u\t",adi_buff[i]);
    }
    //now add the last one
    tx_len+=sprintf(tx_buff+tx_len,"%u\n\r",adi_buff[9]);
    
    //Enable transmission
	tx_idx=0;			
	TX1IE=1;
#endif
}



/**
 End of File
 */