void agc_enable();

void flirReadVOSPIPacket();
void flirSync();

void flirReadVOSPIPacket()
{
  digitalWrite(FLIR_CS, LOW);
  flirSPI->beginTransaction(SPISettings(FLIR_SPI_FREQUENCY, MSBFIRST, SPI_MODE3));

  flirPacketID = flirSPI->transfer16(0x00) & 0x0FFF; //remove the 4 leading bits as they are reserved
  flirPacketCRC = flirSPI->transfer16(0x00);

  for(int i = 0; i < FLIR_VOSPI_PAYLOAD_LENGTH; i++)  //we divide the payload length (in bits) by 8 because we read 8 bytes at a time
  {
    flirVOSPIPacket[i] = flirSPI->transfer(0x00); //AGC disabled actually
  }

  flirSPI->endTransaction();
  digitalWrite(FLIR_CS, HIGH);
}

void flirSetup()
{
  Wire.begin(FLIR_SDA, FLIR_SCL, FLIR_I2C_FREQUENCY); 
  
  flirSPI = new SPIClass(HSPI);
  flirSPI->begin(FLIR_SCLK, FLIR_MISO, FLIR_MOSI, FLIR_CS); 
  pinMode(FLIR_CS, OUTPUT);
  digitalWrite(FLIR_CS, HIGH);

}

void flirSync()
{
  digitalWrite(FLIR_CS, HIGH);
  delay(186);
  digitalWrite(FLIR_CS, LOW);
}