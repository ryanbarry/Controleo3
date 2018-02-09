// Written by Peter Easton
// Released under CC BY-NC-SA 3.0 license
// Build a reflow oven: http://whizoo.com
//

// Preferences are stored in 4K blocks.  There are 4 blocks, and they are rotated for each write to
// maximixe redundancy and minimize write wear.  Blocks have a 32-bit sequence number as the first
// 4 bytes.  This number is increased after each write to be able to identify the latest prefs.
// Blocks are initialized to 0xFF's after erase, so prefrences should be added with this in mind.


#define NO_OF_PREFS_BLOCKS          4
#define PAGES_PER_PREFS_BLOCK       16

uint8_t lastPrefsBlock = 0;
uint32_t timeOfLastSavePrefsRequest = 0;

void getPrefs() 
{
  // Sanity check on the size of the prefences
  if (sizeof(prefs) > 4096)
  {
    //SerialUSB.println("Prefs size must not exceed 4Kb!!!!!!!!");
    // Hopefully getting a massive delay while developing new code alert you to this situation 
    delay(10000);
  }
  
  // Get the prefs with the highest sequence number
  uint8_t prefsToUse = 0;

  // If this is the first time the prefs are read in, initialize them
  if (prefs.sequenceNumber == 0xFFFFFFFF) {
    // Initialize the whole of prefs to zero.
    uint8_t *p = (uint8_t *) &prefs;
    for (uint32_t i=0; i < sizeof(prefs); i++)
      *p++ = 0;

    // Only need to initialize non-zero prefs here
    prefs.bakeTemperature = BAKE_MIN_TEMPERATURE;
    prefs.bakeDuration = 31;  // 1 hour
    prefs.openDoorAfterBake = BAKE_DOOR_OPEN_CLOSE_COOL;

    prefs.lastUsedProfileBlock = FIRST_PROFILE_BLOCK;
  }

  //SerialUSB.println("Read prefs from block " + String(prefsToUse) + ". Seq No = " + String(prefs.sequenceNumber));

/* Defaults for oven in build guide
  prefs.learningComplete = true;
  prefs.learnedPower[0] = 13;
  prefs.learnedPower[1] = 32;
  prefs.learnedPower[2] = 28;
  prefs.learnedInertia[0] = 41;
  prefs.learnedInertia[1] = 136;
  prefs.learnedInertia[2] = 57;
  prefs.learnedInsulation = 124;*/
  
  // Remember which block was last used to save prefs
  lastPrefsBlock = prefsToUse;
}

// Save the prefs to external flash
void savePrefs()
{
  // Don't write prefs immediately.  Wait a short time in case the user is moving quickly between items
  // Doing this will reduce the number of writes to the flash, saving flash wear.
  // It also makes "tap and hold" more responsive.
  timeOfLastSavePrefsRequest = millis();
}


// This performs a factory reset, erasing preferences and profiles
void factoryReset(boolean saveTouchCalibrationData)
{
  uint32_t sequenceNumber = prefs.sequenceNumber;
  
  // Save the touchscreen calibration data
  memcpy(buffer100Bytes, &prefs.topLeftX, 16);
  // Get the factory-default prefs from flash
  // TODO: set factory-default prefs hardcoded since no flash
  getPrefs();
  // Restore the touchscreen data if touchscreen calibration data should be saved
  if (saveTouchCalibrationData)
    memcpy(&prefs.topLeftX, buffer100Bytes, 16);
  // Restore sequence number
  prefs.sequenceNumber = sequenceNumber;
  //TODO: write prefs somewhere?
  //writePrefsToFlash();
}

