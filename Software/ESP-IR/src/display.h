void displaySetup()
{  
  display->begin();
  display->fillScreen(BLACK);

  ledcSetup(ledChannel, ledFrequency, ledResolution);
  ledcAttachPin(DISPLAY_PIN_BACKLIGHT, ledChannel);
  ledcWrite(ledChannel, ledBrightness); 
}

void drawBufferToDisplay()
{
  display->startWrite();

  for(int i = 0; i < 60; i++)
  {
    for(int j = 0; j < 80; j++)
    {
      drawQuadxel(i * 4 + 1, j * 4 + 1, flirBuffer[i][j]);
    }
  }
  display->endWrite();

}

void drawDoubxel(uint8_t x, uint8_t y, uint8_t color)
{
  uint16_t pixelColor = flirToGreyScale(color);

  display->writePixel(x    , y    , pixelColor);
  display->writePixel(x + 1, y    , pixelColor);
  display->writePixel(x    , y + 1, pixelColor);
  display->writePixel(x + 1, y + 1, pixelColor);
}

void drawTripxel(uint8_t x, uint8_t y, uint8_t color)
{
  uint16_t pixelColor = flirToGreyScale(color);
  
  display->writePixel(x    , y    , pixelColor);
  display->writePixel(x + 1, y    , pixelColor);
  display->writePixel(x    , y + 1, pixelColor);
  display->writePixel(x + 1, y + 1, pixelColor);
  display->writePixel(x + 2, y    , pixelColor);
  display->writePixel(x    , y + 2, pixelColor);
  display->writePixel(x + 2, y + 1, pixelColor);
  display->writePixel(x + 1, y + 2, pixelColor);
  display->writePixel(x + 2, y + 2, pixelColor);
}

void drawQuadxel(uint8_t x, uint8_t y, uint8_t color)
{
  uint16_t pixelColor = flirToGreyScale(color);

  display->writePixel(x    , y    , pixelColor);
  display->writePixel(x + 1, y    , pixelColor);
  display->writePixel(x    , y + 1, pixelColor);
  display->writePixel(x + 1, y + 1, pixelColor);
  display->writePixel(x + 2, y    , pixelColor);
  display->writePixel(x    , y + 2, pixelColor);
  display->writePixel(x + 2, y + 1, pixelColor);
  display->writePixel(x + 1, y + 2, pixelColor);
  display->writePixel(x + 2, y + 2, pixelColor);
  display->writePixel(x + 3, y    , pixelColor);
  display->writePixel(x + 3, y + 1, pixelColor);
  display->writePixel(x + 3, y + 2, pixelColor);
  display->writePixel(x + 3, y + 3, pixelColor);
  display->writePixel(x    , y + 3, pixelColor);
  display->writePixel(x + 1, y + 3, pixelColor);
  display->writePixel(x + 2, y + 3, pixelColor);  
}

uint16_t flirToGreyScale(uint8_t flirColor)
{
  uint16_t greyscale = flirColor << 11 | flirColor << 6 | flirColor;

  return greyscale;
}