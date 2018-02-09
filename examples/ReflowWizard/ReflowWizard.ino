// Written by Peter Easton
// Released under CC BY-NC-SA 3.0 license
// Build a reflow oven: http://whizoo.com
//

#include <Arduino.h>
#include <Controleo3.h> // Hardware-specific library
#include "ReflowWizard.h"

// Global temporary buffers (used everywhere)
char buffer100Bytes[100];
uint8_t flashBuffer256Bytes[256];     // Read/write from flash.  This is the size of a flash block

Controleo3LCD tft;
Controleo3Touch  touch;
Controleo3Flash  flash;
Controleo3MAX31856 thermocouple;


void setup(void) {
  // First priority - turn off the relays!
  initOutputs();

  flash.begin();
  getPrefs();
  factoryReset(false);

  // Get the splash screen up as quickly as possible
  tft.begin();
  //flash.begin();

  // Display the initial splash screen
  tft.pokeRegister(ILI9488_DISPLAYOFF);
  tft.fillScreen(WHITE);
  renderBitmap(BITMAP_CONTROLEO3, 40, 10);
  renderBitmap(BITMAP_WHIZOO, 84, 200);
  displayString(42, 85, FONT_12PT_BLACK_ON_WHITE, (char *) "Reflow Oven Controller");
  displayString(420, 290, FONT_9PT_BLACK_ON_WHITE, (char *) CONTROLEO3_VERSION);
  tft.pokeRegister(ILI9488_DISPLAYON);
  playTones(TUNE_STARTUP);
  SerialUSB.begin(115200);

  // Get the prefs from external flash
  getPrefs();

  // Initialize the MAX31856's registers
  thermocouple.begin();
  initTemperature();

  // Initialize the timer used to control the servo and read the temperature
  initializeTimer();

  // Start the touchscreen
  touch.begin();
  
  // Move the servo to the closed position
  setServoPosition(prefs.servoClosedDegrees, 1000); 

  // Is there touchscreen calibration data?
  if (prefs.topLeftX != 0) {
    sendTouchCalibrationData();
    // Show the logos for a few seconds
    delay(3000);
  }
  else {
    // Touchscreen needs to be calibrated first
    CalibrateTouchscreen();
  }

  // Go to the first screen (this should never exit)
  if (areOutputsConfigured())
    showScreen(SCREEN_HOME);
  else  
    showScreen(SCREEN_SETTINGS);
}


void loop()
{
  // This should never be called (showScreen never exits)
  showScreen(SCREEN_HOME);
}


extern "C" char *sbrk(int i);
uint32_t getFreeRAM() {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}


// Free memory is checked every time there is a screen transition
// There should be NO leaks!  The microcontroller has 32K of RAM, and
// around 4K is used by glabal variables and strings.  The software
// (being written in "C" for the most part) is extremely frugal with
// memory and even during relow there should still be more than 20K
// remaining.  Take that Windows 10!
void checkFreeMemory()
{
  static uint32_t freeRAMOnBoot = getFreeRAM();

  // Check the amount of free RAM (Allow 500 bytes because some drivers like SD card consume RAM when they load)
  if (getFreeRAM() > freeRAMOnBoot - 500)
    return;

  // There has been a memory leak!
  sprintf(buffer100Bytes, "Free RAM was %ld and is now %ld", freeRAMOnBoot, getFreeRAM());
  SerialUSB.println(buffer100Bytes);
//  drawThickRectangle(80, 70, 320, 135, 10, RED);
//  displayString(114, 100, FONT_12PT_BLACK_ON_WHITE, (char *) "Memory Leak!!!");
//  sprintf(buffer100Bytes, "%d bytes lost", freeRAMOnBoot - getFreeRAM());
//  displayString(164, 150, FONT_9PT_BLACK_ON_WHITE, buffer100Bytes);
  freeRAMOnBoot = getFreeRAM();
//  delay(1000);
//  tft.fillScreen(WHITE);
}

