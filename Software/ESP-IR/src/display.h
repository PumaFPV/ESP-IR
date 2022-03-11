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

void drawBufferOptimized()
{
  display->startWrite();

  for(int i = 0; i < 60; i++)
  {
    for(int j = 0; j < 60; j++)
    {
      display->writeFillRect(i * 4 + 1, j * 4 + 1, 4, 4, flirBuffer[i][j + displayVerticalOffset]);
    }
  }

  display->endWrite();
}


void drawSuperSampleBuffer()
{
  display->startWrite();

  for(uint8_t x = 0; x < 60; ++x)
  {
    for(uint8_t yDisplay = 0; yDisplay < 60; ++yDisplay)
    {
      uint8_t y = yDisplay; //+ displayVerticalOffset;

      uint8_t xQuad = x * 4;
      uint8_t yQuad = y * 4;

      uint16_t colorTopLeft =     flirBuffer[x][y]/4 + flirBuffer[x-1][y]/4 + flirBuffer[x-1][y-1]/4 + flirBuffer[x][y-1]/4;
      uint16_t colorTop =         flirBuffer[x][y]/2 + flirBuffer[x][y-1]/2;
      uint16_t colorTopRight =    flirBuffer[x][y]/4 + flirBuffer[x][y-1]/4 + flirBuffer[x+1][y-1]/4 + flirBuffer[x+1][y]/4;
      uint16_t colorLeft =        flirBuffer[x][y]/2 + flirBuffer[x-1][y]/2;
      uint16_t colorRight =       flirBuffer[x][y]/2 + flirBuffer[x+1][y]/2;
      uint16_t colorBottomLeft =  flirBuffer[x][y]/4 + flirBuffer[x][y+1]/4 + flirBuffer[x-1][y+1]/4 + flirBuffer[x-1][y]/4;
      uint16_t colorBottom =      flirBuffer[x][y]/2 + flirBuffer[x][y+1]/2;
      uint16_t colorBottomRight = flirBuffer[x][y]/4 + flirBuffer[x+1][y]/4 + flirBuffer[x+1][y+1]/4 + flirBuffer[x][y+1]/4;
      
      display->writePixel(xQuad,     yQuad,                           colorTopLeft);
      display->writeLine( xQuad + 1, yQuad,     xQuad + 2, yQuad,     colorTop);
      display->writePixel(xQuad + 3, yQuad,                           colorTopRight);
      display->writeLine( xQuad,     yQuad + 1, xQuad,     yQuad + 2, colorLeft);
      display->writeLine( xQuad + 3, yQuad + 1, xQuad + 3, yQuad + 2, colorRight);
      display->writePixel(xQuad,     yQuad + 3,                       colorBottomLeft);
      display->writeLine( xQuad + 1, yQuad + 3, xQuad + 2, yQuad + 3, colorBottom);
      display->writePixel(xQuad + 3, yQuad + 3,                       colorBottomRight);

      drawBixel(xQuad + 1, yQuad + 1, flirBuffer[x][y]);

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
