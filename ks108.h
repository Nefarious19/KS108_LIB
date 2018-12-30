/****************************************************************
 * @file   KS108.h
 *
 * @author Nefarious19
 * @date   01.08.2018
 * @brief  This is library for KS0108 based displays.
 *  	   It was developed for STM32F0 and STM32F1 microcontrolers.
 *		   You can use it for free.
 *		   Hardware: STM32F1 and STM32F0 family
 */

#ifndef KS108_KS108_H_
#define KS108_KS108_H_

/**********************************************************************
 * please include here the header for your uC family (stm32f1xx.h or stm32f0xx.h)
 *********************************************************************/
#include "stm32f1xx.h"
//#include "stm32f0xx.h"

//do not change position of this include.
#include "internal/ks108_utilities.h"

/****************************************************************
 * Configuration of API
 *
 * #define's that you can use to configure what API will be
 * compiled
 * if you don't need to use this features (for any reasons)
 * leave it as it is. Library works fine without them.
 ****************************************************************/
#define USE_HARDWARE_RESET				KS108_FEATURE_OFF
#define USE_READ_FROM_PORT_FEATURES 	KS108_FEATURE_OFF
#define USE_WAITING_FOR_BUSY_FLAG_OFF	KS108_FEATURE_OFF

/**********************************************************************
 * Please provide informations about pins assignment. IT'S IMPORTANT! :)
 *********************************************************************/
#define	DATA_D0  	GPIOB, 0
#define	DATA_D1  	GPIOB, 1
#define	DATA_D2  	GPIOB, 2
#define DATA_D3  	GPIOB, 3
#define DATA_D4  	GPIOB, 4
#define DATA_D5  	GPIOB, 5
#define DATA_D6  	GPIOB, 6
#define DATA_D7  	GPIOB, 7

#define CTRL_RW  	GPIOB, 8
#define CTRL_DI  	GPIOB, 9
#define CTRL_E   	GPIOB, 10
#define CTRL_CS1    GPIOB, 11
#define CTRL_CS2    GPIOB, 12
#define CTRL_BACK   GPIOA, 11

#if USE_HARDWARE_RESET == KS108_FEATURE_ON
#define CTRL_RST    GPIOB, 13
#endif


/**********************************************************************
 * Basic API - some stuff for basic operations on display.
 *********************************************************************/
void KS108_InitDisplay (void(*gpio_clocks_init_function_ptr)(void));
uint8_t * KS108_GetBufferPtr(void);
void KS108_sendBuffer (uint8_t * display_buf);
void KS108_WriteInstruction (uint8_t instruction, KS108_CSNumber CS_nr );
void KS108_WriteData (uint8_t data, KS108_CSNumber CS_nr);

#if USE_READ_FROM_PORT_FEATURES == KS108_FEATURE_ON
uint8_t KS108_ReadDisplayData (uint8_t line, uint8_t x );
#endif

/**********************************************************************
 * Graphics API - please note that after any change in display buffer,
 * display must be refreshed with
 * KS108_sendBuffer(KS108_GetBufferPtr()) command.
 *********************************************************************/
void KS108_putPixel ( uint8_t x, uint8_t y );
void KS108_clrPixel ( uint8_t x, uint8_t y );
void KS108_drawLine(uint8_t x_0, uint8_t y_0, uint8_t x_1, uint8_t y_1  , KS108_Color_t color);
void KS108_fillDisplay(uint8_t pattern);
void KS108_drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height , KS108_Color_t color);
void KS108_drawSquare(uint8_t x, uint8_t y, uint8_t a , KS108_Color_t color);
void KS108_drawFilledRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height , KS108_Color_t color);
void KS108_drawFilledSquare(uint8_t x, uint8_t y, uint8_t a , KS108_Color_t color);
void KS108_drawCircle(uint8_t x, uint8_t y, uint8_t r , KS108_Color_t color);
void KS108_drawFilledCircle(uint8_t x0, uint8_t y0, uint8_t radius , KS108_Color_t color);
void KS108_drawArc(uint8_t x0, uint8_t y0, uint8_t r, uint8_t one_eight  , KS108_Color_t color);
void KS108_drawRoundedRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t corner_radius , KS108_Color_t color);
void KS108_loadBitmap(const uint8_t * picture);
void KS108_loadSmallBitmap(const uint8_t *picture, uint8_t height, uint8_t width, uint8_t pos_x, uint8_t pos_y);

/**********************************************************************
 * Text and numbers printing API - please note that after any change
 * in display buffer, display must be refreshed with
 * KS108_sendBuffer(KS108_GetBufferPtr()) command.
 *********************************************************************/

void KS108_putChr(char   data, uint8_t x, uint8_t y, uint8_t size , KS108_Color_t color);
void KS108_putStr(char * data, uint8_t x, uint8_t y, uint8_t size , KS108_Color_t color);
void KS108_putInt(int32_t num , uint8_t x, uint8_t y, uint8_t size , KS108_Color_t color);
void KS108_putHex(int32_t num , uint8_t x, uint8_t y, uint8_t size , KS108_Color_t color);
void KS108_putFloat( float num , uint8_t precision, uint8_t x, uint8_t y, uint8_t size , KS108_Color_t color);

#endif /* KS108_KS108_H_ */
