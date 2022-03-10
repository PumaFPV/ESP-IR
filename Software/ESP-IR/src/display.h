/* Display clock is at 8MHz, each frame is 16*240*240bits so 921600bits/frame, 
at 8MHz this is 8.6fps if it was full on the  display (which isnt even the case) */

void displaySetup();
void drawLiteBuffer();
void drawBuffer();
void drawSuperSampleBuffer();
void drawBixel(uint8_t x, uint8_t y, uint16_t color);
void drawQuadxel(uint8_t x, uint8_t y, uint16_t color);

void displaySetup()
{  
  display->begin(); //Cannot put higher than the predifined 8MHz
  display->fillScreen(BLACK);

  ledcSetup(ledChannel, ledFrequency, ledResolution);
  ledcAttachPin(DISPLAY_PIN_BACKLIGHT, ledChannel);
  ledcWrite(ledChannel, ledBrightness); 
}

void drawLiteBuffer()
{
  display->startWrite();
  for(uint8_t x = 0; x < 60; ++x)
  {
    for(uint8_t y = 0; y < 60; ++y)
    {
      display->writePixel(x + 90, y + 90, flirBuffer[x][y + displayVerticalOffset]);
    }
  }
  display->endWrite();
}

void drawBuffer()
{
  display->startWrite();

  for(int i = 0; i < 60; i++)
  {
    for(int j = 0; j < 60; j++)
    {
      drawQuadxel(i * 4 + 1, j * 4 + 1, flirBuffer[i][j + displayVerticalOffset]);
    }
  }

  display->endWrite();
}

void drawSuperSampleBuffer()
{
  display->startWrite();

  for(uint8_t x = 0; x < 60; ++x)
  {
    for(uint8_t y = 0; y < 60; ++y)
    {
      drawBixel(x * 4 + 1, y * 4 + 1, flirBuffer[x][y + displayVerticalOffset]);
    }
  }

  display->endWrite();
}

void drawBixel(uint8_t x, uint8_t y, uint16_t color)
{
  for(uint8_t x_offset = 0; x_offset < 2; x_offset++)
  {
    for(uint8_t y_offset = 0; y_offset < 2; y_offset++)
    {
      display->writePixel(x + x_offset, y + y_offset, color);
    }
  }
}

void drawQuadxel(uint8_t x, uint8_t y, uint16_t color)
{
  for(uint8_t x_offset = 0; x_offset < 4; x_offset++)
  {
    for(uint8_t y_offset = 0; y_offset < 4; y_offset++)
    {
      display->writePixel(x + x_offset, y + y_offset, color);
    }
  }
}
