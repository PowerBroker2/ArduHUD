#include "SdFat.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1331.h"
#include "SerialTransfer.h"
#include "ELMduino.h"




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

#define SW_PIN 34

#define DEBUG_PORT      Serial
#define LED_DRIVER_PORT Serial1




SdFatSdioEX sd;
SdFile myFile;
File root;
Adafruit_SSD1331 display = Adafruit_SSD1331(CS_PIN, DC_PIN, SDA_PIN, SCL_PIN, RES_PIN);
SerialTransfer myTransfer;




const uint8_t speed_led_pin_array[3][7] = { //--------- one's place
  { 1,   // A
    2,   // B
    3,   // C
    4,   // D
    5,   // E
    6,   // F
    7
  },  // G
  //--------- ten's place
  { 8,   // A
    9,   // B
    10,  // C
    11,  // D
    12,  // E
    24,  // F
    25
  }, // G
  //--------- hundred's place
  { 26,  // A
    27,  // B
    28,  // C
    29,  // D
    30,  // E
    31,  // F
    32
  }  // G
};
// 1 lit - 0 off
const uint8_t seven_seg_pix_map[11][7] = { //--------- 0
  { 1,  // A
    1,  // B
    1,  // C
    1,  // D
    1,  // E
    1,  // F
    0
  }, // G
  //--------- 1
  { 0,  // A
    0,  // B
    0,  // C
    0,  // D
    1,  // E
    1,  // F
    0
  }, // G
  //--------- 2
  { 1,  // A
    0,  // B
    1,  // C
    1,  // D
    0,  // E
    1,  // F
    1
  }, // G
  //--------- 3
  { 1,  // A
    0,  // B
    0,  // C
    1,  // D
    1,  // E
    1,  // F
    1
  }, // G
  //--------- 4
  { 0,  // A
    1,  // B
    0,  // C
    0,  // D
    1,  // E
    1,  // F
    1
  }, // G
  //--------- 5
  { 1,  // A
    1,  // B
    0,  // C
    1,  // D
    1,  // E
    0,  // F
    1
  }, // G
  //--------- 6
  { 1,  // A
    1,  // B
    1,  // C
    1,  // D
    1,  // E
    0,  // F
    1
  }, // G
  //--------- 7
  { 1,  // A
    0,  // B
    0,  // C
    0,  // D
    1,  // E
    1,  // F
    0
  }, // G
  //--------- 8
  { 1,  // A
    1,  // B
    1,  // C
    1,  // D
    1,  // E
    1,  // F
    1
  }, // G
  //--------- 9
  { 1,  // A
    1,  // B
    0,  // C
    1,  // D
    1,  // E
    1,  // F
    1
  }, // G
  //--------- blank
  { 0,  // A
    0,  // B
    0,  // C
    0,  // D
    0,  // E
    0,  // F
    0
  }, // G
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
                               14
                              }; // 10 - Fully right in HUD - Blue

const uint16_t MAX_RPM = 3500;
const uint16_t MIN_RPM = 700;
const uint8_t MSG_LEN = 16;
const uint8_t RESET_MESSAGE = 1;


char filename[20];

bool prevButtonState = true;
bool curButtonState  = true;
bool useSD = false;


struct STRUCT {
  int8_t status;
  uint32_t rpm;
  float mph;
  char msg[MSG_LEN];
} carTelem;




void setup()
{
  DEBUG_PORT.begin(115200);
  LED_DRIVER_PORT.begin(115200);

  setupLEDs();

  display.begin();
  myTransfer.begin(LED_DRIVER_PORT);

  useSD = setupSD();

  display.fillScreen(BLACK);
  display.setCursor(7, 5);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.print("ArduHUD");
}




void loop()
{
  if (myTransfer.available())
  {
    myTransfer.rxObj(carTelem, sizeof(carTelem));

    if (useSD)
      logSD();

    if (carTelem.status)
    {
      setupLEDs();
      printError();

      char buff[30];
      sprintf(buff, "ELM ERROR: %d", carTelem.status);

      display.fillScreen(BLACK);
      display.setCursor(20, 5);
      display.setTextColor(RED);
      display.setTextSize(2);
      display.print(buff);
    }
    else
    {
      updateLEDs();

      display.fillScreen(BLACK);
      display.setCursor(7, 5);
      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.print(int(carTelem.mph + 0.5));
      display.setCursor(7, 25);
      display.print(carTelem.rpm);
    }
  }
  else if (myTransfer.status < 0)
  {
    if (useSD)
      logSD();

    DEBUG_PORT.print("ERROR: ");
    DEBUG_PORT.println(myTransfer.status);

    display.fillScreen(BLACK);
    display.setCursor(20, 5);
    display.setTextColor(RED);
    display.setTextSize(2);
    display.print("TX ERROR");
  }
  
  if (useSD)
    handleCmds();
  else if (sd.begin())
    useSD = true;

  /*if (buttonPressed())
    sendReset();*/
}




void handleCmds()
{
  if (DEBUG_PORT.available())
  {
    char input[40] = { '\0' };
    
    readInput(input, sizeof(input));

    char target[] = {'l', 's'};
    if (stris(input, sizeof(input), target, sizeof(target)))
    {
      DEBUG_PORT.println(F("--------------------------------------------------"));
      
      root = sd.open("/");
      printDirectory(root, 0);
      
      DEBUG_PORT.println(F("--------------------------------------------------"));
      DEBUG_PORT.println();
    }
    else if (sd.exists(input))
    {
      DEBUG_PORT.println(F("--------------------------------------------------"));
      DEBUG_PORT.print(input); DEBUG_PORT.println(" found:");

      myFile.open(input, FILE_READ);
      
      int data;
      while ((data = myFile.read()) >= 0)
        DEBUG_PORT.write(data);
        
      myFile.close();

      DEBUG_PORT.println(F("--------------------------------------------------"));
      DEBUG_PORT.println();
    }
    else
      DEBUG_PORT.println('?');
  }
}




void readInput(char input[], uint8_t inputSize)
{
  uint8_t i = 0;
  unsigned long markTime = millis();
  
  while (DEBUG_PORT.available())
  {
    char c = DEBUG_PORT.read();
    
    if ((millis() - markTime) >= 100)
      break;
    else if (c == '\n')
      break;
    else if (i >= inputSize)
      break;

    input[i] = c;
    i++;
  }
}




void printDirectory(File dir, int numTabs)
{
  char fileName[20];
  
  while (true)
  {
    File entry =  dir.openNextFile();
    
    if (!entry)
      break;
      
    for (uint8_t i = 0; i < numTabs; i++)
      DEBUG_PORT.print('\t');

    entry.getName(fileName, sizeof(fileName));
    DEBUG_PORT.print(fileName);
    
    if (entry.isDirectory())
    {
      DEBUG_PORT.println("/");
      printDirectory(entry, numTabs + 1);
    }
    else
    {
      DEBUG_PORT.print("\t\t");
      DEBUG_PORT.println(entry.size(), DEC);
    }
    
    entry.close();
  }
}




bool stris(const char *input, const uint8_t inputSize, const char *target, const uint8_t targetSize)
{
  char newInput[inputSize] = { '\0' };
  char newTarget[targetSize] = { '\0' };

  memcpy(newInput, input, inputSize);
  memcpy(newTarget, target, targetSize);

  if ((strstr(newInput, newTarget) == newInput) && (newInput[targetSize] == '\0'))
    return true;
  else
    return false;
}




void setupLEDs()
{
  initSevenSeg(0);
  initSevenSeg(1);
  initSevenSeg(3);
  initRpmDisp();
}




bool setupSD()
{
  unsigned int driveCount = 1;

  if(!sd.begin())
  {
    display.fillScreen(BLACK);
    display.setCursor(20, 5);
    display.setTextColor(RED);
    display.setTextSize(2);
    display.print(F("SD iniatialization failed"));

    delay(5000);

    return false;
  }

  sprintf(filename, "drive_%d.txt", driveCount);

  while (sd.exists(filename))
  {
    driveCount++;
    sprintf(filename, "drive_%d.txt", driveCount);
  }

  myFile.open(filename, FILE_WRITE);
  myFile.println(F("Epoch,TX Status,ELM Status,ELM Msg,MPH,RPM"));
  myFile.close();

  return true;
}




void logSD()
{
  char buff[100];

  sprintf(buff, "%lu,%d,%d,%s,%f,%lu", millis(),
          myTransfer.status,
          carTelem.status,
          carTelem.msg,
          carTelem.mph,
          carTelem.rpm);

  myFile.open(filename, FILE_WRITE);
  myFile.println(buff);
  myFile.close();
}




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
  if (hundredsPlace == 0)
    hundredsPlace = 10; // this will cause a blank to be sent to the display

  tensPlace = adjSpeed_mph / 10;
  if (tensPlace == 0)
    tensPlace = 10; // this will cause a blank to be sent to the display
  else if (tensPlace == 10)
    tensPlace = 0;

  onesPlace = adjSpeed_mph % 10;

  updateSevenSeg(0, onesPlace);
  updateSevenSeg(1, tensPlace);
  updateSevenSeg(2, hundredsPlace);
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




// TODO: Debounce
bool buttonPressed()
{
  prevButtonState = curButtonState;
  curButtonState  = digitalRead(SW_PIN);

  if (!curButtonState && prevButtonState)
    return true;
  return false;
}




void sendReset()
{
  myTransfer.txObj(RESET_MESSAGE, sizeof(RESET_MESSAGE));
  myTransfer.sendData(sizeof(RESET_MESSAGE));
}



