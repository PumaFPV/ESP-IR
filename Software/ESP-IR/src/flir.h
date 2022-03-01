void flirSync()
{
  digitalWrite(FLIR_CS, HIGH);
  delay(186);
  digitalWrite(FLIR_CS, LOW);

}

void flirReadVOSPIPacket()
{
    for(int i = 0; i < FLIR_VOSPI_PAYLOAD_LENGTH; i++)
    {
        int data = flirSPI->transfer16(0x0000);

        int flirPacketID = data & 0b0000111111111111;
        int flirReservedID = data & 0b1111000000000000;
    }
}


/*

void lepton_sync(void)
{
  int i;
  int data = 0x0f;

  digitalWrite(8, HIGH);
  delay(185);
  while (data & 0x0f == 0x0f)
{
    digitalWrite(8, LOW);
    data =  flirSPI->transfer(0x00) << 8;
    data |= flirSPI->transfer(0x00);
    digitalWrite(8, HIGH);

    for (i = 0; i < ((164 - 2) / 2); i++)
    {

    digitalWrite(8, LOW);

      flirSPI->transfer(0x00);
      flirSPI->transfer(0x00);

    digitalWrite(8, HIGH);
    }
  }

}
*/