/* 
 * File:   ad8.h
 * Author: Jeff
 *
 * Created on Sept. 8, 2022
 */

#ifndef ADI_H
#define ADI_H

#define PWR_CONTROL 0x0000
#define STAGE_CAL_EN 0x0001
#define AMB_COMP_CTRL0 0x0002
#define AMB_COMP_CTRL1 0x0003
#define AMB_COMP_CTRL2 0x0004
#define STAGE_LOW_INT_EN 0x0005
#define STAGE_HIGH_INT_EN 0x0006
#define STAGE_COMPLETE_INT_EN 0x0007


#define STAGE0_CONNECTION 0x0080
#define STAGE1_CONNECTION 0x0088
#define STAGE2_CONNECTION 0x0090
#define STAGE3_CONNECTION 0x0098
#define STAGE4_CONNECTION 0x00A0
#define STAGE5_CONNECTION 0x00A8
#define STAGE6_CONNECTION 0x00B0
#define STAGE7_CONNECTION 0x00B8
#define STAGE8_CONNECTION 0x00C0
#define STAGE9_CONNECTION 0x00C8
#define STAGE10_CONNECTION 0x00D0
#define STAGE11_CONNECTION 0x00D8



void adi_init(void);
void adi_setup_conversions(unsigned int buffer[]);
void adi_adjust_pos_offsets(void);
void adi_adjust_differential_offsets(void);
void adi_start(void);  //enter full power mode
void adi_stop(void); //enter shutdown mode
void adi_wait_for_int(void);
void adi_clear_int(void);

void adi_write_single(unsigned int address, unsigned int  payload);
unsigned int adi_read_single(unsigned int command);

void adi_write_burst(unsigned int address, unsigned int payload[], unsigned char n);
void adi_read_burst(unsigned int address, unsigned char n);


void adi_read_conversions(void);

const unsigned int single_ended_setup[] = {
    0xFFFE, 0x5FFF,     //Stage 0 
    0xFFFB, 0x5FFF,     //Stage 1 
    0xFFEF, 0x5FFF,     //Stage 2 
    0xFFBF, 0x5FFF,     //Stage 3 
    0xFEFF, 0x5FFF,     //Stage 4 
    0xFBFF, 0x5FFF,     //Stage 5 
    0xEFFF, 0x5FFF,     //Stage 6 
    0xFFFF, 0x5FFE,     //Stage 7 
    0xFFFF, 0x5FFB,     //Stage 8 
    0xFFFF, 0x5FEF,     //Stage 9 
    0xFFFF, 0x5FBF,     //Stage 10 
    0xFFFF, 0x5EFF};    //Stage 11 

const unsigned int differential_setup[] = {
    0xFFF6, 0x3FFF,    //Stage 0 +0, -1, both pos and neg offsets, differential
    0xFFDB, 0x3FFF,    //Stage 1  +1 -2
    0xFF6F, 0x3FFF,    //Stage 2  +2, -3
    0xFDBF, 0x3FFF,    //Stage 3  +3, -4
    0xF6FF, 0x3FFF,    //Stage 4  +4, -5
    0xDBFF, 0x3FFF,    //Stage 5  +5, -6
    0xEFFF, 0x3FFD,    //Stage 6  +6, -7
    0xFFFF, 0x3FF6,    //Stage 7  +7, -8
    0xFFFF, 0x3FDB,    //Stage 8  +8, -9
    0xFFFF, 0x3F6F,    //Stage 9  +9, -10
    0xFFFF, 0x3DBF,    //Stage 10 +10, -11
    0xFFFF, 0x36FF};    //Stage 11 +11, -12
 

#endif	/* ADI_H */

