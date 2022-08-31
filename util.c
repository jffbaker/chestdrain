/*	Util.c
	by Jeff Baker		
	Started 1/07/02
*/

#include <xc.h>
#include <stdlib.h>
#include "globals.h"
#include "options.h"


/****************************************************************
*	fract_mul(A,B)												*
****************************************************************/
unsigned long fract_mul(unsigned long A, unsigned long B)
{
    unsigned long result;
    
    typedef union {
        unsigned long long value;
        unsigned long longs[2];
        unsigned int  ints[4];
        unsigned char bytes[8];
    } longUnion;
    
    longUnion C;
   
    C.value = (unsigned long long)A * (unsigned long long)B;
    result=C.longs[1];
 
    if (C.longs[0]>0x7FFFFFFF){
        result++;
    }
       
	return(result);
}

//**********************************************************************/
void delay(int j)
{
	unsigned int i = 0;
	for(i = 0; i < j; i++)	//dumb loop to waste time
		continue;
}//end of delay()

//**********************************************************************/
void delay_ms(int j)
{
    unsigned int i=0;
    TMR2=0;
    T2CON=0b11100100; //on, 1:64 pre, 1:16 post
    T2CLKCON=0x02;
    T2HLT=0x00;
    T2PR=195;
    for(i=0; i <j; i++)
    {   
        TMR2IF=0;
        while(!TMR2IF);
    }
    T2CON=0x00;
    return;
}//end of delay()


//**********************************************************************/
unsigned long abs_long(signed long value)
{
    if (value < 0) {
        value = value * (-1);
    }
    return ((unsigned long)value);
}


//**********************************************************************/
unsigned int abs_int(signed int value)
{
    if (value < 0) {
        value = value * (-1);
    }
    return ((unsigned int)value);
}

//**********************************************************************/
unsigned int abs_char(signed char value)
{
    if (value < 0) {
        value = value * (-1);
    }
    return ((unsigned char)value);
}


//**********************************************************************/
signed int max_int(signed int a, signed int b)
{
    signed int result;
    if(a >= b){
        result=a;
    }
    else{
        result=b;
    }
    return (result);
}

//**********************************************************************/
//returns index of max
unsigned char array_max_int(signed int a[], unsigned char size)
{
    unsigned char i,result;
    result=0;
    for(i = 0; i <= size; i++){
        if(a[i]>=a[result]){
            result=i;
        }
    }
    return (result);
}

//**********************************************************************/
signed int min_int(signed int a, signed int b)
{
    signed int result;
    if(a <= b){
        result=a;
    }
    else{
        result=b;
    }
    return (result);
}

//**********************************************************************/
//returns index of min
unsigned char array_min_int(signed int a[], unsigned char size)
{
    unsigned char i,result;
    result=0;
    for(i = 0; i <= size; i++){
        if(a[i]<=a[result]){
            result=i;
        }
    }
    return (result);
}

//**********************************************************************/
void wiggle(void)
{
        LATC|=0b00010000;
        unsigned char i;
        for(i=1;i<50;i++){
        }
        LATC&=0b11101111;
}

//**********************************************************************/
void wiggle_high(void)
{
        LATC|=0b00010000;
}

//**********************************************************************/
void wiggle_low(void)
{
        LATC&=0b11101111;
}

unsigned int read_tmr5(void)
{
    unsigned int readVal;
    unsigned char readValHigh;
    unsigned char readValLow;
	
    readValLow = TMR5L;
    readValHigh = TMR5H;
    
    readVal = ((unsigned int)readValHigh << 8) | readValLow;

    return readVal;
}

void write_tmr5(unsigned int timerVal){
        // Stop the Timer by writing to TMRxON bit
        T5CONbits.ON = 0;

        // Write to the Timer1 register
        TMR5H = HIGH_BYTE(timerVal);
        TMR5L = LOW_BYTE(timerVal);

        // Start the Timer after writing to the register
        T5CONbits.ON =1;
}