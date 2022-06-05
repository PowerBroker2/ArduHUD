#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>
#include "obd.h"




#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define SCL_PIN 39
#define SDA_PIN 38
#define RES_PIN 37
#define DC_PIN  36
#define CS_PIN  35




const uint16_t MAX_RPM = 3500;
const uint16_t MIN_RPM = 700;




Adafruit_SSD1331 display = Adafruit_SSD1331(CS_PIN, DC_PIN, SDA_PIN, SCL_PIN, RES_PIN);




const uint8_t speed_led_pin_array[3][7] = {//--------- one's place
                                           { 26,  // A
                                             27,  // B
                                             28,  // C
                                             29,  // D
                                             30,  // E
                                             31,  // F
                                             32}, // G
                                           //--------- ten's place
                                           { 8,   // A
                                             9,   // B
                                             10,  // C
                                             11,  // D
                                             12,  // E
                                             24,  // F
                                             25}, // G
                                           //--------- hundred's place
                                           { 13,  // A
                                             2,   // B
                                             3,   // C
                                             4,   // D
                                             5,   // E
                                             6,   // F
                                             7}   // G
};
// 1 lit - 0 off
const uint8_t seven_seg_pix_map[12][7] = {//--------- 0
                                          { 1,  // A
                                            1,  // B
                                            1,  // C
                                            1,  // D
                                            1,  // E
                                            1,  // F
                                            0}, // G
                                          //--------- 1
                                          { 0,  // A
                                            1,  // B
                                            1,  // C
                                            0,  // D
                                            0,  // E
                                            0,  // F
                                            0}, // G
                                          //--------- 2
                                          { 1,  // A
                                            1,  // B
                                            0,  // C
                                            1,  // D
                                            1,  // E
                                            0,  // F
                                            1}, // G
                                          //--------- 3
                                          { 1,  // A
                                            1,  // B
                                            1,  // C
                                            1,  // D
                                            0,  // E
                                            0,  // F
                                            1}, // G
                                          //--------- 4
                                          { 0,  // A
                                            1,  // B
                                            1,  // C
                                            0,  // D
                                            0,  // E
                                            1,  // F
                                            1}, // G
                                          //--------- 5
                                          { 1,  // A
                                            0,  // B
                                            1,  // C
                                            1,  // D
                                            0,  // E
                                            1,  // F
                                            1}, // G
                                          //--------- 6
                                          { 1,  // A
                                            0,  // B
                                            1,  // C
                                            1,  // D
                                            1,  // E
                                            1,  // F
                                            1}, // G
                                          //--------- 7
                                          { 1,  // A
                                            1,  // B
                                            1,  // C
                                            0,  // D
                                            0,  // E
                                            0,  // F
                                            0}, // G
                                          //--------- 8
                                          { 1,  // A
                                            1,  // B
                                            1,  // C
                                            1,  // D
                                            1,  // E
                                            1,  // F
                                            1}, // G
                                          //--------- 9
                                          { 1,  // A
                                            1,  // B
                                            1,  // C
                                            1,  // D
                                            0,  // E
                                            1,  // F
                                            1}, // G
                                          //--------- blank
                                          { 0,  // A
                                            0,  // B
                                            0,  // C
                                            0,  // D
                                            0,  // E
                                            0,  // F
                                            0}, // G
                                          //--------- negative
                                          { 0,  // A
                                            0,  // B
                                            0,  // C
                                            0,  // D
                                            0,  // E
                                            0,  // F
                                            1}  // G
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




void initSevenSeg(uint8_t segNum)
{
  uint8_t value = 10; // init as blank

  for (uint8_t i = 0; i < 7; i++)
  {
    pinMode(speed_led_pin_array[segNum][i], OUTPUT);

    if (seven_seg_pix_map[value][i])
      digitalWrite(speed_led_pin_array[segNum][i], LOW);
    else
      digitalWrite(speed_led_pin_array[segNum][i], HIGH);
  }
}




void initRpmDisp()
{
  for (uint8_t i = 0; i < 10; i++)
  {
    pinMode(rpm_array[i], OUTPUT);
    digitalWrite(rpm_array[i], HIGH);
  }
}




void updateSevenSeg(uint8_t segNum, uint8_t value)
{
  for (uint8_t i = 0; i < 7; i++)
  {
    if (seven_seg_pix_map[value][i])
      digitalWrite(speed_led_pin_array[segNum][i], LOW);
    else
      digitalWrite(speed_led_pin_array[segNum][i], HIGH);
  }
}




void updateSpeedDisp(float speed_mph)
{
  int8_t hundredsPlace;
  int8_t tensPlace;
  int8_t onesPlace;
  bool isNeg = false;

  if (speed_mph < 0)
  {
    isNeg = true;
    speed_mph *= -1;
  }
  
  int16_t adjSpeed_mph = (int16_t)(speed_mph + 0.5); // add 0.5 and type-cast in order to propperly round float

  onesPlace = adjSpeed_mph % 10;

  tensPlace = (adjSpeed_mph / 10) % 10;
  if (tensPlace == 0)
  {
    if (isNeg)
      tensPlace = 11; // this will cause a neg sign to be sent to the display
    else
      tensPlace = 10; // this will cause a blank to be sent to the display
  }
  else if (tensPlace == 10)
    tensPlace = 0;

  hundredsPlace = (adjSpeed_mph / 100) % 10;
  if (hundredsPlace == 0)
  {
    if (isNeg && (tensPlace != 11))
      hundredsPlace = 11; // this will cause a neg sign to be sent to the display
    else
      hundredsPlace = 10; // this will cause a blank to be sent to the display
  }

  updateSevenSeg(0, onesPlace);
  updateSevenSeg(1, tensPlace);
  updateSevenSeg(2, hundredsPlace);
}




void updateRpmDisp(uint32_t rpm)
{
  uint32_t adjRPM = constrain(map(rpm, MIN_RPM, MAX_RPM, 0, 9), 0, 9);

  for (uint8_t i = 0; i < 10; i++)
  {
    if (adjRPM >= i)
      digitalWrite(rpm_array[i], LOW);
    else
      digitalWrite(rpm_array[i], HIGH);
  }
}




void splashScreen()
{
  display.fillScreen(BLACK);
  display.setCursor(7, 5);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.print(F("ArduHUD"));
}




void sdFailed()
{
  display.fillScreen(BLACK);
  display.setCursor(20, 5);
  display.setTextColor(RED);
  display.setTextSize(2);
  display.print(F("SD iniatialization failed"));
}




void initOLED()
{
  display.begin();
  splashScreen();
}




void dispData(const float& mph, const float& rpm)
{
  display.fillScreen(BLACK);
  display.setCursor(7, 5);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print("MPH: ");
  display.print(int(mph + 0.5));
  display.setCursor(7, 25);
  display.print("RPM: ");
  display.print(int(rpm));
}




void dispErr()
{
  display.fillScreen(BLACK);
  display.setCursor(10, 5);
  display.setTextColor(RED);
  display.setTextSize(2);
  
  int err = myELM327.nb_rx_state;

  if (err == ELM_NO_RESPONSE)
    display.print("NO RESPONSE");
  else if (err == ELM_BUFFER_OVERFLOW)
    display.print("BUFFER OVERFLOW");
  else if (err == ELM_GARBAGE)
    display.print("GARBAGE");
  else if (err == ELM_UNABLE_TO_CONNECT)
    display.print("UNABLE TO CONNECT");
  else if (err == ELM_NO_DATA)
    display.print("NO DATA");
  else if (err == ELM_STOPPED)
    display.print("STOPPED");
  else if (err == ELM_TIMEOUT)
    display.print("TIMEOUT");
  else if (err == ELM_GETTING_MSG)
    display.print("GETTING MSG");
  else if (err == ELM_MSG_RXD)
    display.print("MSG RXD");
  else if (err == ELM_GENERAL_ERROR)
    display.print("GENERAL ERROR");
}
