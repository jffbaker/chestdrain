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
    //power up init
    adi_stop();
    adi_write_single(STAGE_CAL_EN,0x0000);  
    adi_write_single(AMB_COMP_CTRL1,0x0419);
    adi_write_single(AMB_COMP_CTRL2,0x0832);
    adi_write_single(STAGE_LOW_INT_EN,0x0000);
    adi_write_single(STAGE_HIGH_INT_EN,0x0000);
//    adi_write_single(STAGE_COMPLETE_INT_EN,0x0200);    //int only on stage 9 complete!
//    adi_write_single(STAGE_COMPLETE_INT_EN,0x0080);    //int only on stage 7 complete!
    adi_write_single(STAGE_COMPLETE_INT_EN,0x0800);    //int only on stage 12 complete!
}

void adi_start(){
    adi_write_single(PWR_CONTROL,0b0000000010111100);//full power, decimate 256, active low ints, excitation on, normal bias, 12 stages
}

void adi_stop(){
    adi_write_single(PWR_CONTROL,0b0000000010111111);//shutdown, decimate 256, active low ints, excitation on, normal bias, 12 stages
}

void adi_clear_int(){
    IOCAFbits.IOCAF1=0;
    adi_read_burst(0x0008,3); //clears INT pin
}

void adi_wait_for_int(){
    unsigned char fault_timer=0;
    while(!IOCAFbits.IOCAF1){
        fault_timer++;
        delay_ms(1);
        if(fault_timer>50){
            fault_code=FAULT_ADI_NO_INT;
            flag.bits.fault=1;
            break;
        }
    }
    return;
}

void adi_adjust_pos_offsets(){  //TO DO- fine tune the step sizes
    unsigned int i;
    unsigned int stage_offset_register;
    unsigned int offset_word;

    //get initial values of all zones
    adi_clear_int();
    adi_wait_for_int();
    adi_clear_int();
    adi_wait_for_int();
    adi_read_conversions();
 
    for (i=0;i<12;i++){
        stage_offset_register=0x82 + 8*i;
        pos_offset[i]=0;
        
        while (adi_buff[i]>0x7FFF & pos_offset[i]<64)
        {
            if (adi_buff[i]>0xE000){
                pos_offset[i]+=16;
            }
            else if (adi_buff[i]>0xC000){
                pos_offset[i]+=8;
            }
            else if (adi_buff[i]>0x9000){
                pos_offset[i]+=4;
            } 
            else{
                pos_offset[i]++;
            }
            if(pos_offset[i]>63){
                pos_offset[i]=63;
            }
            
            offset_word=((unsigned int)pos_offset[i])<<8;
            adi_write_single(stage_offset_register,offset_word);
            
            adi_clear_int();
            adi_wait_for_int();
            adi_clear_int();
            adi_wait_for_int();
            adi_read_conversions();
            
        } //end of while this zone is still high
    } //end of loop over all zones
}//end of function

void adi_adjust_differential_offsets(){  //TO DO- fine tune the step sizes
    unsigned int i;
    unsigned int stage_offset_register;
    unsigned int offset_word;

    //get initial values of all zones
    adi_clear_int();
    adi_wait_for_int();
    adi_clear_int();
    adi_wait_for_int();
    adi_read_conversions();
 
    for (i=0;i<12;i++){
        stage_offset_register=0x82 + 8*i;
        differential_pos_offset[i]=0;
         
        while (adi_buff[i]>0x7FFF & differential_pos_offset[i]<64)
        {
            if (adi_buff[i]>0xE000){
                differential_pos_offset[i]+=16;
            }
            else if (adi_buff[i]>0xC000){
                differential_pos_offset[i]+=8;
            }
            else if (adi_buff[i]>0x9000){
                differential_pos_offset[i]+=4;
            } 
            else{
                differential_pos_offset[i]++;
            }
            if(differential_pos_offset[i]>63){
                differential_pos_offset[i]=63;
            }
            
            offset_word=((unsigned int)differential_pos_offset[i])<<8;
            adi_write_single(stage_offset_register,offset_word);
            
            adi_clear_int();
            adi_wait_for_int();
            adi_clear_int();
            adi_wait_for_int();
            adi_read_conversions();
        } //end of while this zone is still high

        //now if positive offset is 0, it means the zone was already below 0x7FFF.
        //see if we want to raise the negative offset
        differential_neg_offset[i]=0;
        if(differential_pos_offset[i]==0){
            while (adi_buff[i]<0x7FFF & differential_neg_offset[i]<64)
            {
                if (adi_buff[i]>0xE000){
                    differential_neg_offset[i]+=16;
                }
                else if (adi_buff[i]>0xC000){
                    differential_neg_offset[i]+=8;
                }
                else if (adi_buff[i]>0x9000){
                    differential_neg_offset[i]+=4;
                } 
                else{
                    differential_neg_offset[i]++;
                }
                if(differential_neg_offset[i]>63){
                    differential_neg_offset[i]=63;
                }

                offset_word=((unsigned int)differential_neg_offset[i]);
                adi_write_single(stage_offset_register,offset_word);

                adi_clear_int();
                adi_wait_for_int();
                adi_clear_int();
                adi_wait_for_int();
                adi_read_conversions();
            } //end of while this zone is still low
        } //end of if pos offset was zero

    } //end of loop over all zones
}//end of function



void adi_read_conversions(){
    adi_read_burst(0x000B, 12);
}

void adi_write_single(unsigned int address, unsigned int  payload){
    unsigned char tempchar;
    tempchar=0;
    tempchar&=(unsigned char)(address>>8); //two LSB of first byte are two highest bits of address.
    tempchar|=0b11100000; //ADI specified pattern for first byte, R/nW=0 for write
    ADI_CS_LOW();
    SSP1BUF=tempchar;  //send first byte of command
    tempchar=LOW_BYTE(address);
    wait_for_BF();
    SSP1BUF=tempchar; //send second byte of command 
    wait_for_BF();
    SSP1BUF=HIGH_BYTE(payload); //send high byte of payload
    wait_for_BF();
    SSP1BUF=LOW_BYTE(payload); //send low byte of payload
    wait_for_BF();
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
    wait_for_BF();
    SSP1BUF=tempchar; //send lower byte of address
    wait_for_BF();
    SSP1BUF=0x00; //send junk to clock in high byte of data
    wait_for_BF();
    ph=SSP1BUF;
    SSP1BUF=0x00; //send junk to clock in low byte of data
    wait_for_BF();
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
    wait_for_BF();
    SSP1BUF=tempchar; //send second byte of command 
    wait_for_BF();
    
    //loop over all words in payload
    for(tempchar=0;tempchar<n;tempchar++){
        SSP1BUF=HIGH_BYTE(payload[tempchar]); //send high byte of payload
        wait_for_BF();
        SSP1BUF=LOW_BYTE(payload[tempchar]); //send low byte of payload
        wait_for_BF();  
    }
    
    ADI_CS_HIGH();
}


void adi_read_burst(unsigned int address, unsigned char n){
    //affects global buffer adi_buff[]
    unsigned char tempchar,ph,pl;
    ADI_CS_LOW();
    tempchar=0;
    tempchar&=(unsigned char)(address>>8); //two LSB of first byte are two highest bits of address.
    tempchar|=0b11100100; //ADI specified pattern for first byte, R/nW=1 for read
    SSP1BUF=tempchar;  //send pattern and two high bits of address
    wait_for_BF();
    SSP1BUF=LOW_BYTE(address); //send lower byte of address
    wait_for_BF();

    for(tempchar=0;tempchar<n;tempchar++){
        SSP1BUF=0x00; //send junk to clock in high byte of data
        wait_for_BF();
        ph=SSP1BUF;
        SSP1BUF=0x00; //send junk to clock in low byte of data
        wait_for_BF();
        pl=SSP1BUF;
        adi_buff[tempchar]=( ((unsigned int)ph)<<8 | ((unsigned int)pl) );
    }
    ADI_CS_HIGH();
}

void adi_setup_conversions(unsigned int buffer[]){
    //sets 12 zones to positive single ended, pos AFE offset only
    unsigned char i;
    unsigned int stage_connection_register;
    unsigned int tempint;
    
    //TO DO- write retry on fail
    for (i=0;i<12;i++){
        stage_connection_register=0x80 + 8*i;
        tempint=buffer[2*i];
        adi_write_single(stage_connection_register,tempint);
        tempint=adi_read_single(stage_connection_register);
        if(tempint!=buffer[2*i]){
            fault_code=FAULT_ADI_VERIFY;
            flag.bits.fault=1;
        }
        
        tempint=buffer[2*i + 1];
        stage_connection_register++;
        adi_write_single(stage_connection_register,tempint);   
        tempint=adi_read_single(stage_connection_register);
        if(tempint!=buffer[2*i+1]){
            fault_code=FAULT_ADI_VERIFY;
            flag.bits.fault=1;
        }
        
        stage_connection_register++;
        adi_write_single(stage_connection_register,0);   //AFE offsets start at 0
        tempint=adi_read_single(stage_connection_register);
        if(tempint){
            fault_code=FAULT_ADI_VERIFY;
            flag.bits.fault=1;
        }
        
    }
}


//  //4 independent zones, all single ended
//    //4 differenctial measurements
//    static const unsigned int stageBuffer[] = {
//    //Stage 0: zone 0, pos offset only
//    0xFFFE,
//    0x5FFF, 
//    0x0000, 
//    0x2626,
//    1600,
//    1600,
//    1600,
//    1600,
//    //Stage 1: zone 1, pos offset only 
//    0xFFFB,
//    0x5FFF,
//    0x0000,
//    0x2626,
//    1650,
//    1650,
//    1650,
//    1650,
//    //Stage 2: zone 2, pos offset only
//    0xFFEF,
//    0x5FFF,
//    0x0000,
//    0x2626,
//    1650,
//    1650,
//    1650,
//    1650,
//    //Stage 3: zone 3, pos offset only 
//    0xFFBF,
//    0x5FFF,
//    0x0,
//    0x2626,
//    1650,
//    1650,
//    1650,
//    1650,
//    //Stage 4, zone +0, -2, both pos and neg offsets, differential 
//    0xFEDE,
//    0x3FFF,
//    0x0,
//    0x2626,
//    1650,
//    1650,
//    1650,
//    1650,
//    //Stage 5, +0 -1, both pos and neg offsets, differential 
//    0xFBF6,
//    0x3FFF,
//    0x0,
//    0x2626,
//    1650,
//    1650,
//    1650,
//    1650,
//    //Stage 6, +1, -3, both pos and neg offsets, differential 
//    0xEF7B,
//    0x3FFF,
//    0x0,
//    0x2626,
//    1650,
//    1650,
//    1650,
//    1650,
//    //Stage 7, +2, -3, both pos and neg offsets, differential 
//    0xFF6F,
//    0x3FFE,
//    0x0,
//    0x2626,
//    1600,
//    1600,
//    1600,
//    1600,
//    //Stage 8 
//    0xFFFF,
//    0x7FFB,
//    0x0,
//    0x2626,
//    1100,
//    1100,
//    1150,
//    1150,
//    //Stage 9 
//    0xFFFF,
//    0x7FEF,
//    0x0,
//    0x2626,
//    1100,
//    1100,
//    1150,
//    1150,
//    //Stage 10 
//    0xFFFF,
//    0x5FBF,
//    0x0,
//    0x2626,
//    1200,
//    1200,
//    1300,
//    1300,
//    //Stage 11 
//    0xFFFF,
//    0xDEFF,
//    0x0,
//    0x2626,
//    1100,
//    1100,
//    1150,
//    1150};


