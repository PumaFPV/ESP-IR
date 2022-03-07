void flirSync()
{
  digitalWrite(FLIR_CS, HIGH);
  delay(186);
  digitalWrite(FLIR_CS, LOW);

}

void flirReadVOSPIPacket()
{
  digitalWrite(FLIR_CS, LOW);
  flirSPI->beginTransaction(SPISettings(FLIR_SPI_FREQUENCY, MSBFIRST, SPI_MODE3));

  flirPacketID = flirSPI->transfer16(0x00) & 0x0FFF; //remove the 4 leading bits as they are reserved
  flirPacketCRC = flirSPI->transfer16(0x00);

  for(int i = 0; i < FLIR_VOSPI_PAYLOAD_LENGTH; ++i)  //we divide the payload length (in bits) by 8 because we read 8 bytes at a time
  {
    flirVOSPIPacket[i] = flirSPI->transfer(0x00); //AGC disabled actually
  }

  flirSPI->endTransaction();
  digitalWrite(FLIR_CS, HIGH);

}

void flirLoop()
{
  flirReadVOSPIPacket();
  uint8_t discardPacket = (flirPacketID & 0x0F00) >> 8;

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
      uint16_t pixel = (flirVOSPIPacket[i+1] << 8) | flirVOSPIPacket[i]; //should be uint8_t pixel = (flirVOSPIPacket[i] << 8) | flirVOSPIPacket[i+1];
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