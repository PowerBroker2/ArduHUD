#include "SerialTransfer.h"
#include "ELMduino.h"


#define DEBUG_PORT      Serial
#define LED_DRIVER_PORT Serial2


SerialTransfer myTransfer;


const uint8_t speed_led_pin_array[3][7] = { //--------- one's place
                                           {1,   // A
                                            2,   // B
                                            3,   // C
                                            4,   // D
                                            5,   // E
                                            6,   // F
                                            7},  // G
                                            //--------- ten's place
                                           {8,   // A
                                            9,   // B
                                            10,  // C
                                            11,  // D
                                            12,  // E
                                            24,  // F
                                            25}, // G
                                            //--------- hundred's place
                                           {26,  // A
                                            27,  // B
                                            28,  // C
                                            29,  // D
                                            30,  // E
                                            31,  // F
                                            32}  // G
                                          };
// 1 lit - 0 off
const uint8_t seven_seg_pix_map[11][7] = { //--------- 0
                                          {1,  // A
                                           1,  // B
                                           1,  // C
                                           1,  // D
                                           1,  // E
                                           1,  // F
                                           0}, // G
                                           //--------- 1
                                          {0,  // A
                                           0,  // B
                                           0,  // C
                                           0,  // D
                                           1,  // E
                                           1,  // F
                                           0}, // G
                                           //--------- 2
                                          {1,  // A
                                           0,  // B
                                           1,  // C
                                           1,  // D
                                           0,  // E
                                           1,  // F
                                           1}, // G
                                           //--------- 3
                                          {1,  // A
                                           0,  // B
                                           0,  // C
                                           1,  // D
                                           1,  // E
                                           1,  // F
                                           1}, // G
                                           //--------- 4
                                          {0,  // A
                                           1,  // B
                                           0,  // C
                                           0,  // D
                                           1,  // E
                                           1,  // F
                                           1}, // G
                                           //--------- 5
                                          {1,  // A
                                           1,  // B
                                           0,  // C
                                           1,  // D
                                           1,  // E
                                           0,  // F
                                           1}, // G
                                           //--------- 6
                                          {1,  // A
                                           1,  // B
                                           1,  // C
                                           1,  // D
                                           1,  // E
                                           0,  // F
                                           1}, // G
                                           //--------- 7
                                          {1,  // A
                                           0,  // B
                                           0,  // C
                                           0,  // D
                                           1,  // E
                                           1,  // F
                                           0}, // G
                                           //--------- 8
                                          {1,  // A
                                           1,  // B
                                           1,  // C
                                           1,  // D
                                           1,  // E
                                           1,  // F
                                           1}, // G
                                           //--------- 9
                                          {1,  // A
                                           1,  // B
                                           0,  // C
                                           1,  // D
                                           1,  // E
                                           1,  // F
                                           1}, // G
                                           //--------- blank
                                          {0,  // A
                                           0,  // B
                                           0,  // C
                                           0,  // D
                                           0,  // E
                                           0,  // F
                                           0}, // G
                                         };
const uint8_t rpm_array[10] = {23,  // 1 (LED #) - Fully left in HUD - Red
                               22,  // 2
                               21,  // 3
                               20,  // 4
                               19,  // 5
                               18,  // 6
                               17,  // 7
                               16,  // 8
                               15,  // 9
                               14}; // 10 - Fully right in HUD - Blue
const uint16_t MAX_RPM = 3500;
const uint16_t MIN_RPM = 700;


struct STRUCT {
  int8_t status;
  uint32_t rpm;
  float mph;
} carTelem;


void setup()
{
  DEBUG_PORT.begin(115200);
  LED_DRIVER_PORT.begin(115200);

  myTransfer.begin(LED_DRIVER_PORT);

  setupLEDs();
}


void loop()
{
  if(myTransfer.available())
  {
    myTransfer.rxObj(carTelem, sizeof(carTelem));

    if (carTelem.status)
      printError();
    else
      updateLEDs();
  }
  else if(myTransfer.status < 0)
  {
    DEBUG_PORT.print("ERROR: ");
    DEBUG_PORT.println(myTransfer.status);
  }
}


void setupLEDs()
{
  initSevenSeg(0);
  initSevenSeg(1);
  initSevenSeg(3);
  initRpmDisp();
}


void initSevenSeg(uint8_t segNum)
{
  uint8_t value = 10; // init as blank
  
  for(uint8_t i = 0; i < 7; i++)
  {
    pinMode(speed_led_pin_array[segNum][i], OUTPUT);
    
    if(seven_seg_pix_map[value][i])
      digitalWrite(speed_led_pin_array[segNum][i], LOW);
    else
      digitalWrite(speed_led_pin_array[segNum][i], HIGH);
  }
}


void initRpmDisp()
{
  for(uint8_t i = 0; i < 10; i++)
  {
    pinMode(rpm_array[i], OUTPUT);
    digitalWrite(rpm_array[i], HIGH);
  }
}


void updateLEDs()
{
  updateSpeedDisp(carTelem.mph);
  updateRpmDisp(carTelem.rpm);
}


void updateSpeedDisp(float speed_mph)
{
  uint8_t hundredsPlace;
  uint8_t tensPlace;
  uint8_t onesPlace;
  uint8_t adjSpeed_mph = (uint8_t)(speed_mph + 0.5); // add 0.5 and type-cast in order to propperly round float

  hundredsPlace = adjSpeed_mph / 100;
  if(hundredsPlace == 0)
    hundredsPlace = 10; // this will cause a blank to be sent to the display
  
  tensPlace = adjSpeed_mph / 10;
  if(tensPlace == 0)
    tensPlace = 10; // this will cause a blank to be sent to the display
    
  onesPlace = adjSpeed_mph % 10;
  
  updateSevenSeg(0, onesPlace);
  updateSevenSeg(1, tensPlace);
  updateSevenSeg(2, hundredsPlace);
}


void updateSevenSeg(uint8_t segNum, uint8_t value)
{
  for(uint8_t i = 0; i < 7; i++)
  {
    if(seven_seg_pix_map[value][i])
      digitalWrite(speed_led_pin_array[segNum][i], LOW);
    else
      digitalWrite(speed_led_pin_array[segNum][i], HIGH);
  }
}


void updateRpmDisp(uint32_t rpm)
{
  uint32_t adjRPM = constrain(map(rpm, MIN_RPM, MAX_RPM, 0, 9), 0, 9);
  
  for(uint8_t i = 0; i < 10; i++)
  {
    if(adjRPM >= i)
      digitalWrite(rpm_array[i], LOW);
    else
      digitalWrite(rpm_array[i], HIGH);
  }
}


void printError()
{
  if (carTelem.status == ELM_SUCCESS)
    DEBUG_PORT.println(F("\tELM_SUCCESS"));
  else if (carTelem.status == ELM_NO_RESPONSE)
    DEBUG_PORT.println(F("\tERROR: ELM_NO_RESPONSE"));
  else if (carTelem.status == ELM_BUFFER_OVERFLOW)
    DEBUG_PORT.println(F("\tERROR: ELM_BUFFER_OVERFLOW"));
  else if (carTelem.status == ELM_UNABLE_TO_CONNECT)
    DEBUG_PORT.println(F("\tERROR: ELM_UNABLE_TO_CONNECT"));
  else if (carTelem.status == ELM_NO_DATA)
    DEBUG_PORT.println(F("\tERROR: ELM_NO_DATA"));
  else if (carTelem.status == ELM_STOPPED)
    DEBUG_PORT.println(F("\tERROR: ELM_STOPPED"));
  else if (carTelem.status == ELM_TIMEOUT)
    DEBUG_PORT.println(F("\tERROR: ELM_TIMEOUT"));
  else if (carTelem.status == ELM_TIMEOUT)
    DEBUG_PORT.println(F("\tERROR: ELM_GENERAL_ERROR"));
}

