/*
 * KS108_gfx.c
 *
 *  Created on: 16.07.2018
 *      Author: rafal
 */

#include "stm32f0xx.h"

#include "../inc/ks108.h"
#include "../inc/ks108_utilities.h"
#include "../inc/ks108_gfx.h"

#include "../../DELAY/delay.h"

uint8_t KS108_displayBuf[8][128];

static inline void KS108_DATA_OUT(void)
{
	DATA_PORT->MODER = (DATA_PORT->MODER & 0xFFFF0000) | 0x00005555;
}
static inline void KS108_DATA_IN(void)
{
	DATA_PORT->MODER  &=  0xFFFF0000;
}

inline void KS108_putPixel (uint8_t x, uint8_t y)
{
	KS108_displayBuf[y/8][x] |= (1<<(y%8));
}

inline void KS108_clrPixel (uint8_t x, uint8_t y)
{
	KS108_displayBuf[y/8][x] &= ~(1<<(y%8));
}

void KS108_sendBuffer (uint8_t * display_buf)
{
	uint8_t * buf_ptr = (void *)0;

	buf_ptr = display_buf;

	KS108_WriteInstruction(KS108_SET_Y_ADR(0),CS1);
	KS108_WriteInstruction(KS108_SET_Y_ADR(0),CS2);

	for( uint8_t i = 0; i < 8; i++ )
	{
		KS108_WriteInstruction(KS108_SET_X_ADR(i),CS1);
		KS108_WriteInstruction(KS108_SET_X_ADR(i),CS2);
		KS108_RW_LO;
		KS108_DI_HI;
		KS108_CS1_LO;
		KS108_DATA_OUT();
		for(uint8_t x = 0; x < 64; x++)
		{
			DATA_PORT->ODR = (uint32_t)((DATA_PORT->ODR & 0x0000FF00) | *buf_ptr++);
			KS108_SET_E;
			_delay_us(1);
			KS108_CLR_E;
			_delay_us(1);
			_delay_us(1);

		}
		KS108_CS1_HI;
		KS108_CS2_LO;
		for(uint8_t x = 0; x < 64; x++)
		{
			DATA_PORT->ODR = (uint32_t)((DATA_PORT->ODR & 0x0000FF00) | *buf_ptr++);
			KS108_SET_E;
			_delay_us(1);
			KS108_CLR_E;
			_delay_us(1);
			_delay_us(1);
		}
		KS108_CS2_HI;
	}
}

void KS108_fillDisplay(uint8_t pattern)
{

	KS108_WriteInstruction(KS108_SET_Y_ADR(0),CS1);
	KS108_WriteInstruction(KS108_SET_Y_ADR(0),CS2);

	for( uint8_t i = 0; i < 8; i++ )
	{
		KS108_WriteInstruction(KS108_SET_X_ADR(i),CS1);
		KS108_WriteInstruction(KS108_SET_X_ADR(i),CS2);
		KS108_RW_LO;
		KS108_DI_HI;
		KS108_CS1_LO;
		KS108_DATA_OUT();
		_delay_us(1);
		DATA_PORT->ODR = (uint32_t)((DATA_PORT->ODR & 0x0000FF00) | pattern);
		for(uint8_t x = 0; x < 64; x++)
		{
			KS108_SET_E;
			_delay_us(1);
			KS108_CLR_E;
			_delay_us(6);
		}
		KS108_CS_Off();
		_delay_us(1);
		KS108_CS2_LO;
		_delay_us(1);
		for(uint8_t x = 0; x < 64; x++)
		{
			KS108_SET_E;
			_delay_us(1);
			KS108_CLR_E;
			_delay_us(6);
		}
		KS108_CS_Off();
		_delay_us(1);
	}
}


void KS108_drawLine(uint8_t x_0, uint8_t y_0, uint8_t x_1, uint8_t y_1 )
{

  if( (x_0 > KS108_WIDTH-1 ) || (x_1 > KS108_WIDTH-1 ) || (y_1 > KS108_HEIGHT-1 ) || (y_0 > KS108_HEIGHT-1 ) ) return;

  int x0,x1,y0,y1;
  int dx, dy;

  x0 = (int)x_0;
  x1 = (int)x_1;
  y0 = (int)y_0;
  y1 = (int)y_1;


  if( x1 <= x0 ) dx = x0 - x1;
  else dx = x1 - x0;
  if( y1 <= y0 ) dy = x0 - x1;
  else dy = y1 - y0;

  int sx = x0<x1 ? 1 : -1;

  int sy = y0<y1 ? 1 : -1;

  int err = (dx>dy ? dx : -dy)/2, e2;

  while(1)
  {
	KS108_putPixel(x0,y0);
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}


void KS108_clrLine(uint8_t x_0, uint8_t y_0, uint8_t x_1, uint8_t y_1 )
{

  if( (x_0 > KS108_WIDTH-1 ) || (x_1 > KS108_WIDTH-1 ) || (y_1 > KS108_HEIGHT-1 ) || (y_0 > KS108_HEIGHT-1 ) ) return;

  int x0,x1,y0,y1;
  int dx, dy;

  x0 = (int)x_0;
  x1 = (int)x_1;
  y0 = (int)y_0;
  y1 = (int)y_1;


  if( x1 <= x0 ) dx = x0 - x1;
  else dx = x1 - x0;
  if( y1 <= y0 ) dy = x0 - x1;
  else dy = y1 - y0;

  int sx = x0<x1 ? 1 : -1;

  int sy = y0<y1 ? 1 : -1;

  int err = (dx>dy ? dx : -dy)/2, e2;

  while(1)
  {
	KS108_clrPixel(x0,y0);
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}

void KS108_drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	if( ( (x + width) > (KS108_WIDTH - 1) ) || ( (y + height) > (KS108_HEIGHT - 1) ) ) return;

	KS108_drawLine(x,y,x+width,y);
	KS108_drawLine(x+width,y,x+width,y+height);
	KS108_drawLine(x,y+height,x+width,y+height);
	KS108_drawLine(x,y,x,y+height);
}

void KS108_drawClrRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	if( ( (x + width) > (KS108_WIDTH - 1) ) || ( (y + height) > (KS108_HEIGHT - 1) ) ) return;

	KS108_clrLine(x,y,x+width,y);
	KS108_clrLine(x+width,y,x+width,y+height);
	KS108_clrLine(x,y+height,x+width,y+height);
	KS108_clrLine(x,y,x,y+height);
}

void KS108_drawFilledRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	if( ( (x + width) > (KS108_WIDTH - 1) ) || ( (y + height) > (KS108_HEIGHT - 1) ) ) return;

	union pattern
	{
		uint64_t bits;
		uint8_t  tab[8];
	};

	union pattern pat;

	pat.bits = 0xFFFFFFFFFFFFFFFFULL;
	pat.bits =  (uint64_t)((uint64_t)pat.bits << (uint64_t)y);
	pat.bits &= (uint64_t)((uint64_t)pat.bits >> (uint64_t)(64-height-y));

	for(uint8_t i = x; i < (x + width); i++)
	{
		for(uint8_t j = 0; j < 8; j++ )
		{
			KS108_displayBuf[j][i] |= pat.tab[j];
		}
	}

}

void KS108_drawClearedRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	if( ( (x + width) > (KS108_WIDTH - 1) ) || ( (y + height) > (KS108_HEIGHT - 1) ) ) return;

	union pattern
	{
		uint64_t bits;
		uint8_t  tab[8];
	};

	union pattern pat;

	pat.bits = 0xFFFFFFFFFFFFFFFFULL;
	pat.bits =  (uint64_t)((uint64_t)pat.bits << (uint64_t)y);
	pat.bits &= (uint64_t)((uint64_t)pat.bits >> (uint64_t)(64-height-y));

	for(uint8_t i = x; i < (x + width); i++)
	{
		for(uint8_t j = 0; j < 8; j++ )
		{
			KS108_displayBuf[j][i] &= ~pat.tab[j];
		}
	}

}

void KS108_drawSquare(uint8_t x, uint8_t y, uint8_t a)
{
	KS108_drawRect(x, y, a, a);
}

void KS108_drawClrSquare(uint8_t x, uint8_t y, uint8_t a)
{
	KS108_drawClearedRect(x, y, a, a);
}

void KS108_drawFilledSquare(uint8_t x, uint8_t y, uint8_t a)
{
	KS108_drawFilledRect(x,y,a,a);
}

void KS108_drawClearedSquare(uint8_t x, uint8_t y, uint8_t a)
{
	KS108_drawClearedRect(x,y,a,a);
}

void KS108_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius)
{

	if( (radius > x0) ||
		(radius > y0) ||
		((KS108_WIDTH-1 - radius)  < x0) ||
		((KS108_HEIGHT-1 - radius) < y0 )
	  )
	{
		return;
	}

	int x = (int)(radius);
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (int)(radius << 1);

    while (x >= y)
    {
        KS108_putPixel((uint8_t)(x0 + x), (uint8_t)(y0 + y));
        KS108_putPixel((uint8_t)(x0 + y), (uint8_t)(y0 + x));
        KS108_putPixel((uint8_t)(x0 - y), (uint8_t)(y0 + x));
        KS108_putPixel((uint8_t)(x0 - x), (uint8_t)(y0 + y));
        KS108_putPixel((uint8_t)(x0 - x), (uint8_t)(y0 - y));
        KS108_putPixel((uint8_t)(x0 - y), (uint8_t)(y0 - x));
        KS108_putPixel((uint8_t)(x0 + y), (uint8_t)(y0 - x));
        KS108_putPixel((uint8_t)(x0 + x), (uint8_t)(y0 - y));

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

void KS108_drawFilledCircle(uint8_t x0, uint8_t y0, uint8_t radius)
{

	if( (radius > x0) ||
		(radius > y0) ||
		((KS108_WIDTH-1 - radius)  < x0) ||
		((KS108_HEIGHT-1 - radius) < y0 )
	  )
	{
		return;
	}

	int x = (int)(radius);
	int r2 = x << 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - r2;

    while (x >= y)
    {
        KS108_drawLine( (uint8_t)(x0 - x), (uint8_t)(y0 + y), (uint8_t)(x0 + x), (uint8_t)(y0 + y) );
        KS108_drawLine( (uint8_t)(x0 - x), (uint8_t)(y0 - y), (uint8_t)(x0 + x), (uint8_t)(y0 - y) );
        KS108_drawLine( (uint8_t)(x0 - y), (uint8_t)(y0 - x), (uint8_t)(x0 + y), (uint8_t)(y0 - x) );
        KS108_drawLine( (uint8_t)(x0 - y), (uint8_t)(y0 + x), (uint8_t)(x0 + y), (uint8_t)(y0 + x) );
        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - r2;
        }
    }
}

void KS108_drawArc(uint8_t x0, uint8_t y0, uint8_t r, uint8_t one_eight )
{
	int x = (int)(r);
	    int y = 0;
	    int dx = 1;
	    int dy = 1;
	    int err = dx - (int)(r << 1);

	    while (x >= y)
	    {
	      switch(one_eight)
	      {
	      	  case 0: KS108_putPixel((uint8_t)(x0 + x), (uint8_t)(y0 - y)); break;
	      	  case 1: KS108_putPixel((uint8_t)(x0 + y), (uint8_t)(y0 - x)); break;
	      	  case 2: KS108_putPixel((uint8_t)(x0 - y), (uint8_t)(y0 - x)); break;
	      	  case 3: KS108_putPixel((uint8_t)(x0 - x), (uint8_t)(y0 - y)); break;
	      	  case 4: KS108_putPixel((uint8_t)(x0 - x), (uint8_t)(y0 + y)); break;
	      	  case 5: KS108_putPixel((uint8_t)(x0 - y), (uint8_t)(y0 + x)); break;
	      	  case 6: KS108_putPixel((uint8_t)(x0 + y), (uint8_t)(y0 + x)); break;
	      	  case 7: KS108_putPixel((uint8_t)(x0 + x), (uint8_t)(y0 + y)); break;
	      }

	        if (err <= 0)
	        {
	            y++;
	            err += dy;
	            dy += 2;
	        }

	        if (err > 0)
	        {
	            x--;
	            dx += 2;
	            err += dx - (r << 1);
	        }
	    }
}

void KS108_drawRoundedRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t corner_radius)
{
	if( ( (x + width) > (KS108_WIDTH - 1) ) || ( (y + height) > (KS108_HEIGHT - 1) ) ) return;

	if( ((corner_radius) > (width>>1)) || ((corner_radius) > (height>>1)) ) return;

	uint8_t r = corner_radius;



	KS108_drawArc( x+width-r, y + r, r , 0);
	KS108_drawArc( x+width-r, y + r, r , 1);
	KS108_drawArc( x + r, y + r, r , 2);
	KS108_drawArc( x + r, y + r, r , 3);
	KS108_drawArc( x+width-r, y+height-r, r , 6);
	KS108_drawArc( x+width-r, y+height-r, r , 7);
	KS108_drawArc( x+r, y+height-r, r , 4);
	KS108_drawArc( x+r, y+height-r, r , 5);

	KS108_drawLine( x+r, y, x+width-r , y );
	KS108_drawLine( x+width, y + r , x+width , y+height - r );
	KS108_drawLine( x+r, y+height , x+width-r , y+height );
	KS108_drawLine( x, y+r, x, y+height-r);
}

