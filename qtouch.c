
#include <xc.h>
#include "util.h"
#include "options.h"
#include "qtouch.h"

#define CS_LOW()    LATB2=0
#define CS_HIGH()   LATB2=1
#define DRDY()      PORTB&0b00000010 //B1


void qtouch_init(void){
//    //wait for accel power up
//    delay(2000);                      //wait for reset to complete
//    unsigned char p=0;
//    while(p!=0x44){
//        p = spi_read_single(0x0F);   //should be non-zero
//    }
//    // set up accel registers
//    spi_write_single(0x20,0b00010011); //1.6Hz, low power mode, LP Mode 4
//    spi_write_single(0x21,0b00010110); //no boot, no reset, disconnect CS pullup, cont update, auto inc, disable I2C, SPI is 4 wire
//    spi_write_single(0x22,0b00000010); //no self test, push-pull, no latching, active high, write reg for single conversion, don't start now
//    spi_write_single(0x22,0b00000010); //no self test, push-pull, no latching, active high, write reg for single conversion, don't start now
//    //spi_write_single(0x23,0b00000000); //nothing routed to int1
//    spi_write_single(0x23,0b00000001); //data ready is on int1
//    spi_write_single(0x24,0b00000000); //nothing routed to int2
//    //spi_write_single(0x24,0b00000001); //data ready routed to int2
//    spi_write_single(0x25,0b00000000); //LPF cutoff is ODR/2, +/-2g, LP, no Low noise
//    spi_write_single(0x3F,0b10000000); //DRDY is pulsed, no INT2 routing, no...
}

/* clocks one byte out while clocking one byte in  */
unsigned char spi_single(unsigned char command){
    unsigned char tempchar;
    tempchar=PORTB&0x02;
    while(!tempchar) //wait for DRDY line to be high
    {
        tempchar=PORTB&0x02;
    }
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