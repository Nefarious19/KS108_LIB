/****************************************************************
 * @file   KS108_utilities.h
 *
 * @author Nefarious19
 * @date   01.08.2018
 * @brief  This file contains some #defines used by other source
 *         files, some API configuration options and pin
 *         assigment definitions.
 *
 *
 ****************************************************************/
#ifndef KS108_KS108_UTILITIES_H_
#define KS108_KS108_UTILITIES_H_

typedef uint8_t KS108_pinNo_t;

typedef enum
{
	KS108_COLOR_BLACK,
	KS108_COLOR_WHITE
}KS108_Color_t;

typedef struct
{
	GPIO_TypeDef *   gpio_reg;
	KS108_pinNo_t    pinNumber;
}KS108_pinMapping_t;

typedef enum
{
	CS1,
	CS2
}KS108_CSNumber;

typedef enum
{
	KS108_DATA_D0,
	KS108_DATA_D1,
	KS108_DATA_D2,
	KS108_DATA_D3,
	KS108_DATA_D4,
	KS108_DATA_D5,
	KS108_DATA_D6,
	KS108_DATA_D7,
	KS108_CTRL_RW,
	KS108_CTRL_DI,
	KS108_CTRL_E,
	KS108_CTRL_CS1,
	KS108_CTRL_CS2,
	KS108_CTRL_BACK,
	KS108_CTRL_RST,
}KS108_PinName_t;

#define FONT_WIDTH 	5
#define FONT_HEIGHT	7

#define CHAR_WIDTH   (FONT_WIDTH + 2)
#define CHAR_HEIGHT  (FONT_HEIGHT + 3)

typedef enum
{
	KS108_LINE1 = 0,
	KS108_LINE2 = CHAR_HEIGHT*1,
	KS108_LINE3 = CHAR_HEIGHT*2,
	KS108_LINE4 = CHAR_HEIGHT*3,
	KS108_LINE5 = CHAR_HEIGHT*4,
	KS108_LINE6 = CHAR_HEIGHT*5,
}KS108_LineNo_t;

#define KS108_FEATURE_ON 	1
#define KS108_FEATURE_OFF   0
/****************************************************************
 * @brief SBBVAL and CBBVAL macros used for bit manipulation
 ****************************************************************/
#define  SBBVAL( value_to_shift, nr_of_beginning , begining_mul )   (value_to_shift<<(nr_of_beginning*begining_mul))
#define  CBBVAL( value_to_shift, nr_of_beginning , begining_mul )   (~(value_to_shift<<(nr_of_beginning*begining_mul)))

/****************************************************************
 * @brief pseudo macro-functions that are used to manipulate
 * control pins
 ****************************************************************/

#define KS108_ModifyRegister( reg, setMask, clrMask, maskPos)  \
{ 																\
  reg = (reg & (~(clrMask<<(maskPos)))) | (setMask<<(maskPos));		\
}

#define KS108_SetCtrlPin(reg, pin) 	reg |= (1<<pin)
#define KS108_ClrCtrlPin(reg, pin)  reg &= ~(1<<pin)




/****************************************************************
 * @brief KS0108 commands
 * KS0108 display commands that are used to configure display
 ****************************************************************/
#define KS108_WIDTH  ( 128 )
#define KS108_HEIGHT ( 64  )
#define KS108_LINES  ( KS108_HEIGHT / 8 )

#define KS108_DISPLAY_ON    0x3F
#define KS108_DISPLAY_OFF   0x3E

#define KS108_SET_Y_ADR(y_adr) (0x40 | y_adr)
#define KS108_SET_X_ADR(x_adr) (0xB8 | x_adr)
#define KS108_SET_Z_ADR(z_adr) (0xC0 | z_adr)

#define KS108_DISPLAY_BSY   0x80



/****************************************************************
 * UNUSED() macro that can be used to avoid copmiler warnings
 * caused by unused parameters
 ****************************************************************/
#define UNUSED(x)	   \
		{              \
			x = x + 0; \
        }


#endif /* KS108_KS108_UTILITIES_H_ */
