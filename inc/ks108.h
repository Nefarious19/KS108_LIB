/*
 * ks108.h
 *
 *  Created on: 20.06.2018
 *      Author: rafal
 */

#ifndef KS108_KS108_H_
#define KS108_KS108_H_


typedef enum {
	CS1,
	CS2
}CS_number;


extern uint8_t KS108_displayBuf[8][128];

void KS108_InitDisplay (void);
void KS108_WriteInstruction (uint8_t instruction, CS_number CS_nr );
void KS108_WriteDsiplay (uint8_t data, CS_number CS_nr);

#endif /* KS108_KS108_H_ */
