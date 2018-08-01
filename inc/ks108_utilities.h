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

/****************************************************************
 * @brief Configuration of API
 *
 * #define's that you can use to configure what API will be
 * compiled
 ****************************************************************/
#define USE_READ_FROM_PORT_FEATURES 0

/****************************************************************
 * @brief SBBVAL and CBBVAL macros used for bit manipulation
 ****************************************************************/
#define  SBBVAL( value_to_shift, nr_of_beginning , begining_mul )   (value_to_shift<<(nr_of_beginning*begining_mul))
#define  CBBVAL( value_to_shift, nr_of_beginning , begining_mul )   (~(value_to_shift<<(nr_of_beginning*begining_mul)))


/****************************************************************
 * @brief Here you can assign your pin'o'logy
 * This version of library supports situation when data pins are
 * located on the same port and on pins from 0 to 7 (8 bits).
 * This will be changed in future but universality of pins assigment
 * will decrease library performance.
 ****************************************************************/
#define DATA_PORT  GPIOB
#define CTRL_PORT  GPIOB
#define    BACKL_PORT GPIOA

#define  DATA_D0    0
#define  DATA_D1    1
#define  DATA_D2    2
#define  DATA_D3    3
#define  DATA_D4    4
#define  DATA_D5    5
#define  DATA_D6    6
#define  DATA_D7    7

#define  CTRL_CS1    11
#define  CTRL_CS2    12
#define  CTRL_RW    8
#define  CTRL_DI    9
#define  CTRL_E        10
#define  CTRL_RST   13

#define  BACKL_GPIO 11


/****************************************************************
 * @brief pseudo macro-functions that are used to manipulate
 * control pins
 ****************************************************************/
#define KS108_Backlight_ON()  BACKL_PORT->ODR &= CBBVAL(0x01,BACKL_GPIO,1)
#define KS108_Backlight_OFF() BACKL_PORT->ODR |= SBBVAL(0x01,BACKL_GPIO,1)


#define KS108_CS1_LO    CTRL_PORT->ODR &= ~(1<<CTRL_CS1)
#define KS108_CS1_HI    CTRL_PORT->ODR |= (1<<CTRL_CS1)

#define KS108_CS2_LO    CTRL_PORT->ODR &= ~(1<<CTRL_CS2)
#define KS108_CS2_HI    CTRL_PORT->ODR |= (1<<CTRL_CS2)


#define KS108_CS_Off() {                                  \
                          CTRL_PORT->ODR |= (1<<CTRL_CS1);\
                          CTRL_PORT->ODR |= (1<<CTRL_CS2);\
                       }


#define KS108_DI_HI   CTRL_PORT->ODR |= (1<<CTRL_DI)
#define KS108_DI_LO   CTRL_PORT->ODR &= ~(1<<CTRL_DI)

#define KS108_RW_HI  CTRL_PORT->ODR |= (1<<CTRL_RW)
#define KS108_RW_LO  CTRL_PORT->ODR &= ~(1<<CTRL_RW)

#define KS108_CLR_E     CTRL_PORT->ODR &= ~(1<<CTRL_E)
#define KS108_SET_E     CTRL_PORT->ODR |= (1<<CTRL_E)

#define KS108_CLR_RST CTRL_PORT->ODR &= ~(1<<CTRL_RST)
#define KS108_SET_RST CTRL_PORT->ODR |= (1<<CTRL_RST)


/****************************************************************
 * @brief KS0108 commands
 * KS0108 display commands that are used to configure display
 ****************************************************************/
#define KS108_WIDTH  ( 128 )
#define KS108_HEIGHT ( 64  )
#define KS108_LINES  ( KS108_HEIGHT / 8 )

#define KS108_DISPLAY_ON    0x3F
#define KS108_DISPLAY_OFF    0x3E

#define KS108_SET_Y_ADR(y_adr) (0x40 | y_adr)
#define KS108_SET_X_ADR(x_adr) (0xB8 | x_adr)
#define KS108_SET_Z_ADR(z_adr) (0xC0 | z_adr)

#define KS108_DISPLAY_BSY   0x80

/****************************************************************
 * @brief UNUSED() MACRO
 * UNUSED() macro that can be used to avoid copmiler warnigs
 * caused by unused parameters
 ****************************************************************/
#define UNUSED(x){ \
        x = x + 0; \
        }


#endif /* KS108_KS108_UTILITIES_H_ */
