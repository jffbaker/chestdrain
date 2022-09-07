
#include <xc.h>
#include "util.h"
#include "options.h"
#include "qtouch.h"
#include "globals.h"

#define CS_LOW()    LATB2=0
#define CS_HIGH()   LATB2=1
#define DRDY()      PORTB&0b00000010 //B1


void qtouch_init(void){
    //wait for qtouch initial power up
    //delay_ms(120);              
//    while(p!=0xF0){
//        p = spi_single(0x0F);   //should be non-zero
//    }
    
    p=spi_single(0x04);//Force reset
    p=spi_single(0x04);//Force reset
    
    delay_ms(10);
    delay_ms(10);
    
}

/* clocks one byte out while clocking one byte in  */
unsigned char spi_single(unsigned char command){
    unsigned char tempchar;
    
    //wait 110us for DRDY to go low from any previous comms
    delay(157); 
    
    //wait for DRDY to go high
    tempchar=PORTB&0x02;
    while(!tempchar) //wait for DRDY line to be high
    {
        tempchar=PORTB&0x02;
    }
    
    //Transmit byte
    CS_LOW();
    SSP1BUF=command;
    while(BF==0);
    CS_HIGH();
    
    return(SSP1BUF); //Does this read clear BF?
}

void spi_write_burst(unsigned char address,  unsigned char payload[], unsigned char n){
    return;
}

unsigned char spi_read_single(unsigned char command){
    unsigned char p;
    command|=0x80; //set highest bit to 1 for read
    CS_LOW();
    SSP1BUF=command; //send address
    while(BF==0); //wait til done
    p=SSP1BUF; //read to clear BF
    SSP1BUF=command; //send anything to receive byte
    while(BF==0); //wait til done
    CS_HIGH();
    return(SSP1BUF);
}
void spi_read_burst(unsigned char address, unsigned char* payload, unsigned char n){
    unsigned char i,p;
    //write address
    address|=0x80; //set highest bit to 1 for read
    CS_LOW();
    SSP1BUF=address; //send address
    while(BF==0); //wait til done
    p=SSP1BUF; //read to clear BF
    //read data
    for(i = 0; i < n; i++){
        SSP1BUF=address; //send anything to receive byte
        while(BF==0); //wait til done
        payload[i]=SSP1BUF;
    }
    CS_HIGH();
}