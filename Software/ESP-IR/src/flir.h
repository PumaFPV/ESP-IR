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

  for(int i = 0; i < FLIR_VOSPI_PAYLOAD_LENGTH; i++)  //we divide the payload length (in bits) by 8 because we read 8 bytes at a time
  {
    flirVOSPIPacket[i] = flirSPI->transfer(0x00); //AGC disabled actually
  }

  flirSPI->endTransaction();
  digitalWrite(FLIR_CS, HIGH);

}
