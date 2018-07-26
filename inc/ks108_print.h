/*
 * ks108_print.h
 *
 *  Created on: 18.07.2018
 *      Author: rafal
 */

#ifndef KS108_INC_KS108_PRINT_H_
#define KS108_INC_KS108_PRINT_H_

void KS108_putChr(char   data, uint8_t x, uint8_t y, uint8_t size );
void KS108_putStr(char * data, uint8_t x, uint8_t y, uint8_t size );
void KS108_putInt(int32_t num , uint8_t x, uint8_t y, uint8_t size );
void KS108_putHex(int32_t num , uint8_t x, uint8_t y, uint8_t size );
void KS108_putFloat( float num , uint8_t x, uint8_t y, uint8_t size );



#endif /* KS108_INC_KS108_PRINT_H_ */
