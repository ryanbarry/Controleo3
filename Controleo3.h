// Written by Peter Easton
// Released under CC BY-NC-SA 3.0 license
// Build a reflow oven: http://whizoo.com
//
// This library controls the functions of ControLeo3
//  - LCD display and backlight
//  - Touchscreen
//  - Buzzer
//  - Thermocouple

#ifndef CONTROLEO3_H_
#define CONTROLEO3_H_

#include "Arduino.h"

// Include all the code for the display, touch, and thermocouple
#include "Controleo3MAX31856.h"
#include "Controleo3Touch.h"

#if defined(TEENSYDUINO)
#define WHITE ILI9341_WHITE
#define RED ILI9341_RED
#define BLACK ILI9341_BLACK
#define BLUE ILI9341_BLUE
#define LIGHT_GREY 0xC618

#define LCD_WIDTH  		320
#define LCD_HEIGHT 		240
#define LCD_MAX_X		319
#define LCD_MAX_Y		239
#else
#define LCD_WIDTH  		480
#define LCD_HEIGHT 		320
#define LCD_MAX_X		479
#define LCD_MAX_Y		319
#endif

#define BUZZER_PIN              MISO

#endif // CONTROLEO3_H_
