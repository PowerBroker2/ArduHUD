#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "BluetoothSerial.h"
#include "SerialTransfer.h"
#include "ELMduino.h"


BluetoothSerial SerialBT;
#define DEBUG_PORT      Serial
#define LED_DRIVER_PORT Serial2
#define ELM_PORT        SerialBT


const char *ssid = "HUDuino";
const char MAIN_page[] PROGMEM = "<!DOCTYPE html>\n<html>\n<style>\n.card{\n    max-width: 400px;\n     min-height: 250px;\n     background: #02b875;\n     padding: 30px;\n     box-sizing: border-box;\n     color: #FFF;\n     margin:20px;\n     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);\n}\n</style>\n\n<body>\n<div class=\"card\">\n  <h4>The ESP32 Update web page without refresh</h4><br>\n  <h1>Sensor Value: <span id=\"ADCValue\">0</span></h1><br>\n</div>\n</body>\n\n<script>\nsetInterval(function() {\n  // Call a function repetatively with 0.1 Second interval\n  getData();\n}, 100); //100mSeconds update rate\n\nfunction getData() {\n  var xhttp = new XMLHttpRequest();\n  xhttp.onreadystatechange = function() {\n    if (this.readyState == 4 && this.status == 200) {\n      document.getElementById(\"ADCValue\").innerHTML =\n      this.responseText;\n    }\n  };\n  xhttp.open(\"GET\", \"readADC\", true);\n  xhttp.send();\n}\n</script>\n</html>";


SerialTransfer myTransfer;
WiFiServer server(80);
ELM327 myELM327;


enum fsm{get_speed, 
         get_rpm};
fsm state = get_rpm;


struct STRUCT {
  uint32_t rpm;
  float mph;
} carTelem;


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  DEBUG_PORT.begin(115200);
  LED_DRIVER_PORT.begin(115200);
  ELM_PORT.begin("ESP32test", true);
  
  if (!ELM_PORT.connect("OBDII"))
  {
    DEBUG_PORT.println("Couldn't connect to OBD scanner");
    while(1);
  }

  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  DEBUG_PORT.println(myIP);
  server.begin();
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
      
      if(myELM327.status == ELM_SUCCESS)
      {
        carTelem.rpm = tempRPM;
        myTransfer.txObj(carTelem, sizeof(carTelem));
        myTransfer.sendData(sizeof(carTelem));
      }
      else
        printError();
      
      state = get_speed;
      break;
    }

    case get_speed:
    {
      float tempMPH = myELM327.mph();
      
      if(myELM327.status == ELM_SUCCESS)
      {
        carTelem.mph = tempMPH;
        myTransfer.txObj(carTelem, sizeof(carTelem));
        myTransfer.sendData(sizeof(carTelem));
      }
      else
        printError();
      
      state = get_rpm;
      break;
    }
  }

  serverProcessing();
}


void serverProcessing()
{
  bool main = false;
  bool adc = false;
  
  WiFiClient client = server.available();

  if (client)
  {
    String currentLine = "";
    
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        //Serial.write(c);

        if (currentLine.endsWith("GET / HTTP"))
          main = true;
        else if (currentLine.endsWith("GET /readADC HTTP"))
          adc = true;
        
        if (c == '\n')
        {
          if (!currentLine.length())
          {
            if (main)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
        
              client.print(MAIN_page);
              client.println();
      
              main = false;
              break;
            }
            else if (adc)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
        
              client.println(random(0, 1023));
              client.println();
      
              adc = false;
              break;
            }
            else
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
        
              client.print(MAIN_page);
              client.println();
              break;
            }
          }
          else
            currentLine = "";
        }
        else if (c != '\r')
          currentLine += c;
      }
    }

    client.stop();
  }
}


void printError()
{
  DEBUG_PORT.print("Received: ");
  for (byte i = 0; i < PAYLOAD_LEN; i++)
    DEBUG_PORT.write(myELM327.payload[i]);
  DEBUG_PORT.println();
  
  switch (myELM327.status)
  {
    case ELM_SUCCESS:
    {
      DEBUG_PORT.println(F("\tELM_SUCCESS"));
    }
    case ELM_NO_RESPONSE:
    {
      DEBUG_PORT.println(F("\tERROR: ELM_NO_RESPONSE"));
    }
    case ELM_BUFFER_OVERFLOW:
    {
      DEBUG_PORT.println(F("\tERROR: ELM_BUFFER_OVERFLOW"));
    }
    case ELM_GARBAGE:
    {
      DEBUG_PORT.println(F("\tERROR: ELM_GARBAGE"));
    }
    case ELM_UNABLE_TO_CONNECT:
    {
      DEBUG_PORT.println(F("\tERROR: ELM_UNABLE_TO_CONNECT"));
    }
    case ELM_NO_DATA:
    {
      DEBUG_PORT.println(F("\tERROR: ELM_NO_DATA"));
    }
    case ELM_STOPPED:
    {
      DEBUG_PORT.println(F("\tERROR: ELM_STOPPED"));
    }
    case ELM_TIMEOUT:
    {
      DEBUG_PORT.println(F("\tERROR: ELM_TIMEOUT"));
    }
    case ELM_GENERAL_ERROR:
    {
      DEBUG_PORT.println(F("\tERROR: ELM_GENERAL_ERROR"));
    }
  }

  delay(100);
}

