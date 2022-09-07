/*******************************************************************************
 * SPI for the Newhaven 4x20 OLED Display
 *  helped greatly by:
 * https://github.com/Andy4495/NewhavenOLED/blob/main/src/NewhavenOLED.cpp
 *
 * Quirks:
 * -The PIC transmits MSB first, but the NHD wants LSB first.  
 *  So I have to reverse the bits in each byte
 * -There is a start byte for each transmission that has five 1's and then some 
 *  bits to direct the action
 * -Each byte must be broken into two nibbles, and each nibble sent with a high 
 *  nibble of zeros.
*/
#include <xc.h>
#include "util.h"
#include "options.h"
#include "newhaven.h"
#include "globals.h"

#define CS_LOW()    LATB2=0
#define CS_HIGH()   LATB2=1
#define nRES()      PORTB&0b00000010 //B1

unsigned char row_address[4];
unsigned char row_bit;
unsigned char cursor_row, cursor_col;

void nhd_init(void){
    //init some vars
    row_address[0] = 0x80;
    row_address[1] = 0xA0;
    row_address[2] = 0xC0;
    row_address[3] = 0xE0;
    cursor_row=0;
    cursor_col=0;
    row_bit=0x08;
        
    PORTBbits.RB1=0; //nRES high
    delay_ms(200);
    PORTBbits.RB1=1; //nRES high
    delay_ms(1); //wait for powerup
    nhd_command(0x2A); //function set (extended command set)
    nhd_command(0x71); //function selection A
    nhd_data(0x00); // disable internal VDD regulator (2.8V I/O). data(0x5C) = enable regulator (5V I/O)
    nhd_command(0x28); //function set (fundamental command set)
    nhd_command(0x08); //display off, cursor off, blink off
    nhd_command(0x2A); //function set (extended command set)
    nhd_command(0x79); //OLED command set enabled
    nhd_command(0xD5); //set display clock divide ratio/oscillator frequency
    nhd_command(0x70); //set display clock divide ratio/oscillator frequency
    nhd_command(0x78); //OLED command set disabled
    nhd_command(0x09); //extended function set (4-lines)
    nhd_command(0x06); //COM SEG direction
    nhd_command(0x72); //function selection B
    nhd_data(0x00); //ROM CGRAM selection
    nhd_command(0x2A); //function set (extended command set)
    nhd_command(0x79); //OLED command set enabled
    nhd_command(0xDA); //set SEG pins hardware configuration
    nhd_command(0x10); //set SEG pins hardware configuration
    nhd_command(0xDC); //function selection C
    nhd_command(0x00); //function selection C
    nhd_command(0x81); //set contrast control
    nhd_command(0x7F); //set contrast control
    nhd_command(0xD9); //set phase length
    nhd_command(0xF1); //set phase length
    nhd_command(0xDB); //set VCOMH deselect level
    nhd_command(0x40); //set VCOMH deselect level
    nhd_command(0x78); //OLED command set disabled
    nhd_command(0x28); //function set (fundamental command set)
    nhd_command(0x01); //clear display
    nhd_command(0x80); //set DDRAM address to 0x00
    nhd_command(0x0C); //display ON
    delay_ms(100); //delay    
}

void nhd_clear()
{
    nhd_command(0x01);
    cursor_row=0;
    cursor_col=0;
}

void nhd_setCursor(unsigned char col, unsigned char row)
{
    cursor_row=row;
    cursor_col=col;
}

void nhd_writeChar(unsigned char c, unsigned char col, unsigned char row)
{
    nhd_setCursor(col,row);
    nhd_command(row_address[row]+cursor_col); //write location
    nhd_data(c); //write char
    // Update cursor position. Wrap around if necessary.
    cursor_col++;
    if (cursor_col >= COLS) {
      cursor_row++;
      cursor_col = 0;
    }
    if (cursor_row >= ROWS){
        cursor_row = 0;
    }
        
}

void nhd_whiteScreen()
{
    int i;
    nhd_command(0x01); //clear display
    nhd_command(0x02); //return home
    for(i=0;i<20;i++)
    {
    nhd_data(0x1F); //write solid blocks
    }
    nhd_command(0xA0); //line 2
    for(i=0;i<20;i++)
    {
    nhd_data(0x1F); //write solid blocks
    }
    nhd_command(0xC0); //line 3
    for(i=0;i<20;i++)
    {
    nhd_data(0x1F); //write solid blocks
    }
    nhd_command(0xE0); //line 4
    for(i=0;i<20;i++)
    {
    nhd_data(0x1F); //write solid blocks
    }
}


/* clocks one byte out while clocking one byte in  */
unsigned char nhd_command(unsigned char byte){
    unsigned char tempchar;
    byte=reverse_byte(byte); //reverse so LSB first
    CS_LOW();
    SSP1BUF=0b11111000; //"start byte, reversed so LSB goes first"
    while(BF==0);
    tempchar=byte&0xF0;
    SSP1BUF=tempchar; //send low nibble
    while(BF==0);
    tempchar=(byte&0x0F)<<4;
    SSP1BUF=tempchar; //send high nibble
    while(BF==0);
     CS_HIGH();
    return(SSP1BUF); //Does this read clear BF?
}

unsigned char nhd_data(unsigned char byte){
    unsigned char tempchar;
    byte=reverse_byte(byte);
    CS_LOW();
    SSP1BUF=0b11111010; //start byte,reversed so LSB goes first    
    while(BF==0);
    tempchar=byte&0xF0;
    SSP1BUF=tempchar;
    while(BF==0);
    tempchar=(byte&0x0F)<<4;
    SSP1BUF=tempchar;
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

unsigned char reverse_byte(unsigned char x)
{
    static const unsigned char table[] = {
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
        0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
        0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
        0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
        0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
        0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
        0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
        0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
        0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
        0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
        0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
        0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
        0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
        0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
        0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
        0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
        0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
    };
    return table[x];
}