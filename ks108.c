/****************************************************************
 * @file   KS108.c
 *
 * @author Nefarious19
 * @date   01.08.2018
 * @brief  This is library for KS0108 based displays.
 *  	   It was developed for STM32F0 and STM32F1 microcontrolers.
 *		   You can use it for free.
 *		   Hardware: STM32F1 and STM32F0 family
 */
#include <stdlib.h>
#include <string.h>

#include "ks108.h"
#include "internal/ks108_font.h"
static uint8_t KS108_displayBuf[KS108_LINES][KS108_WIDTH];

static const KS108_pinMapping_t pinMappingArray[] =
{
	{DATA_D0},
	{DATA_D1},
	{DATA_D2},
	{DATA_D3},
	{DATA_D4},
	{DATA_D5},
	{DATA_D6},
	{DATA_D7},
	{CTRL_RW},
	{CTRL_DI},
	{CTRL_E},
	{CTRL_CS1},
	{CTRL_CS2},
	{CTRL_BACK},
#if USE_HARDWARE_RESET == KS108_FEATURE_ON
	{CTRL_RST}
#endif
};


#define KS108_Backlight_ON() KS108_ClrCtrlPin( pinMappingArray[KS108_CTRL_BACK].gpio_reg->ODR, pinMappingArray[KS108_CTRL_BACK].pinNumber );
#define KS108_Backlight_OFF() KS108_SetCtrlPin( pinMappingArray[KS108_CTRL_BACK].gpio_reg->ODR, pinMappingArray[KS108_CTRL_BACK].pinNumber );

#define KS108_CS1_LO() KS108_ClrCtrlPin( pinMappingArray[KS108_CTRL_CS1].gpio_reg->ODR, pinMappingArray[KS108_CTRL_CS1].pinNumber );
#define KS108_CS1_HI() KS108_SetCtrlPin( pinMappingArray[KS108_CTRL_CS1].gpio_reg->ODR, pinMappingArray[KS108_CTRL_CS1].pinNumber );

#define KS108_CS2_LO() KS108_ClrCtrlPin( pinMappingArray[KS108_CTRL_CS2].gpio_reg->ODR, pinMappingArray[KS108_CTRL_CS2].pinNumber );
#define KS108_CS2_HI() KS108_SetCtrlPin( pinMappingArray[KS108_CTRL_CS2].gpio_reg->ODR, pinMappingArray[KS108_CTRL_CS2].pinNumber );

#define KS108_CS_Off() 																							\
{																							 					\
	KS108_SetCtrlPin( pinMappingArray[KS108_CTRL_CS1].gpio_reg->ODR, pinMappingArray[KS108_CTRL_CS1].pinNumber );\
	KS108_SetCtrlPin( pinMappingArray[KS108_CTRL_CS2].gpio_reg->ODR, pinMappingArray[KS108_CTRL_CS2].pinNumber );\
}

#define KS108_DI_HI()   KS108_SetCtrlPin( pinMappingArray[KS108_CTRL_DI].gpio_reg->ODR, pinMappingArray[KS108_CTRL_DI].pinNumber );
#define KS108_DI_LO()   KS108_ClrCtrlPin( pinMappingArray[KS108_CTRL_DI].gpio_reg->ODR, pinMappingArray[KS108_CTRL_DI].pinNumber );

#define KS108_RW_HI()   KS108_SetCtrlPin( pinMappingArray[KS108_CTRL_RW].gpio_reg->ODR, pinMappingArray[KS108_CTRL_RW].pinNumber );
#define KS108_RW_LO()   KS108_ClrCtrlPin( pinMappingArray[KS108_CTRL_RW].gpio_reg->ODR, pinMappingArray[KS108_CTRL_RW].pinNumber );

#define KS108_CLR_E()   KS108_ClrCtrlPin( pinMappingArray[KS108_CTRL_E].gpio_reg->ODR, pinMappingArray[KS108_CTRL_E].pinNumber );
#define KS108_SET_E()   KS108_SetCtrlPin( pinMappingArray[KS108_CTRL_E].gpio_reg->ODR, pinMappingArray[KS108_CTRL_E].pinNumber );

#if USE_HARDWARE_RESET == KS108_FEATURE_ON
#define KS108_CLR_RST() KS108_ClrCtrlPin( pinMappingArray[KS108_CTRL_RST].gpio_reg->ODR, pinMappingArray[KS108_CTRL_RST].pinNumber );
#define KS108_SET_RST() KS108_SetCtrlPin( pinMappingArray[KS108_CTRL_RST].gpio_reg->ODR, pinMappingArray[KS108_CTRL_RST].pinNumber );
#endif

static inline void _delay_us( uint32_t us )
{
	uint32_t maxCount = (SystemCoreClock / 10000000);

	while(us--)
	{
		for(uint32_t i = 0; i <= maxCount; i++)
		{
			asm volatile("nop");
		}
	}
}

static inline void _delay_ms( uint32_t ms )
{
	while(ms--)	_delay_us( 1000 );
}

#if defined (STM32F1)
static inline void KS108_DATA_OUT(void)
{
	for(KS108_pinNo_t i = 0; i < 8; i++)
	{
		if(pinMappingArray[i].pinNumber < 8)
		{
			KS108_ModifyRegister(pinMappingArray[i].gpio_reg->CRL, 0x3, 0xF, pinMappingArray[i].pinNumber*4 );
		}
		else
		{
			KS108_ModifyRegister(pinMappingArray[i].gpio_reg->CRH, 0x3, 0xF, (pinMappingArray[i].pinNumber-8)*4 );
		}
	}
}

static inline void KS108_DATA_IN(void)
{
	for(KS108_pinNo_t i = 0; i < 8; i++)
	{
		if(pinMappingArray[i].pinNumber < 8)
		{
			KS108_ModifyRegister(pinMappingArray[i].gpio_reg->CRL, 0x4, 0xF, pinMappingArray[i].pinNumber*4 );
		}
		else
		{
			KS108_ModifyRegister(pinMappingArray[i].gpio_reg->CRH, 0x4, 0xF, (pinMappingArray[i].pinNumber-8)*4 );
		}
	}
}

static inline void KS108_GPIO_INIT (void)
{

	for(KS108_pinNo_t i = 8; i < (sizeof(pinMappingArray)/sizeof(pinMappingArray[0])); i++)
	{
		if(pinMappingArray[i].pinNumber < 8)
		{
			KS108_ModifyRegister(pinMappingArray[i].gpio_reg->CRL, 0x3, 0xF, pinMappingArray[i].pinNumber*4 );
			pinMappingArray[i].gpio_reg->ODR |= (1<<pinMappingArray[i].pinNumber);
		}
		else
		{
			KS108_ModifyRegister(pinMappingArray[i].gpio_reg->CRH, 0x3, 0xF, (pinMappingArray[i].pinNumber-8)*4 );
			pinMappingArray[i].gpio_reg->ODR |= (1<<pinMappingArray[i].pinNumber);
		}
	}
#if USE_HARDWARE_RESET == KS108_FEATURE_ON
	KS108_CLR_RST();
	_delay_ms(50);
	KS108_SET_RST();
#endif
	_delay_ms(10);
}

#elif defined (STM32F0)

static inline void KS108_DATA_OUT(void)
{
	for(KS108_pinNo_t i = 0; i < 8; i++)
	{
		KS108_ModifyRegister(pinMappingArray[i].gpio_reg->MODER, 0x1, 0x3, pinMappingArray[i].pinNumber*2 );
		KS108_ModifyRegister(pinMappingArray[i].gpio_reg->OTYPER, 0x0, 0x1, pinMappingArray[i].pinNumber );
		KS108_ModifyRegister(pinMappingArray[i].gpio_reg->OSPEEDR, 0x3, 0x3, pinMappingArray[i].pinNumber*2 );
		KS108_ModifyRegister(pinMappingArray[i].gpio_reg->PUPDR, 0x0, 0x3, pinMappingArray[i].pinNumber*2 );
	}
}


static inline void KS108_GPIO_INIT (void)
{

	KS108_DATA_OUT();

	for(KS108_pinNo_t i = 8; i < sizeof(pinMappingArray) / sizeof(pinMappingArray[0]); i++)
	{
		pinMappingArray[i].gpio_reg->ODR |= (1<<pinMappingArray[i].pinNumber);
		KS108_ModifyRegister(pinMappingArray[i].gpio_reg->MODER, 0x1, 0x3, pinMappingArray[i].pinNumber*2 );
		KS108_ModifyRegister(pinMappingArray[i].gpio_reg->OTYPER, 0x0, 0x1, pinMappingArray[i].pinNumber );
		KS108_ModifyRegister(pinMappingArray[i].gpio_reg->OSPEEDR, 0x3, 0x3, pinMappingArray[i].pinNumber*2 );
		KS108_ModifyRegister(pinMappingArray[i].gpio_reg->PUPDR, 0x0, 0x3, pinMappingArray[i].pinNumber*2 );
	}

#if USE_HARDWARE_RESET == KS108_FEATURE_ON
	KS108_CLR_RST();
	_delay_ms(50);
	KS108_SET_RST();
#endif

	_delay_ms(10);
}

#endif

#if USE_READ_FROM_PORT_FEATURES == KS108_FEATURE_ON

static inline void KS108_DATA_IN(void)
{
	for(KS108_pinNo_t i = 0; i < 8; i++)
	{
		KS108_ModifyRegister(pinMappingArray[i].gpio_reg->MODER, 0x0, 0x3, pinMappingArray[i].pinNumber*2 );
		KS108_ModifyRegister(pinMappingArray[i].gpio_reg->OTYPER, 0x0, 0x1, pinMappingArray[i].pinNumber );
		KS108_ModifyRegister(pinMappingArray[i].gpio_reg->OSPEEDR, 0x0, 0x3, pinMappingArray[i].pinNumber*2 );
		KS108_ModifyRegister(pinMappingArray[i].gpio_reg->PUPDR, 0x0, 0x3, pinMappingArray[i].pinNumber*2 );
	}
}

static inline uint8_t KS108_READ_DATA_FROM_PORT(void)
{
	uint8_t data = 0;
	for(KS108_pinNo_t i = 0; i < 8; i++)
	{
		data |= (uint8_t)(pinMappingArray[i].gpio_reg->IDR & (1<<i));
	}

	return data;
}

uint8_t KS108_ReadDisplayData (uint8_t line, uint8_t x )
{
	KS108_CSNumber CS_nr;

	CS_nr = x < 64 ? CS1 : CS2;

	KS108_WriteInstruction(KS108_SET_X_ADR(line),CS_nr);
	KS108_WriteInstruction(KS108_SET_Y_ADR(x),CS_nr);
	uint8_t data = 0;
	KS108_RW_HI();
	KS108_DI_HI();
	switch(CS_nr)
	{
		case CS1: KS108_CS1_LO(); break;
		case CS2: KS108_CS2_LO(); break;
	}
	KS108_DATA_IN();
	_delay_us(1);
	KS108_SET_E();
	_delay_us(3);
	data = KS108_READ_DATA_FROM_PORT();
	KS108_CLR_E();
	_delay_us(1);
	switch(CS_nr)
	{
		case CS1: KS108_CS1_HI(); break;
		case CS2: KS108_CS2_HI(); break;
	}
	_delay_us(1);
	return data;
}

uint8_t KS108_ReadDisplayStatus ( KS108_CSNumber CS_nr )
{
	uint8_t data = 0;
	KS108_RW_HI();
	KS108_DI_LO();
	KS108_DATA_IN();
	switch(CS_nr)
	{
		case CS1: KS108_CS1_LO(); break;
		case CS2: KS108_CS2_LO(); break;
	}
	_delay_us(1);
	KS108_SET_E();
	_delay_us(1);
	data = KS108_READ_DATA_FROM_PORT();
	KS108_CLR_E();
	_delay_us(1);
	switch(CS_nr)
	{
		case CS1: KS108_CS1_HI(); break;
		case CS2: KS108_CS2_HI(); break;
	}
	_delay_us(1);
	return data;
}

#endif

static inline void KS108_PUT_DATA_TO_PORT(uint8_t data)
{
	for(KS108_pinNo_t i = 0; i < 8; i++)
	{
		if( data & (1<<i) )
		{
			pinMappingArray[i].gpio_reg->ODR |=  (1<<i);
		}
		else
		{
			pinMappingArray[i].gpio_reg->ODR &= ~(1<<i);
		}
	}
}

void KS108_WriteInstruction (uint8_t instruction, KS108_CSNumber CS_nr )
{
#if USE_WAITING_FOR_BUSY_FLAG_OFF == KS108_FEATURE_ON
	while(KS108_ReadDisplayStatus(CS_nr) & KS108_DISPLAY_BSY);
#endif
	KS108_RW_LO();
	KS108_DI_LO();
	switch(CS_nr)
	{
		case CS1: KS108_CS1_LO(); break;
		case CS2: KS108_CS2_LO(); break;
	}
	KS108_DATA_OUT();
	_delay_us(1);
	KS108_PUT_DATA_TO_PORT(instruction);
	_delay_us(1);
	KS108_SET_E();
	_delay_us(1);
	KS108_CLR_E();
	_delay_us(1);
	switch(CS_nr)
	{
		case CS1: KS108_CS1_HI(); break;
		case CS2: KS108_CS2_HI(); break;
	}
}

void KS108_WriteData (uint8_t data, KS108_CSNumber CS_nr)
{
	KS108_RW_LO();
	KS108_DI_HI();
	switch(CS_nr)
	{
		case CS1: KS108_CS1_LO(); break;
		case CS2: KS108_CS2_LO(); break;
	}
	KS108_DATA_OUT();
	_delay_us(1);
	KS108_PUT_DATA_TO_PORT(data);
	_delay_us(1);
	KS108_SET_E();
	_delay_us(1);
	KS108_CLR_E();
	_delay_us(1);
	switch(CS_nr)
	{
		case CS1: KS108_CS1_HI(); break;
		case CS2: KS108_CS2_HI(); break;
	}
}

void KS108_InitDisplay (void(*gpio_clocks_init_function_ptr)(void))
{
	if(gpio_clocks_init_function_ptr)
	{
		gpio_clocks_init_function_ptr();
	}

	KS108_GPIO_INIT();

	KS108_WriteInstruction(KS108_DISPLAY_OFF ,CS1);
	KS108_WriteInstruction(KS108_SET_X_ADR(0),CS1);
	KS108_WriteInstruction(KS108_SET_Y_ADR(0),CS1);
	KS108_WriteInstruction(KS108_SET_Z_ADR(0),CS1);
	KS108_WriteInstruction(KS108_DISPLAY_ON ,CS1);

	KS108_WriteInstruction(KS108_DISPLAY_OFF ,CS2);
	KS108_WriteInstruction(KS108_SET_X_ADR(0),CS2);
	KS108_WriteInstruction(KS108_SET_Y_ADR(0),CS2);
	KS108_WriteInstruction(KS108_SET_Z_ADR(0),CS2);
	KS108_WriteInstruction(KS108_DISPLAY_ON ,CS2);

	KS108_Backlight_ON();

	KS108_fillDisplay(0x00);
}

uint8_t * KS108_GetBufferPtr(void)
{
	return &KS108_displayBuf[0][0];
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
		KS108_RW_LO();
		KS108_DI_HI();
		KS108_CS1_LO();
		KS108_DATA_OUT();
		for(uint8_t x = 0; x < 64; x++)
		{
			KS108_PUT_DATA_TO_PORT(*buf_ptr++);
			KS108_SET_E();
			_delay_us(1);
			KS108_CLR_E();
			_delay_us(1);
		}
		KS108_CS1_HI();
		KS108_CS2_LO();
		for(uint8_t x = 0; x < 64; x++)
		{
			KS108_PUT_DATA_TO_PORT(*buf_ptr++);
			KS108_SET_E();
			_delay_us(1);
			KS108_CLR_E();
			_delay_us(1);
		}
		KS108_CS2_HI();
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
		KS108_RW_LO();
		KS108_DI_HI();
		KS108_CS1_LO();
		KS108_DATA_OUT();
		_delay_us(1);
		KS108_PUT_DATA_TO_PORT(pattern);
		for(uint8_t x = 0; x < 64; x++)
		{
			KS108_SET_E();
			_delay_us(1);
			KS108_CLR_E();
			_delay_us(1);
		}
		KS108_CS_Off();
		_delay_us(1);
		KS108_CS2_LO();
		_delay_us(1);
		for(uint8_t x = 0; x < 64; x++)
		{
			KS108_SET_E();
			_delay_us(1);
			KS108_CLR_E();
			_delay_us(1);
		}
		KS108_CS_Off();
		_delay_us(1);
	}
}


void KS108_putPixel (uint8_t x, uint8_t y)
{
	KS108_displayBuf[y/8][x] |= (1<<(y%8));
}

void KS108_clrPixel (uint8_t x, uint8_t y)
{
	KS108_displayBuf[y/8][x] &= ~(1<<(y%8));
}

void KS108_drawLine(uint8_t x_0, uint8_t y_0, uint8_t x_1, uint8_t y_1  , KS108_Color_t color)
{

  if( (x_0 > KS108_WIDTH) || (x_1 > KS108_WIDTH) || (y_1 > KS108_HEIGHT) || (y_0 > KS108_HEIGHT) ) return;

  void (*pixelDrawFuncPtr)(uint8_t x, uint8_t y) = color ? KS108_putPixel : KS108_clrPixel;

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
	pixelDrawFuncPtr(x0,y0);
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}

void KS108_drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, KS108_Color_t color)
{
	if( ( (x + width) > (KS108_WIDTH) ) || ( (y + height) > (KS108_HEIGHT) ) ) return;


	KS108_drawLine(x,y,x+width,y, color);
	KS108_drawLine(x+width,y,x+width,y+height, color);
	KS108_drawLine(x,y+height,x+width,y+height, color);
	KS108_drawLine(x,y,x,y+height, color);
}


void KS108_drawFilledRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, KS108_Color_t color)
{
	if( ( (x + width) > (KS108_WIDTH) ) || ( (y + height) > (KS108_HEIGHT) ) ) return;

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
			if(color)
			{
				KS108_displayBuf[j][i] |=  pat.tab[j];
			}
			else
			{
				KS108_displayBuf[j][i] &= ~pat.tab[j];
			}
		}
	}

}


void KS108_drawSquare(uint8_t x, uint8_t y, uint8_t a, KS108_Color_t color)
{
	KS108_drawRect(x, y, a, a, color);
}



void KS108_drawFilledSquare(uint8_t x, uint8_t y, uint8_t a, KS108_Color_t color)
{
	KS108_drawFilledRect(x,y,a,a,color);
}


void KS108_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, KS108_Color_t color)
{

	if( (radius > x0) ||
		(radius > y0) ||
		((KS108_WIDTH-1 - radius)  < x0) ||
		((KS108_HEIGHT-1 - radius) < y0 )
	  )
	{
		return;
	}

	void (*pixelDrawFuncPtr)(uint8_t x, uint8_t y) = color ? KS108_putPixel : KS108_clrPixel;

	int x = (int)(radius);
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (int)(radius << 1);

    while (x >= y)
    {
    	pixelDrawFuncPtr((uint8_t)(x0 + x), (uint8_t)(y0 + y));
    	pixelDrawFuncPtr((uint8_t)(x0 + y), (uint8_t)(y0 + x));
    	pixelDrawFuncPtr((uint8_t)(x0 - y), (uint8_t)(y0 + x));
    	pixelDrawFuncPtr((uint8_t)(x0 - x), (uint8_t)(y0 + y));
    	pixelDrawFuncPtr((uint8_t)(x0 - x), (uint8_t)(y0 - y));
    	pixelDrawFuncPtr((uint8_t)(x0 - y), (uint8_t)(y0 - x));
    	pixelDrawFuncPtr((uint8_t)(x0 + y), (uint8_t)(y0 - x));
    	pixelDrawFuncPtr((uint8_t)(x0 + x), (uint8_t)(y0 - y));

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

void KS108_drawFilledCircle(uint8_t x0, uint8_t y0, uint8_t radius, KS108_Color_t color)
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
        KS108_drawLine( (uint8_t)(x0 - x), (uint8_t)(y0 + y), (uint8_t)(x0 + x), (uint8_t)(y0 + y), color );
        KS108_drawLine( (uint8_t)(x0 - x), (uint8_t)(y0 - y), (uint8_t)(x0 + x), (uint8_t)(y0 - y), color );
        KS108_drawLine( (uint8_t)(x0 - y), (uint8_t)(y0 - x), (uint8_t)(x0 + y), (uint8_t)(y0 - x), color );
        KS108_drawLine( (uint8_t)(x0 - y), (uint8_t)(y0 + x), (uint8_t)(x0 + y), (uint8_t)(y0 + x), color );
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

void KS108_drawArc(uint8_t x0, uint8_t y0, uint8_t r, uint8_t one_eight, KS108_Color_t color )
{
	int x = (int)(r);
	    int y = 0;
	    int dx = 1;
	    int dy = 1;
	    int err = dx - (int)(r << 1);

	    void (*pixelDrawFuncPtr)(uint8_t x, uint8_t y) = color ? KS108_putPixel : KS108_clrPixel;

	    while (x >= y)
	    {
	      switch(one_eight)
	      {
	      	  case 0: pixelDrawFuncPtr((uint8_t)(x0 + x), (uint8_t)(y0 - y)); break;
	      	  case 1: pixelDrawFuncPtr((uint8_t)(x0 + y), (uint8_t)(y0 - x)); break;
	      	  case 2: pixelDrawFuncPtr((uint8_t)(x0 - y), (uint8_t)(y0 - x)); break;
	      	  case 3: pixelDrawFuncPtr((uint8_t)(x0 - x), (uint8_t)(y0 - y)); break;
	      	  case 4: pixelDrawFuncPtr((uint8_t)(x0 - x), (uint8_t)(y0 + y)); break;
	      	  case 5: pixelDrawFuncPtr((uint8_t)(x0 - y), (uint8_t)(y0 + x)); break;
	      	  case 6: pixelDrawFuncPtr((uint8_t)(x0 + y), (uint8_t)(y0 + x)); break;
	      	  case 7: pixelDrawFuncPtr((uint8_t)(x0 + x), (uint8_t)(y0 + y)); break;
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

void KS108_drawRoundedRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t corner_radius, KS108_Color_t color)
{
	if( ( (x + width) > (KS108_WIDTH - 1) ) || ( (y + height) > (KS108_HEIGHT - 1) ) ) return;

	if( ((corner_radius) > (width>>1)) || ((corner_radius) > (height>>1)) ) return;

	uint8_t r = corner_radius;



	KS108_drawArc( x+width-r, y + r, r , 0, color);
	KS108_drawArc( x+width-r, y + r, r , 1, color);
	KS108_drawArc( x + r, y + r, r , 2, color);
	KS108_drawArc( x + r, y + r, r , 3, color);
	KS108_drawArc( x+width-r, y+height-r, r , 6, color);
	KS108_drawArc( x+width-r, y+height-r, r , 7, color);
	KS108_drawArc( x+r, y+height-r, r , 4, color);
	KS108_drawArc( x+r, y+height-r, r , 5, color);

	KS108_drawLine( x+r, y, x+width-r , y ,color);
	KS108_drawLine( x+width, y + r , x+width , y+height - r , color);
	KS108_drawLine( x+r, y+height , x+width-r , y+height , color);
	KS108_drawLine( x, y+r, x, y+height-r, color);
}

void KS108_loadBitmap(const uint8_t * picture)
{
	for(uint8_t j = 0; j < 8; j++)
	{
		for(uint8_t i = 0; i < 128; i++)
		{
			KS108_displayBuf[j][i] = *picture++;
		}
	}
}

void KS108_loadSmallBitmap(const uint8_t *picture, uint8_t height, uint8_t width, uint8_t pos_x, uint8_t pos_y)
{
	uint8_t current_byte;

	for(uint8_t y = 0; y < height; y++)
	{
		for(uint8_t x = 0; x < width; x++)
		{
			current_byte = *(picture + y*(width/8) + x/8);

			if(0x80 & (current_byte<<(x%8)))
			{
				KS108_putPixel(pos_x + x, pos_y + y);
			}
			else
			{
				KS108_clrPixel(pos_x + x, pos_y + y);
			}
		}
	}
}

void KS108_putChr(char data, uint8_t x, uint8_t y, uint8_t size, KS108_Color_t color )
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
          if( byte_from_flash & (1<<x) ) KS108_drawFilledSquare(tmp_x,tmp_y,size, color);
          else KS108_drawFilledSquare(tmp_x,tmp_y,size, color ^ (1<<0) );
          tmp_y += size;
      }
      tmp_y = y;
      tmp_x += size;

   }
}



void KS108_putStr(char * data, uint8_t x, uint8_t y, uint8_t size, KS108_Color_t color )
{
    char ascii = 0;
    uint8_t shift_x = 0;

    shift_x = 5 * size + 1;

    while( (ascii = *data++) )
    {
        KS108_putChr(ascii,x,y,size,color);
        if(!color)
        {
           	for(uint8_t i = y ; i < (y + 8); i++)
           	{
           		KS108_putPixel(x+(5*size),y+i);
           	}
        }
        x += shift_x;

    }
}

void KS108_putInt(int32_t num , uint8_t x, uint8_t y, uint8_t size, KS108_Color_t color )
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

    KS108_putStr(temp_buffer,x,y,size,color);

    free(temp_buffer);
}


void KS108_putHex(int32_t num , uint8_t x, uint8_t y, uint8_t size, KS108_Color_t color )
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

    KS108_putStr(temp_buffer,x,y,size,color);

    free(temp_buffer);
}

void KS108_putFloat( float num , uint8_t precision, uint8_t x, uint8_t y, uint8_t size, KS108_Color_t color )
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

    KS108_putStr(f.wsk,x,y,size,color);

    free(f.wsk);
}


