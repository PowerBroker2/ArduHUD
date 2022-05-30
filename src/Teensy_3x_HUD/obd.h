#pragma once

#include "ELMduino.h"
#include "leds.h"




#define ELM_PORT Serial1




const bool DEBUG        = false;
const int  TIMEOUT      = 2000;
const bool HALT_ON_FAIL = false;
const int  ERR          = -1e6;




ELM327 myELM327;




typedef enum { ENG_RPM,
               SPEED } obd_pid_states;
obd_pid_states obd_state = ENG_RPM;




void initELM()
{
  Serial.println("Attempting to connect to ELM327...");

  if (!myELM327.begin(ELM_PORT, DEBUG, TIMEOUT))
  {
    Serial.println("Couldn't connect to OBD scanner");

    if (HALT_ON_FAIL)
      while (1);
  }

  Serial.println("Connected to ELM327");
}
