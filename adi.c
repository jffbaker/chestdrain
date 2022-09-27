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
    static const unsigned int stageBuffer[] = {
    //Stage 0 
    0xFFFE,
    0x5FFF, 
    0x0000, 
    0x2626,
    1600,
    1600,
    1600,
    1600,
    //Stage 1 
    0xFFFB,
    0x5FFF,
    0x0000,
    0x2626,
    1650,
    1650,
    1650,
    1650,
    //Stage 2 
    0xFFEF,
    0x5FFF,
    0x0000,
    0x2626,
    1650,
    1650,
    1650,
    1650,
    //Stage 3 
    0xFFBF,
    0x5FFF,
    0x0,
    0x2626,
    1650,
    1650,
    1650,
    1650,
    //Stage 4 
    0xFEFF,
    0x5FFF,
    0x0,
    0x2626,
    1650,
    1650,
    1650,
    1650,
    //Stage 5 
    0xFBFF,
    0x5FFF,
    0x0,
    0x2626,
    1650,
    1650,
    1650,
    1650,
    //Stage 6 
    0xEFFF,
    0x5FFF,
    0x0,
    0x2626,
    1650,
    1650,
    1650,
    1650,
    //Stage 7 
    0xFFFF,
    0x5FFE,
    0x0,
    0x2626,
    1600,
    1600,
    1600,
    1600,
    //Stage 8 
    0xFFFF,
    0x7FFB,
    0x0,
    0x2626,
    1100,
    1100,
    1150,
    1150,
    //Stage 9 
    0xFFFF,
    0x7FEF,
    0x0,
    0x2626,
    1100,
    1100,
    1150,
    1150,
    //Stage 10 
    0xFFFF,
    0x5FBF,
    0x0,
    0x2626,
    1200,
    1200,
    1300,
    1300,
    //Stage 11 
    0xFFFF,
    0xDEFF,
    0x0,
    0x2626,
    1100,
    1100,
    1150,
    1150};
    
    adi_write_burst(STAGE0_CONNECTION,stageBuffer,96); 
    adi_read_burst(STAGE0_CONNECTION,96);

//    //initialization sequence
//    adi_write_single(0x0000,0b1100000010111100);//full power, decimate 256, active low ints, excitation on, nomral bias.
//    adi_write_single(0x0001,0x00); //all cal disabled
//    adi_write_single(0x0005,0x00); //no low ints, GPIO disabled
//    adi_write_single(0x0006,0x00); //no high ints
//    adi_write_single(0x0007,0x0800);//stage 11 completion interrupt enable; no GPIO int
//   
//    s[0]=0b0000000010110000; //normal bias, exc on, act low ints, 256 decimation, 12 stages, full power mode
//    s[1]=0x00; //all cal disabled, skip modes 00
//    s[2]=0b00; 
//    s[3]=0x00;  // no amb recal?
//    s[4]=0x00;
//    s[5]=0x00; //no low ints
//    s[6]=0x00; //no stage high ints, no gpio int
//    s[7]=0b0000 1000 0000 0000;
    
    //power up init
    adi_write_single(PWR_CONTROL,0x00B0);
    adi_write_single(AMB_COMP_CTRL1,0x0419);
    adi_write_single(AMB_COMP_CTRL2,0x0832);
    adi_write_single(STAGE_LOW_INT_EN,0x0000);
    adi_write_single(STAGE_HIGH_INT_EN,0x0000);
    adi_write_single(STAGE_COMPLETE_INT_EN,0x0800);    
    adi_write_single(STAGE_CAL_EN,0x0);  
}

void adi_start(){
    adi_write_single(0x0000,0b0000000010111100);//full power, decimate 256, active low ints, excitation on, normal bias.
}

void adi_stop(){
    adi_write_single(0x0000,0b0000000010011111);//shutdown, decimate 256, active low ints, excitation on, normal bias.
}

void adi_adjust_pos_offsets(){
    unsigned char i,stage_offset_register,offset;
    unsigned int offset_word;
    for (i=0;i<10;i++){
        stage_offset_register=0x82 + 8*i;
        offset=0;
        adi_buf[10]=i;
        
        IOCAFbits.IOCAF1=0;  //TODO- write this into conversion reader
        adi_read_burst(0x0008,3); //clears INT pin

        while(!IOCAFbits.IOCAF1); //wait for conversion done
        IOCAFbits.IOCAF1=0;
        adi_read_burst(0x0008,3); //clears INT pin
        adi_read_conversions();
        
        while(!IOCAFbits.IOCAF1); //wait for conversion done
        IOCAFbits.IOCAF1=0;
        adi_read_burst(0x0008,3); //clears INT pin
        adi_read_conversions();
        
        
        while (adi_buf[i]>0x7FFF){
            offset++; //TO DO- limit this to 64
            if(offset==64){
                break;
            }
            adi_buf[11]=(unsigned int)offset; //for debugging
            offset_word=((unsigned int)offset)<<8;
            adi_write_single(stage_offset_register,offset_word);
            
            while(!IOCAFbits.IOCAF1); //wait for conversion done
            IOCAFbits.IOCAF1=0;
            adi_read_burst(0x0008,3); //clears INT pin
 
            while(!IOCAFbits.IOCAF1); //wait for 2nd conversion done
            IOCAFbits.IOCAF1=0;
            adi_read_burst(0x0008,3); //clears INT pin
            adi_read_conversions();
        } //end of while this zone is still high
    } //end of loop over all zones
}//end of function

void adi_read_conversions(){
    adi_read_burst(0x000B, 10);
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



