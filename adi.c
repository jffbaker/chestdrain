/*******************************************************************************
 * SPI for ADI 7147
*/
#include <xc.h>
#include "util.h"
#include "options.h"
#include "adi.h"
#include "globals.h"

#define ADI_CS_LOW()    LATB0=0
#define ADI_CS_HIGH()   LATB0=1

void adi_init(void){
    unsigned int s[11]; //all 12 stages have same setting
    unsigned int stage_setup[24];
    s[0]=0b0000000010110000; 
    s[1]=0x00; //all cal disabled, skip modes 00
    s[2]=0b00; 
    s[3]=0x00;  // no amb recal?
    s[4]=0x00;
    s[5]=0x00; //no low ints
    s[6]=0x00; //no stage complete ints, no gpio int
    
    stage_setup[0]=0b0011111111111110; //zone 0 to +; zones 1-6 to bias
    stage_setup[1]=0b1101111111111111; //Pos and Neg offsets disabled; single ended; zones 7-11 to bias
    stage_setup[2]=0x0000; //offsets not specified
    stage_setup[3]=0x0000; //thresholds not specified
    stage_setup[4]=0x0000; //low offset not specified
    stage_setup[5]=0x0000; //high offset not specified
    stage_setup[6]=0xFFFF; //high clamp not specified
    stage_setup[7]=0x0000; //low clamp not specified
    
    
    
    adi_write_burst(0x0080,stage_setup,8);
    adi_read_burst(0x0080,8);
}

void adi_write_single(unsigned int address, unsigned int  payload){
    unsigned char tempchar;

    tempchar=0;
    tempchar&=(unsigned char)(address>>8); //two LSB of first byte are two highest bits of address.
    tempchar|=0b11100000; //ADI specified pattern for first byte, R/nW=0 for write
    ADI_CS_LOW();
    SSP1BUF=tempchar;  //send first byte of command
    tempchar=LOW_BYTE(address);
    while(BF==0);
    SSP1BUF=tempchar; //send second byte of command 
    while(BF==0);
    SSP1BUF=HIGH_BYTE(payload); //send high byte of payload
    while(BF==0);
    SSP1BUF=LOW_BYTE(payload); //send low byte of payload
    while(BF==0);
    ADI_CS_HIGH();
}

unsigned int adi_read_single(unsigned int address){
    unsigned char tempchar,ph,pl;
    ADI_CS_LOW();
    //two LSB of first byte are two highest bits of address.
    tempchar=0;
    tempchar&=(unsigned char)(address>>8);
    tempchar|=0b11100100; //ADI specified pattern for first byte, R/nW=1 for read
    SSP1BUF=tempchar;  //send pattern and two high bits of address
    tempchar=LOW_BYTE(address);
    while(BF==0);
    SSP1BUF=tempchar; //send lower byte of address
    while(BF==0);
    SSP1BUF=0x00; //send junk to clock in high byte of data
    while(BF==0);
    ph=SSP1BUF;
    SSP1BUF=0x00; //send junk to clock in low byte of data
    while(BF==0);
    pl=SSP1BUF;
    ADI_CS_HIGH();
    return( ((unsigned int)ph)<<8 | ((unsigned int)pl) );
}

void adi_write_burst(unsigned int address, unsigned int payload[], unsigned char n){
    unsigned char tempchar;

    //send command word
    tempchar=0;
    tempchar&=(unsigned char)(address>>8); //two LSB of first byte are two highest bits of address.
    tempchar|=0b11100000; //ADI specified pattern for first byte, R/nW=0 for write
    ADI_CS_LOW();
    SSP1BUF=tempchar;  //send first byte of command
    tempchar=LOW_BYTE(address);
    while(BF==0);
    SSP1BUF=tempchar; //send second byte of command 
    while(BF==0);
    
    //loop over all words in payload
    for(tempchar=0;tempchar<n;tempchar++){
        SSP1BUF=HIGH_BYTE(payload[tempchar]); //send high byte of payload
        while(BF==0);
        SSP1BUF=LOW_BYTE(payload[tempchar]); //send low byte of payload
        while(BF==0);    
    }
    
    ADI_CS_HIGH();
}


void adi_read_burst(unsigned int address, unsigned char n){
    //affects global buffer adi_buf[]
    unsigned char tempchar,ph,pl;
    ADI_CS_LOW();
    tempchar=0;
    tempchar&=(unsigned char)(address>>8); //two LSB of first byte are two highest bits of address.
    tempchar|=0b11100100; //ADI specified pattern for first byte, R/nW=1 for read
    SSP1BUF=tempchar;  //send pattern and two high bits of address
    while(BF==0);
    SSP1BUF=LOW_BYTE(address); //send lower byte of address
    while(BF==0);

    for(tempchar=0;tempchar<n;tempchar++){
        SSP1BUF=0x00; //send junk to clock in high byte of data
        while(BF==0);
        ph=SSP1BUF;
        SSP1BUF=0x00; //send junk to clock in low byte of data
        while(BF==0);
        pl=SSP1BUF;
        adi_buf[tempchar]=( ((unsigned int)ph)<<8 | ((unsigned int)pl) );
    }
    ADI_CS_HIGH();
}



