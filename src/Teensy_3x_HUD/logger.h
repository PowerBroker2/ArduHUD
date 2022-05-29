#pragma once

#include <SPI.h>
#include "SdFat.h"
#include "SdTerminal.h"
#include "leds.h"




// Uncomment 'USE_EXTERNAL_SD' define to use an external SD card adapter or leave
// it commented to use the built in sd card.
//#define USE_EXTERNAL_SD 

#ifdef USE_EXTERNAL_SD
const uint8_t SD_CS_PIN = SS;
#define SPI_CLOCK SD_SCK_MHZ(10)
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#else // Use built in SD card

#ifdef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif // SDCARD_SS_PIN
#define SPI_CLOCK SD_SCK_MHZ(50)
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#endif // USE_EXTERNAL_SD




SdFs sd;
SdFile myFile;
Terminal myTerminal;




char filename[20];




bool setupSD()
{
  unsigned int driveCount = 1;

  if(!sd.begin(SD_CONFIG))
  {
    sdFailed();
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

  myTerminal.begin(sd);

  return true;
}




void logSD(const float& mph, const unsigned long& rpm)
{
  char buff[50];

  sprintf(buff,
          "%lu,%f,%lu",
          millis(),
          mph,
          rpm);

  myFile.open(filename, FILE_WRITE);
  myFile.println(buff);
  myFile.close();
}
