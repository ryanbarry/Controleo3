// Written by Peter Easton
// Released under CC BY-NC-SA 3.0 license
// Build a reflow oven: http://whizoo.com
//
// Touch controller for XPT2046


#ifndef CONTROLEO3TOUCH_H_
#define CONTROLEO3TOUCH_H_

#include "Arduino.h"
#include "bits.h"

#if defined(TEENSYDUINO)
// SCK0 is 13
#define TOUCH_CLK_ACTIVE        digitalWrite(13, HIGH);
#define TOUCH_CLK_IDLE		    digitalWrite(13, LOW);

// TOUCH_CS is 10
#define TOUCH_CS_ACTIVE         digitalWrite(10, HIGH);
#define TOUCH_CS_IDLE		    digitalWrite(10, LOW);

// MOSI0 is 11
#define TOUCH_MOSI_ACTIVE       digitalWrite(11, HIGH);
#define TOUCH_MOSI_IDLE         digitalWrite(11, LOW);

// MISO0 is 12
#define TOUCH_MISO_HIGH  		(digitalRead(12) == 1)

// PEN_IRQ is on 15
#define TOUCH_PEN_IRQ           (digitalRead(15) == 0)
#else
// CLK is D4 (PA8)
#define TOUCH_CLK_ACTIVE        (*portAOut |= SETBIT08)
#define TOUCH_CLK_IDLE		    (*portAOut &= CLEARBIT08)

// CS is D3 (PA9)
#define TOUCH_CS_ACTIVE         (*portAOut |= SETBIT09)
#define TOUCH_CS_IDLE			(*portAOut &= CLEARBIT09)

// MOSI is D1 (PA10)
#define TOUCH_MOSI_ACTIVE       (*portAOut |= SETBIT10)
#define TOUCH_MOSI_IDLE         (*portAOut &= CLEARBIT10)

// MISO is D0 (PA11)
#define TOUCH_MISO_HIGH  		(*portAIn & SETBIT11)

// PEN_IRQ is on PB10
#define TOUCH_PEN_IRQ           ((*portBIn & SETBIT10) == 0)
#endif

#define TOUCH_PULSE_CLK         { TOUCH_CLK_IDLE; TOUCH_CLK_ACTIVE; }

class Controleo3Touch
{
	public:
    	Controleo3Touch(void);

		void begin();
        void calibrate(int16_t,int16_t,int16_t,int16_t,int16_t,int16_t,int16_t,int16_t);
		bool read(int16_t *x, int16_t *y);
		bool readRaw(int16_t *x, int16_t *y);
        bool isPressed();

    private:
  		volatile uint32_t *portAOut, *portAIn, *portAMode, *portBOut, *portBIn, *portBMode;
        int16_t topLeftX,topRightX,bottomLeftX,bottomRightX,topLeftY,bottomLeftY,topRightY,bottomRightY;
		void write8(byte data);
		word read12();
        uint16_t calcDeviation(uint16_t *array, uint8_t num, int16_t *average);
};

#endif // CONTROLEO3TOUCH_H_
