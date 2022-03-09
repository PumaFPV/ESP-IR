/*
 * ESP32 various dev board     : CS:  5, DC: 27, RST: 33, BL: 22
 * Flir is on HSPI: CLK: 11, MISO: 9, CS: 8, SCL: 3, SDA: 2
 */

#include "esp32/Arduino.h"
#include "Wire.h"
#include "SPI.h"
#include "Arduino_GFX_Library.h"

#include "variables.h"

Arduino_DataBus *bus = create_default_Arduino_DataBus();
Arduino_GFX     *display = new Arduino_GC9A01(bus, DISPLAY_PIN_RESET, DISPLAY_ROTATION, true /*IPS needs to be true to have correct colors on GC9A01*/);
SPIClass        *flirSPI = NULL;


#include "buffer.h"
#include "display.h"
#include "flir.h"

void setup() 
{
  Serial.begin(115200);
  Serial.println("ESP-IR started");

  displaySetup();

  flirSetup();
 
}

void loop() 
{
/*   //find 16 steps of color
  display->fillScreen(0x0000);
  delay(500);
  display->fillScreen(0x701F);
  delay(500);
  display->fillScreen(0xA01F);
  delay(500);
  display->fillScreen(0xF920);
  delay(500);
  display->fillScreen(0xFEA0);
  delay(500);
  display->fillScreen(0xFFFF);
  delay(500);
 */


  flirReadVOSPIPacket();  //Read VOSPI packet and put each pixel in array

  if((flirPacketID & 0x0F00) == FLIR_VOSPI_DISCARD_PACKET) //if packet is discard, then discard
  {
    flirFrameLine = 0;  //restart frame line counter
    flirDiscardCounter++;
  }
  else
  {
    //Not discard
    flirDiscardCounter = 0; //restart discarde counter
    for(int i = 0; i < FLIR_VOSPI_PAYLOAD_LENGTH; i = i+2)
    {
      uint16_t flirPixel = (flirVOSPIPacket[i+1] << 8) | flirVOSPIPacket[i];  //display pixel is 16bits, 5R 6G 6B

      /*
       *flir color range goes from black, purple, pink, red, orange, yellow, white
       *
       * Black  0x0000
       * Purple 0x701F
       * Pink   0xA01F
       * Red    0xF920
       * Orange 0xFBC0
       * Yellow 0xFEA0
       * White  0xFFFF
       */

      uint8_t red;
      uint8_t green;
      uint8_t blue;

      //pixel = pixel & 0b1111100111111110; 
      uint16_t displayPixel = (red << 11 )| (green << 5) | blue;
      displayPixel = flirPixel & 0b1111100111111110;
      //displayPixel  = 
      flirBuffer[flirFrameLine][i/2] = displayPixel;
    }
    flirFrameLine++;
  } 
  
  if(flirFrameLine > FLIR_VOSPI_PACKETS_PER_FRAME_RAW14_WO_TLM - 1)  //if we received a complete frame, then display buffer to display
  {
    drawBufferToDisplay();
    uint16_t flirPixel = (flirVOSPIPacket[10+1] << 8) | flirVOSPIPacket[10];  //display pixel is 16bits, 5R 6G 6B

    Serial.println(flirPixel, BIN);
    flirFrameLine = 0;
  } 

  if(flirDiscardCounter > 60)
  {
    //We shouldnt reach this state
    flirSync();
    flirDiscardCounter = 0;
  } 
}


