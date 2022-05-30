#include "leds.h"
#include "logger.h"
#include "obd.h"




#define Serial Serial




bool useSD   = false;
bool useOLED = true;

float rpm = 0;
float mph = 0;




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

  switch (obd_state)
  {
    case ENG_RPM:
    {
      rpm = myELM327.rpm();
      
      if (myELM327.nb_rx_state == ELM_SUCCESS)
      {
        updateRpmDisp(rpm);
        dispData(mph, rpm);
        
        obd_state = SPEED;
      }
      else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        initRpmDisp();
        myELM327.printError();
        dispErr();
        
        obd_state = SPEED;
      }
      
      break;
    }
    
    case SPEED:
    {
      mph = myELM327.mph();
      
      if (myELM327.nb_rx_state == ELM_SUCCESS)
      {
        updateSpeedDisp(mph);
        dispData(mph, rpm);
        
        obd_state = ENG_RPM;
      }
      else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        updateSevenSeg(0, 10);
        updateSevenSeg(1, 10);
        updateSevenSeg(2, 10);
        
        myELM327.printError();
        dispErr();
        
        obd_state = ENG_RPM;
      }
      
      break;
    }
  }
}
