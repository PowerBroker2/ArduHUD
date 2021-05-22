#include "BluetoothSerial.h"
#include "SerialTransfer.h"
#include "ELMduino.h"




BluetoothSerial SerialBT;
#define DEBUG_PORT      Serial
#define LED_DRIVER_PORT Serial2
#define ELM_PORT        SerialBT




const uint16_t MAX_RPM = 3500;
const uint16_t MIN_RPM = 700;
const uint8_t MSG_LEN = 16;




SerialTransfer myTransfer;
ELM327 myELM327;




enum fsm{get_speed, 
         get_rpm};
fsm state = get_rpm;

struct STRUCT {
  int8_t status;
  uint32_t rpm;
  float mph;
  char msg[MSG_LEN];
} carTelem;




void setup()
{
#if LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif

  DEBUG_PORT.begin(115200);
  LED_DRIVER_PORT.begin(115200);
  ELM_PORT.begin("ArduHUD", true);
  
  if (!ELM_PORT.connect("OBDII"))
  {
    DEBUG_PORT.println("Couldn't connect to OBD scanner");
    while(1);
  }
  
  myTransfer.begin(LED_DRIVER_PORT);

  while(!myELM327.begin(ELM_PORT))
    delay(500);
}




void loop()
{
  switch(state)
  {
    case get_rpm:
    {
      float tempRPM = myELM327.rpm();

      carTelem.status = myELM327.status;
      memcpy(carTelem.msg, myELM327.payload, MSG_LEN);
      
      if(myELM327.status == ELM_SUCCESS)
        carTelem.rpm = tempRPM;
      else
        myELM327.printError();
      
      state = get_speed;
      break;
    }

    case get_speed:
    {
      float tempMPH = myELM327.mph();

      carTelem.status = myELM327.status;
      memcpy(carTelem.msg, myELM327.payload, MSG_LEN);
      
      if(myELM327.status == ELM_SUCCESS)
        carTelem.mph = tempMPH;
      else
        myELM327.printError();
      
      state = get_rpm;
      break;
    }
  }
  
  myTransfer.sendDatum(carTelem);
}
