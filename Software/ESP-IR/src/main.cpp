/*
 * ESP32 various dev board     : CS:  5, DC: 27, RST: 33, BL: 22
 * Flir is on HSPI: CLK: 11, MISO: 9, CS: 8, SCL: 3, SDA: 2
 */

#define DISPLAY_SCL 18
#define DISPLAY_SDA 23
#define DISPLAY_PIN_RESET 33
#define DISPLAY_PIN_DC 27
#define DISPLAY_PIN_CS 5
#define DISPLAY_PIN_BACKLIGHT 22
#define DISPLAY_ROTATION 0

#define FLIR_MOSI 13
#define FLIR_MISO 12  
#define FLIR_SCLK 14  
#define FLIR_CS 15  //Active LOW
#define FLIR_SPI_FREQUENCY 8000000 //4MHz,can go up to 20MHz
#define FLIR_SDA 17
#define FLIR_SCL 16
#define FLIR_I2C_FREQUENCY 400000
#define FLIR_I2C_ADDRESS 0x2A

#define FLIR_REGISTER_COMMAND 0x0004
#define FLIR_REGISTER_STATUS 0x0002 //if 0 than FLIR ready for new command, bit 0 of the register, response code written in 0b1111111100000000
#define FLIR_REGISTER_DATA_LENGTH 0x0006  //number of 16bit being transfered
#define FLIR_REGISTER_DATA 0x0008 //starts at 0x0008, ends at 0x0026
#define FLIR_BOOT_STATUS_BIT 0b10 //location of the boot status bit inside the FLIR_STATUS_REGISTER, is 1 when camera has booted
#define FLIR_BOOT_MODE_BIT 0b100 //location of the boot mode bit inside the FLIR_STATUS_REGISTER, is 1 when succesful boot 
#define FLIR_COMMAND_GET 0b00
#define FLIR_COMMAND_SET 0b01
#define FLIR_COMMAND_RUN 0b10
#define FLIR_MODULE_ID_AGC 0b100000000  //Automatic Gain Control
#define FLIR_MODULE_ID_SYS 0b1000000000 //System information
#define FLIR_MODULE_ID_VID 0b110000000  //Video Processing Control

//VOSPI packet is 2 bytes of ID, 2 bytes of CRC and 160 bytes of payload
#define FLIR_VOSPI_DISCARD_PACKET 0x0F00 //if the 2 ID bytes are 0F00, then discard this frame
#define FLIR_VOSPI_PAYLOAD_LENGTH 160 //VOSPI payload is 160 bytes long, 2 bytes/pixel, w/ AGC disabled: 0b00xxxxxxxxxxxxxx, w/ AGC enabled: 0b00000000xxxxxxxx
#define FLIR_VOSPI_PACKET_LENGTH 164 //2 bytes ID, 2 bytes CRC, 160 bytes payload 
#define FLIR_VOSPI_PACKETS_PER_FRAME_RAW14_WO_TLM 60 //Raw14, telemtry disabled, 1312 bits long
#define FLIR_VOSPI_PACKETS_PER_FRAME_RAW14_W_TLM 63
//disable telemetry to have 60 packets of FPA rows
#define FLIR_REFRESH_RATE 9 //9Hz of real refresh rate
#define VOSPI_REFRES_RATE 27 //27Hz of VOSPI frame refresh rate, so each frame is sent 3 times


#include "esp32/Arduino.h"
#include "Wire.h"
#include "SPI.h"
#include "Arduino_GFX_Library.h"

const uint8_t  ledChannel = 0;
const uint16_t ledFrequency = 5000;
const uint8_t  ledResolution = 3; //8 steps
const uint8_t  ledBrightness = 1;

Arduino_DataBus *bus = create_default_Arduino_DataBus();

Arduino_GFX     *display = new Arduino_GC9A01(bus, DISPLAY_PIN_RESET, DISPLAY_ROTATION, true /*IPS needs to be true to have correct colors on GC9A01*/);

SPIClass        *flirSPI = NULL;

void drawBufferToDisplay();
void drawDoubxel(uint8_t x, uint8_t y, uint8_t color);
void drawTripxel(uint8_t x, uint8_t y, uint8_t color);
void drawQuadxel(uint8_t x, uint8_t y, uint8_t color);

uint16_t flirToGreyScale(uint8_t flirColor);

void flirSync();
void flirReadVOSPIPacket();
uint8_t flirVOSPIPacket[160];
uint16_t flirPacketID;
uint16_t flirPacketCRC;
bool lastPacketWasDiscard = 0;
uint8_t flirFrameLine = 0;
uint8_t flirDiscardCounter = 0;
#include "buffer.h"
#include "flir.h"
#include "display.h"

bool runonce = 0;

void setup() 
{
  Serial.begin(115200);
  Serial.println("ESP-IR started");

  displaySetup();

  Wire.begin(FLIR_SDA, FLIR_SCL, FLIR_I2C_FREQUENCY); 
  
  flirSPI = new SPIClass(HSPI);
  flirSPI->begin(FLIR_SCLK, FLIR_MISO, FLIR_MOSI, FLIR_CS); 
  pinMode(FLIR_CS, OUTPUT);
  digitalWrite(FLIR_CS, HIGH);

}

void loop() 
{

  flirReadVOSPIPacket();
  uint8_t discardPacket = (flirPacketID & 0b0000111100000000) >> 8;

  if(discardPacket == 0x0F) //if discard frame
  {
    //discard
    flirFrameLine = 0;
    flirDiscardCounter++;
  }
  else
  {
    //Not discard
    flirDiscardCounter = 0;
    for(int i = 0; i < FLIR_VOSPI_PAYLOAD_LENGTH; i = i+2)
    {
      uint8_t pixel = (flirVOSPIPacket[i] << 8) | flirVOSPIPacket[i + 1]; 
      flirBuffer[flirFrameLine][i/2] = pixel;
    }
    flirFrameLine++;
  } 
  
  if(flirFrameLine > 59)
  {
    drawBufferToDisplay();
    flirFrameLine = 0;
  } 

  if(flirDiscardCounter > 60)
  {
    //flirSync();
    flirDiscardCounter = 0;
  }
}


