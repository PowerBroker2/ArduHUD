#include "BluetoothSerial.h"
#include "SerialTransfer.h"
#include "ELMduino.h"


BluetoothSerial SerialBT;
#define DEBUG_PORT      Serial
#define LED_DRIVER_PORT Serial2
#define ELM_PORT        SerialBT


SerialTransfer myTransfer;
ELM327 myELM327;


enum fsm{get_speed, 
         get_rpm};
fsm state = get_rpm;


struct STRUCT {
  int8_t status;
  uint32_t rpm;
  float mph;
} carTelem;


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Wait for ELM327 to init
  delay(3000);
  
  DEBUG_PORT.begin(115200);
  LED_DRIVER_PORT.begin(115200);
  ELM_PORT.begin("ArduHUD", true);
  
  if (!ELM_PORT.connect("OBDII"))
  {
    DEBUG_PORT.println("Couldn't connect to OBD scanner");
    while(1);
  }
  
  myELM327.begin(ELM_PORT);
  myTransfer.begin(LED_DRIVER_PORT);

  while(!myELM327.connected)
    myELM327.initializeELM();
}


void loop()
{
  switch(state)
  {
    case get_rpm:
    {
      float tempRPM = myELM327.rpm();

      carTelem.status = myELM327.status;
      
      if(myELM327.status == ELM_SUCCESS)
        carTelem.rpm = tempRPM;
      else
        printError();
      
      state = get_speed;
      break;
    }

    case get_speed:
    {
      float tempMPH = myELM327.mph();

      carTelem.status = myELM327.status;
      
      if(myELM327.status == ELM_SUCCESS)
        carTelem.mph = tempMPH;
      else
        printError();
      
      state = get_rpm;
      break;
    }
  }

  myTransfer.txObj(carTelem, sizeof(carTelem));
  myTransfer.sendData(sizeof(carTelem));
}


void printError()
{
  DEBUG_PORT.print("Received: ");
  for (byte i = 0; i < PAYLOAD_LEN; i++)
    DEBUG_PORT.write(myELM327.payload[i]);
  DEBUG_PORT.println();
  
  if (myELM327.status == ELM_SUCCESS)
    DEBUG_PORT.println(F("\tELM_SUCCESS"));
  else if (myELM327.status == ELM_NO_RESPONSE)
    DEBUG_PORT.println(F("\tERROR: ELM_NO_RESPONSE"));
  else if (myELM327.status == ELM_BUFFER_OVERFLOW)
    DEBUG_PORT.println(F("\tERROR: ELM_BUFFER_OVERFLOW"));
  else if (myELM327.status == ELM_UNABLE_TO_CONNECT)
    DEBUG_PORT.println(F("\tERROR: ELM_UNABLE_TO_CONNECT"));
  else if (myELM327.status == ELM_NO_DATA)
    DEBUG_PORT.println(F("\tERROR: ELM_NO_DATA"));
  else if (myELM327.status == ELM_STOPPED)
    DEBUG_PORT.println(F("\tERROR: ELM_STOPPED"));
  else if (myELM327.status == ELM_TIMEOUT)
    DEBUG_PORT.println(F("\tERROR: ELM_TIMEOUT"));
  else if (myELM327.status == ELM_TIMEOUT)
    DEBUG_PORT.println(F("\tERROR: ELM_GENERAL_ERROR"));

  delay(100);
}

