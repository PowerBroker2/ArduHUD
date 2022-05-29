#include "ELMduino.h"
#include "leds.h"
#include "logger.h"




#define DEBUG_PORT Serial
#define ELM_PORT   Serial1




const int ERR = -1e6;




ELM327 myELM327;




bool useSD   = false;
bool useOLED = true;




void setup()
{
  DEBUG_PORT.begin(115200);
  ELM_PORT.begin(115200);

  /*DEBUG_PORT.println("Attempting to connect to ELM327...");

  if (!myELM327.begin(ELM_PORT, false, 2000))
  {
    DEBUG_PORT.println("Couldn't connect to OBD scanner");
    while (1);
  }

  DEBUG_PORT.println("Connected to ELM327");*/

  if (useSD)
    useSD = setupSD();

  if (useOLED)
    initOLED();

  initSevenSeg(0);
  initSevenSeg(1);
  initSevenSeg(2);
  initRpmDisp();

  updateSpeedDisp(123);
  updateRpmDisp(1500);
}




void loop()
{
  myTerminal.handleCmds();
  
  /*float mph = getMPH();
  float rpm = getRPM();

  if ((mph != ERR) && (rpm != ERR))
  {
    updateSpeedDisp(mph);
    updateRpmDisp(rpm);
  }
  else
  {
    
  }*/
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
