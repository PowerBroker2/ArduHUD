#include <SerialTransfer.h>


SerialTransfer myTransfer;


void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);

  myTransfer.begin(Serial2);
}


void loop()
{
  float myFloat;
  
  if(myTransfer.available())
  {
    myTransfer.rxObj(myFloat, sizeof(myFloat));
    
    Serial.print("New Data: ");
    Serial.println(myFloat);
  }
  else if(myTransfer.status < 0)
  {
    Serial.print("ERROR: ");
    Serial.println(myTransfer.status);
  }
}
