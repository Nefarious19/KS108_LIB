/*
 * ks108_gfx.h
 *
 *  Created on: 16.07.2018
 *      Author: rafal
 */

#ifndef KS108_KS108_GFX_H_
#define KS108_KS108_GFX_H_

void KS108_sendBuffer (uint8_t * display_buf);

void KS108_putPixel (uint8_t x, uint8_t y);
void KS108_drawLine(uint8_t x_0, uint8_t y_0, uint8_t x_1, uint8_t y_1 );
void KS108_fillDisplay(uint8_t pattern);
void KS108_drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void KS108_drawSquare(uint8_t x, uint8_t y, uint8_t a);
void KS108_drawFilledRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void KS108_drawFilledSquare(uint8_t x, uint8_t y, uint8_t a);
void KS108_drawCircle(uint8_t x, uint8_t y, uint8_t r);
void KS108_drawFilledCircle(uint8_t x0, uint8_t y0, uint8_t radius);
void KS108_drawArc(uint8_t x0, uint8_t y0, uint8_t r, uint8_t one_eight );
void KS108_drawRoundedRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t corner_radius);

void KS108_clrPixel (uint8_t x, uint8_t y);
void KS108_clrLine(uint8_t x_0, uint8_t y_0, uint8_t x_1, uint8_t y_1 );
void KS108_drawClrRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void KS108_drawClearedRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void KS108_drawClrSquare(uint8_t x, uint8_t y, uint8_t a);
void KS108_drawClearedSquare(uint8_t x, uint8_t y, uint8_t a);

#endif /* KS108_KS108_GFX_H_ */
