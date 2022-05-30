#include "leds.h"
#include "logger.h"
#include "obd.h"




#define Serial Serial




bool useSD   = false;
bool useOLED = true;




void setup()
{
  Serial.begin(115200);
  ELM_PORT.begin(9600);

  initELM();

  if (useSD)
    useSD = setupSD();

  if (useOLED)
    initOLED();

  initSevenSeg(0);
  initSevenSeg(1);
  initSevenSeg(2);
  initRpmDisp();
}




void loop()
{
  if (useSD)
    myTerminal.handleCmds();
  
  float mph = getMPH();
  float rpm = getRPM();

  if ((mph != ERR) && (rpm != ERR))
  {
    updateSpeedDisp(mph);
    updateRpmDisp(rpm);

    dispData(mph, rpm);
  }
  else
  {
    updateSpeedDisp(0);
    updateRpmDisp(0);

    dispErr();
  }
}
