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
 
}

void adi_write_single(unsigned int address, unsigned int  payload){
    unsigned char tempchar;

    tempchar=0;
    tempchar&=(unsigned char)(address>>8); //two LSB of first byte are two highest bits of address.
    tempchar|=0b11100000; //ADI specified pattern for first byte, R/nW=0 for write
    ADI_CS_LOW();
    SSP1BUF=tempchar; 
    tempchar=LOW_BYTE(address);
    while(BF==0);
    SSP1BUF=tempchar; //send low nibble
    tempchar=HIGH_BYTE(payload);
    while(BF==0);
    SSP1BUF=tempchar; //send high nibble
    tempchar=LOW_BYTE(payload);
    while(BF==0);
    SSP1BUF=tempchar; //send high nibble
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

void adi_write_burst(unsigned int command, unsigned int payload[], unsigned int n){
    
}




