// Written by Peter Easton
// Released under CC BY-NC-SA 3.0 license
// Build a reflow oven: http://whizoo.com
//

// Read all the profiles from the SD card.  The profiles can be in sub-directories

// This is the instruction set.  The instructions must be unique
char *tokenString[NUM_TOKENS] = {(char *) "not_a_token", (char *) "name", (char *) "#", (char *) "//", (char *) "deviation", (char *) "maximum temperature",
                                 (char *) "initialize timer", (char *) "start timer", (char *) "stop timer", (char *) "maximum duty", (char *) "display",
                                 (char *) "open door", (char *) "close door", (char *) "bias", (char *) "convection fan on", (char *) "convection fan off",
                                 (char *) "cooling fan on", (char *) "cooling fan off", (char *) "ramp temperature", (char *) "element duty cycle",
                                 (char *) "wait for", (char *) "wait until above", (char *) "wait until below", (char *) "play tune", (char *) "play beep",
                                 (char *) "door percentage", (char *) "maintain"};
char *tokenPtr[NUM_TOKENS];


// Set the tokens back to the beginning to continue the search for them
void initTokenPtrs()
{
  for (uint8_t i=0; i < NUM_TOKENS; i++)
    tokenPtr[i] = tokenString[i];
}


// If the given character matches the required character in the token then advance
// the token search.  Return the found token if this was the last character
uint8_t hasTokenBeenFound(char c)
{
  // Make the token search case-insensitive
  c = tolower(c);
  // Start this loop at 1 because the first "token" is not-a-token
  for (uint8_t i=1; i < NUM_TOKENS; i++) {
    // Does this character match?
    if (c == *tokenPtr[i]) {
      // Advance the token pointer
      tokenPtr[i]++;
      // Is this the end of the string?
      if (*tokenPtr[i] == 0) {
        return i;
      }
    }
    else {
      // The character didn't match.  Reset the token pointer
      tokenPtr[i] = tokenString[i];
    }
  }
  // This wasn't the last character of any of the tokens
  return NOT_A_TOKEN;
}

// Convert the token to readable text
char *tokenToText(char *str, uint8_t token, uint16_t *numbers)
{
  *str = 0;
  switch (token) {
    case TOKEN_DEVIATION:
      sprintf(str, "Deviation to abort %dC", numbers[0]);
      break;
    case TOKEN_MAX_TEMPERATURE:
      sprintf(str, "Maximum temperature %dC", numbers[0]);
      break;
    case TOKEN_INITIALIZE_TIMER:
      sprintf(str, "Initialize timer to %d seconds", numbers[0]);
      break;
    case TOKEN_START_TIMER:
      strcpy(str, "Start timer");
      break;
    case TOKEN_STOP_TIMER:
      strcpy(str, "Stop timer");
      break;
    case TOKEN_MAX_DUTY:
      sprintf(str, "Maximum duty %d/%d/%d", numbers[0], numbers[1], numbers[2]);
      break;
    case TOKEN_OVEN_DOOR_OPEN:
      sprintf(str, "Open door over %d seconds", numbers[0]);
      break;
    case TOKEN_OVEN_DOOR_CLOSE:
      sprintf(str, "Close door over %d seconds", numbers[0]);
      break;
    case TOKEN_OVEN_DOOR_PERCENT:
      sprintf(str, "Door percentage %d%% over %d seconds", numbers[0], numbers[1]);
      break;
    case TOKEN_BIAS:
      sprintf(str, "Bias %d/%d/%d", numbers[0], numbers[1], numbers[2]);
      break;
    case TOKEN_CONVECTION_FAN_ON:
      strcpy(str, "Convection fan on");
      break;
    case TOKEN_CONVECTION_FAN_OFF:
      strcpy(str, "Convection fan off");
      break;
    case TOKEN_COOLING_FAN_ON:
      strcpy(str, "Cooling fan on");
      break;
    case TOKEN_COOLING_FAN_OFF:
      strcpy(str, "Cooling fan off");
      break;
    case TOKEN_TEMPERATURE_TARGET:
      sprintf(str, "Ramp temperature to %dC in %d seconds", numbers[0], numbers[1]);
      break;
    case TOKEN_MAINTAIN_TEMP:
      sprintf(str, "Maintain %dC for %d seconds", numbers[0], numbers[1]);
      break;
    case TOKEN_ELEMENT_DUTY_CYCLES:
      sprintf(str, "Element duty cycle %d/%d/%d", numbers[0], numbers[1], numbers[2]);
      break;
    case TOKEN_WAIT_FOR_SECONDS:
      sprintf(str, "Wait for %d seconds", numbers[0]);
      break;
    case TOKEN_WAIT_UNTIL_ABOVE_C:
      sprintf(str, "Wait until above %dC", numbers[0]);
      break;
    case TOKEN_WAIT_UNTIL_BELOW_C:
      sprintf(str, "Wait until below %dC", numbers[0]);
      break;
    case TOKEN_PLAY_DONE_TUNE:
      strcpy(str, "Play tune");
      break;
    case TOKEN_PLAY_BEEP:
      strcpy(str, "Play beep");
      break;
  }
  return str;  
}


// Return true if a profile of the same name exists
boolean profileExists(char *profileName)
{
  for (uint8_t i = 0; i< prefs.numProfiles; i++) {
    if (strcmp(profileName, prefs.profile[i].name) == 0)
      return true;
  }
  return false;
}

// Keep profiles in alphabetical order, with unused profiles at the end
// Yes, inefficient and slow.  But how often is this called?  Only when
// profiles are read from the SD card (and the SD card is really slow!)
void sortProfiles(void) 
{
  profiles temp;
  for (uint8_t i=0; i < MAX_PROFILES-1; i++) {
    for (uint8_t j=i+1; j < MAX_PROFILES; j++) {
      // Handle the simple case first
      if (prefs.profile[j].startBlock == 0)
        continue;
      // If the first profile is not valid then switch them
      if (prefs.profile[i].startBlock == 0) {
        //SerialUSB.println("Moving " + String(prefs.profile[j].name) + " (" + String(j) + ") to " + String(i));
        // Copy one block to the other
        memcpy(&prefs.profile[i], &prefs.profile[j], sizeof(struct profiles));
        // Zero out the old profile
        prefs.profile[j].startBlock = 0;
        continue;
      }
      // Both profiles are valid
      if (strcmp(prefs.profile[i].name, prefs.profile[j].name) <= 0)
        continue;
      // Swap the profiles
      //SerialUSB.println("Swapping " + String(prefs.profile[i].name) + " (" + String(i) + " and " + String(j) + ")");
      memcpy(&temp, &prefs.profile[i], sizeof(struct profiles));
      memcpy(&prefs.profile[i], &prefs.profile[j], sizeof(struct profiles));
      memcpy(&prefs.profile[j], &temp, sizeof(struct profiles));
    }
  }
  //SerialUSB.println("Profiles have been sorted");
  prefs.numProfiles = getNumberOfProfiles();
}


// Get the number of profiles.
uint8_t getNumberOfProfiles()
{
  uint8_t count = 0;
  
  for (uint8_t i = 0; i< MAX_PROFILES; i++) {
    // If the profile has a non-zero start block then it is valid
    if (prefs.profile[i].startBlock != 0)
      count++;
  }
  return count;  
}

