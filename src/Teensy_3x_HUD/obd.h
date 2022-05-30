#pragma once

#include "ELMduino.h"




#define ELM_PORT Serial1




const bool DEBUG        = false;
const int  TIMEOUT      = 2000;
const bool HALT_ON_FAIL = false;
const int  ERR          = -1e6;




ELM327 myELM327;




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




float getMPH()
{
  float mph = myELM327.mph();

  if (myELM327.nb_rx_state == ELM_SUCCESS)
    return mph;
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    myELM327.printError();

  return ERR;
}




float getRPM()
{
  float rpm = myELM327.rpm();

  if (myELM327.nb_rx_state == ELM_SUCCESS)
    return rpm;
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    myELM327.printError();

  return ERR;
}
