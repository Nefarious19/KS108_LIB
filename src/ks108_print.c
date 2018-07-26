/*
 * ks108_print.c
 *
 *  Created on: 18.07.2018
 *      Author: rafal
 */

#include "stm32f0xx.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "../inc/ks108.h"
#include "../inc/ks108_utilities.h"
#include "../inc/ks108_gfx.h"
#include "../inc/ks108_print.h"
#include "../inc/ks108_font.h"

void KS108_putChr(char data, uint8_t x, uint8_t y, uint8_t size )
{
   uint16_t tab_index = ((uint16_t)(data - 0x20)*5);
   uint8_t byte_from_flash = 0;
   uint8_t tmp_x = x;
   uint8_t tmp_y = y;

   if( x > (KS108_WIDTH - size * 5) ) return;

   for(uint8_t i = 0; i < 5; i++)
   {

	  byte_from_flash = KS108_Font5x7[tab_index+i];

	  for(uint8_t x = 0; x < 8; x++)
	  {
		  if( byte_from_flash & (1<<x) ) KS108_drawFilledSquare(tmp_x,tmp_y,size);
		  else KS108_drawClearedSquare(tmp_x,tmp_y,size);
		  tmp_y += size;
	  }
	  tmp_y = y;
	  tmp_x += size;

   }
}

void KS108_putStr(char * data, uint8_t x, uint8_t y, uint8_t size )
{
	char ascii = 0;
	uint8_t shift_x = 0;

	shift_x = 5 * size + 2;

	while( (ascii = *data++) )
	{
		KS108_putChr(ascii,x,y,size);
		x += shift_x;
	}
}

void KS108_putInt(int32_t num , uint8_t x, uint8_t y, uint8_t size )
{
	int32_t tmp_num = 0;
    uint8_t int_len = 0;
    uint8_t sign = 0;
    char * temp_buffer = ( void * )0;

    tmp_num = num;

    do
    {
    	tmp_num /= 10;
    	int_len++;

    }while(tmp_num);

    temp_buffer = malloc((int_len+2)*sizeof(char));
    memset(temp_buffer,0,int_len+2);

    if( num < 0 )
    {
    	num *= -1;
    	*temp_buffer = '-';
    	sign = 1;
    }

    while(int_len--)
    {
    	temp_buffer[int_len+sign] = (char)('0' + num%10);
    	num /= 10;
    };

    KS108_putStr(temp_buffer,x,y,size);

    free(temp_buffer);
}


void KS108_putHex(int32_t num , uint8_t x, uint8_t y, uint8_t size )
{
	uint32_t tmp_num = 0;
    uint8_t int_len = 0;

    char * temp_buffer = ( void * )0;

    tmp_num = (uint32_t)(num);

    do
    {
    	tmp_num /= 16;
    	int_len++;
    }while(tmp_num);

    temp_buffer = malloc((int_len+3)*sizeof(char));
    memset(temp_buffer,0,int_len+3);

    temp_buffer[0] = '0';
    temp_buffer[1] = 'x';

    while(int_len--)
    {
    	if( num%16 < 10) temp_buffer[int_len+2] = (char)('0' + num%16);
    	else temp_buffer[int_len+2] = (char)('A' + ((num%16)-10));
    	num /= 16;
    };

    KS108_putStr(temp_buffer,x,y,size);

    free(temp_buffer);
}

void KS108_putFloat( float num , uint8_t precision, uint8_t x, uint8_t y, uint8_t size )
{
	struct fakeFloat
	{
		uint8_t  sign;
		int32_t  intPart;
		int32_t  tempIntPart;
		uint32_t fractPart;
		uint32_t tempFractPart;
		char * wsk;
		uint32_t mult10;
		uint8_t  tempPrec;
		uint8_t  intPartLen;
		uint8_t  fractPartLen;
	};

	struct fakeFloat f = {0};

	f.tempPrec = precision;

	f.sign = (uint8_t)( ((*(uint32_t * )(&num))>>31) & 0x01 );

	f.intPart = (int)num;

	f.mult10 = 1;

	while(f.tempPrec--)
	{
		f.mult10 *= 10;
	}

	num -= (float)f.intPart;

	num = num * (float)(f.mult10);

	*((uint32_t *)(&num)) &= ~(1<<31);

	f.fractPart = (uint32_t)num;

	f.tempIntPart = f.intPart;
	f.tempFractPart = f.fractPart;

	while(f.tempIntPart)
	{
		f.intPartLen++;
		f.tempIntPart /= 10;
	}

	while(f.tempFractPart)
	{
		f.fractPartLen++;
		f.tempFractPart /= 10;
	}



	f.wsk = malloc( sizeof(char) * (f.intPartLen + precision + 3) );
	memset(f.wsk , 0, sizeof(char) * (f.intPartLen + precision + 3) );

	if(f.sign)
	{
		f.wsk[0] = '-';
		f.intPart *= (-1);
	}






	if(precision)
	{
		f.wsk[ f.intPartLen + f.sign ] = '.';


		if(f.fractPart%10 > 4) f.fractPart++;

		while(f.fractPart)
		{
			f.wsk[ f.intPartLen + f.sign + precision] = (char)((f.fractPart%10)+'0');
			f.fractPart /= 10;
			precision--;
		}
		while(precision)
		{
			f.wsk[ f.intPartLen + f.sign + precision] = '0';
			precision--;
		}


	}

	while(f.intPart)
	{
		f.wsk[ f.intPartLen + f.sign -1] = (char)((f.intPart%10)+'0');
		f.intPartLen--;
		f.intPart /= 10;
	}

	KS108_putStr(f.wsk,x,y,size);

	free(f.wsk);
}
























