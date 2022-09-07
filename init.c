/**
 
*/

#include "init.h"
#include "options.h"
#include "util.h"
#include "globals.h"

void SYSTEM_Initialize(void)
{
    PMD_Initialize();
    OSCILLATOR_Initialize();
    
    
    //**************************************************************************
    //set up Timer 0 as timer gate
    T0CON0=0b10010000; //On, 16 bit, 1:1 postscaler  62.5ns tics; 4.096ms rollovers 
    T0CON1=0b01000001;  //Fosc/4, sync, 1:2 prescaler gives 125ns tics, 8.192ms rollovers
    TMR0IF=0;
    TMR0IE=0;

    //**************************************************************************
    // Setup SPI for the Qtouch chip
    // B5 is mosi (data out of pic)
    
    //A0 is reset
    PORTAbits.RA0 =0; //default low
    TRISAbits.TRISA0=0; //output
    ODCONAbits.ODCA0=0;
    
    //B1 is nRES
    PORTBbits.RB1=0; //display initially held in reset
    TRISBbits.TRISB1=0; //B1 reset output
    
    //B2 is CS
    LATBbits.LATB2=1;   //CS is initially high
    TRISBbits.TRISB2=0; //B2 is CS output
        
    //B3 is SCL
    RB3PPS=0x0D;  //Set B3 PPS to SCL
    TRISBbits.TRISB3=0; //B3 is output
    ANSELBbits.ANSELB3=0; //digital io. See section 26.2.1
    SSP1CLKPPS=0x0D; //clock input to MSSP1 module.  See section 26.2.1.
            
    //B4 is MOSI
    RB4PPS=0x0E; //B5 is SDO
    TRISBbits.TRISB4=0; //B5 is output
    
    //B5 is MISO (data into pic)
    SSP1DATPPS=0b00001101; //Set SDA PPS to PortB, pin 5
    RB5PPS=0x0E; //not sure if this is needed, since pin is an input.
    TRISBbits.TRISB5=1; //B4 is input
    ANSELBbits.ANSELB5=0; //digital input buffer is enabled
    
    SSP1CON1bits.SSPEN=0; //turn it off before configuring
    SSP1CON1bits.SSPM=2;  //SPI master, Fosc/16 (8MHz)
    SSP1CON1bits.CKP=1;   //clock polarity idle high
    SSP1STATbits.CKE=0;   //clock edge;
    SSP1STATbits.SMP=1;   //sample at end
    SSP1CON1bits.SSPEN=1; //turn it on
    
    
        //**************************************************************************
//    //set up Timer 5 as ZCD qaulity checker
//    T5CON=0b00100011; //1:4 prescale, 16bit rw, on
//    TMR5CLK=0b00000001; //Src is F0osc/4;
//    TMR0IF=0;
//    TMR5IE=0;
    
//    //**************************************************************************
//    //set up Timer 2 for ZCD time check
//    T2CLKCON=0b00000001; //Fosc/4
//    T2CON=0b01101001; //Off, 1:64 prescaler, 1:10 postscaler 25kHz, 40us tics
//    TMR2IF=0;
//    TMR2IE=0;
//    T2ON=1;
//    
//    //**************************************************************************
//    //set up Timer 4 for Timer 3 gate
//    T4CLKCON=0b00000001; //Fosc/4
//    T4CON=0b01101001; //Off, 1:64 prescaler, 1:10 postscaler 25kHz, 40us tics
//    T4PR=GATE_WIDTH; //
//    TMR4IF=0;
//    TMR4IE=0;
//    T4ON=1;
    
    //**************************************************************************
    //Setup timers 1 and 3, with gates, for counting sensors
    //Setup Timer 1 on A4
//    TRISAbits.TRISA4=1;
//    ANSELAbits.ANSELA4=0; //enable digital input buffer
//    WPUAbits.WPUA4=0; //pullup disabled
//    INLVLAbits.INLVLA4=1; //Schmidt trigger input
//    T1CKIPPS=0x04; //This is A4
//    TMR1CLK =0; //external pin, T1CKIPPS
//    T1CON = 0b00000001; //prescale 1:1; no sync; Read 8; On
//    TMR1IF=0;				
//    TMR1IE=0;
//    
//    T1GCONbits.GE=1;
//    T1GCONbits.T1GPOL=1; //TMR1 counts when gate goes high
//    T1GCONbits.T1GTM=0;  //toggle mode; will count from one TMR2IF to next.
//    T1GCONbits.T1GSPM=0; //no single pulse mode
//    TMR1GATE=0b00000001; //TMR0 Overflow toggles gate

    //Setup Timer 3 on C3
//    TRISC3=1; //C3 is an input
//    ANSELCbits.ANSELC3=0; //enable digital input buffer
//    WPUCbits.WPUC3=0;
//    INLVLCbits.INLVLC3=1; 
//    T3CKIPPS=0x13; //This is C3
//    TMR3CLK =0; //external pin, T3CKIPPS
//    T3CON = 0b00000101; //prescale 1:1; no sync; Read 8; On
//    TMR3IF=0;				
//    TMR3IE=0;
//    
//    T3GCONbits.GE=1;
//    T3GCONbits.T3GPOL=1; //TMR3 counts when gate goes high
//    T3GCONbits.T3GTM=0;  //toggle mode; will count from one TMR2IF to next.
//    T3GCONbits.T3GSPM=0; //no single pulse mode
//    TMR3GATE=0b00000001; //TMR0 Overflow toggles gate

    
    //**************************************************************************
    //Setup C4 for debugging
    TRISC&=0b11101111;
    
    //**************************************************************************
    //Setup pins for controlling oscillators
//    TRISA5=0; //A5 is top oscillator power
//    TRISC2=0; //C2 is bottom oscillator power
    
    
    //**************************************************************************
    //Setup ZCD
//    ANSELB0=1; //analog input on B0
//    WPUB0=0;    //pullup disabled
//    ZCDCON=0b10000011;  //on, not inverted, int on high and low going
//    ZCDIE=0;
    
    
    //**************************************************************************
    //Setup B3 for controlling relay
//    LATBbits.LATB3=0; //default off
//    TRISBbits.TRISB3=0; //output

    //**************************************************************************
    //Setup UART
    //RB5PPS=0x09; //B2 is TX1
    //TRISB&=0b11011111; //B2 is an output
    
#ifdef ENABLE_TX
    RB0PPS=0x09; //B0 is TX1
    TRISBbits.TRISB0 =0; //B2 is an output
    //LATB2=1; //idle high
    ANSELBbits.ANSELB0=0;
    WPUBbits.WPUB0=0;
    ODCONBbits.ODCB0=0; //push-pull
    SLRCONBbits.SLRB0=0; //max slew rate
    
    
    BAUD1CONbits.BRG16=1; //use 16 bit generator
    TX1STAbits.BRGH=1; //high speed        
    SPBRGH = 0;
    SPBRGL = 137; // 137 gives 115 kbaud @ 64MHz
    //SPBRGL = 17; // 17 gives 460800 kbaud
    //SPBRGL = 16; // 16 gives 921600 kbaud @64MHz
    
    TX1STAbits.SYNC=0; //asynchronous mode
    RC1STAbits.SPEN=1; //enable serial port!
    BAUD1CONbits.SCKP=0; //not inverted       
    TX1STAbits.TX9=0; //transmit 8 bits

    TX1STAbits.TXEN=1;
//    TX1STAbits.SENDB=0; //no break
//    TX1STAbits.TX9D=1; //always high; this is stop bit??
    

//
//    BAUD1CONbits.ABDEN=0; //disable auto baud rate detector
//    BAUD1CONbits.WUE=0; //wake up on receiver disabled
#endif
}

void OSCILLATOR_Initialize(void)
{
    // NOSC HFINTOSC; NDIV 1; 
    OSCCON1 = 0x60;
    // CSWHOLD may proceed; SOSCPWR Low power; 
    OSCCON3 = 0x00;
    // MFOEN disabled; LFOEN disabled; ADOEN disabled; SOSCEN disabled; EXTOEN disabled; HFOEN disabled; 
    OSCEN = 0x00;
    // HFFRQ 16_MHz; 
    OSCFRQ = 0x08;
    // TUN 0; 
    OSCTUNE = 0x00;
}

void PMD_Initialize(void)
{
    // CLKRMD CLKR enabled; SYSCMD SYSCLK enabled; SCANMD SCANNER enabled; FVRMD FVR enabled; IOCMD IOC enabled; CRCMD CRC enabled; HLVDMD HLVD enabled; NVMMD NVM enabled; 
    PMD0 = 0x00;
    // TMR0MD TMR0 enabled; TMR1MD TMR1 enabled; TMR4MD TMR4 enabled; TMR5MD TMR5 enabled; TMR2MD TMR2 enabled; TMR3MD TMR3 enabled; TMR6MD TMR6 enabled; 
    PMD1 = 0x00;
    // ZCDMD ZCD enabled; DACMD DAC enabled; CMP1MD CMP1 enabled; ADCMD ADC enabled; CMP2MD CMP2 enabled; 
    PMD2 = 0x00;
    // CCP2MD CCP2 enabled; CCP1MD CCP1 enabled; PWM4MD PWM4 enabled; PWM3MD PWM3 enabled; 
    PMD3 = 0x00;
    // CWG1MD CWG1 enabled; MSSP1MD MSSP1 enabled; UART1MD EUSART enabled; 
    PMD4 = 0x00;
    // DSMMD DSM enabled; 
    PMD5 = 0x00;
}


/**
 End of File
*/
