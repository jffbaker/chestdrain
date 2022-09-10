/* 
 * File:   ad8.h
 * Author: Jeff
 *
 * Created on Sept. 8, 2022
 */

#ifndef ADI_H
#define ADI_H

void adi_init(void);
void adi_write_single(unsigned int address, unsigned int  payload);
unsigned int adi_read_single(unsigned int command);

void adi_write_burst(unsigned int address, unsigned int payload[], unsigned char n);
void adi_read_burst(unsigned int address, unsigned char n);


#endif	/* ADI_H */

