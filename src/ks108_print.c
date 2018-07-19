/*
 * ks108_print.c
 *
 *  Created on: 18.07.2018
 *      Author: rafal
 */

#include "../inc/ks108.h"
#include "../inc/ks108_utilities.h"
#include "../inc/ks108_gfx.h"
#include "../inc/ks108_print.h"
#include "../inc/ks108_font.h"

void KS108_putC(char data, uint8_t x, uint8_t y)
{
   CS_number CS_pin;
   uint16_t tab_index = 0;
   uint8_t temp_x = 0;
   uint8_t temp_y = 0;

   tab_index = ((uint16_t)(data - 0x20)*5);

   temp_x = x;

   if( x < 64 ) CS_pin = CS1;
   else 		CS_pin = CS2;

   KS108_WriteInstruction( KS108_SET_X_ADR(line), CS_pin );
   KS108_WriteInstruction( KS108_SET_Y_ADR(x), CS_pin );

   for(uint8_t i = 0; i < 5; i++)
   {
      KS108_WriteDsiplay(KS108_Font5x7[tab_index+i], CS_pin);
      temp_x++;
      if( temp_x == 64 )
      {
    	  CS_pin = CS2;
    	  KS108_WriteInstruction( KS108_SET_Y_ADR(0), CS_pin );
    	  KS108_WriteInstruction( KS108_SET_X_ADR(line), CS_pin );
      }
      else if(temp_x > 64) CS_pin = CS2;
      else 	CS_pin = CS1;
   }
}
