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
void adi_write_single(unsigned int address, unsigned int  payload);
unsigned int adi_read_single(unsigned int command);

void adi_write_burst(unsigned int address, unsigned int payload[], unsigned char n);
void adi_read_burst(unsigned int address, unsigned char n);

void adi_read_conversions(void);

void adi_start(void);  //enter full power mode
void adi_stop(void); //enter shutdown mode


#endif	/* ADI_H */

