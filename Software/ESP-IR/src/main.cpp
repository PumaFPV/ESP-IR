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
 
uint16_t tempToRainbow(uint16_t temp)
{
  //For flir rainbow -> blue 0-63 -> red 0-63 -> blue 63-0 -> green 0-255
  temp = temp >> 8;
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;

  if(temp < 64)
  {
    red = 0;
    green = 0;
    blue = temp / 2;
  }
  if((63 < temp) & (temp < 128))
  {
    red = (temp - 64) / 2;
    green = 0;
    blue = 31;
  }
  if((127 < temp) & (temp < 192))
  {
    red = 31;
    green = 0;
    blue = 31 - (temp - 128) / 2;
  }
  if((191 < temp) & (temp < 256))
  {
    red = 31;
    green = (temp - 192);
    blue = 0;
  }

  uint16_t displayPixel = (red << 11 )| (green << 5) | blue;

  return displayPixel;
}

void loop() 
{

/*   
  //YUV 2nd try
  uint8_t Y = 0b000;
  uint8_t U = 0b1000;
  uint8_t V = 0b00;

  for(V = 0; V < 0b111111; ++V)
  {
    uint8_t red = constrain(Y + V + ((V*103)>>8), 0, 0b11111);
    uint8_t green = constrain(Y - ((U * 88) >> 8) - ((V * 183 ) >> 8), 0, 0b111111);
    uint8_t blue = constrain(Y + U + ((U* 198) >> 8), 0, 0b11111);

    uint16_t displayPixel = (red << 11 )| (green << 5) | blue;
    display->fillScreen(displayPixel);
    display->setCursor(120,120);
    display->setTextSize(3);
    display->print(V);
    delay(250);
  }   
*/

/*   
  //YUV 1st try
  uint8_t Y = 0b0010;
  uint8_t U = 0b0000;
  uint8_t V = 0b0000;

  uint8_t red = Y + 1.402 * (V - 128);
  uint8_t green = Y - 0.34414 * (U - 128);
  uint8_t blue = Y + 1.772 * (U -128);

  uint16_t displayPixel = (red << 11 )| (green << 5) | blue;
  display->fillScreen(displayPixel);
 */


  //Working sample
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

      //uint16_t displayPixel = flirPixel & 0b1111100111111110;
      flirBuffer[flirFrameLine][i/2] = tempToRainbow(flirPixel);//displayPixel;
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


