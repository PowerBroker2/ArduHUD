// 1D,A5,1F16BB

//#define ATMODE 1

void setup()
{
  Serial.begin(115200);
  while(!Serial);

#ifdef ATMODE
  Serial2.begin(38400);
#else
  Serial2.begin(115200);
#endif

  while(!Serial2);
  Serial2.println("AT SP 0");
}

void loop()
{
  byte c;

 #ifdef ATMODE
  if(Serial.available())
  {
    c = Serial.read();
    Serial.write(c);
    Serial2.write(c);
  }

  if(Serial2.available())
  {
    c = Serial2.read();
    Serial.write(c);
  }
#else
  Serial2.println("010C");
  
  while(Serial2.available())
  {
    c = Serial2.read();
    Serial.write(c);
  }
  delay(100);
#endif
}
