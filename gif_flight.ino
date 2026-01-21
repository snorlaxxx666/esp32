#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <AnimatedGIF.h>
#include "gif_data.h"

#define TFT_CS     15
#define TFT_DC     2
#define TFT_RST    4
#define X_OFFSET   60
#define Y_OFFSET   20

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
AnimatedGIF gif;

void GIFDraw(GIFDRAW *pDraw) {
  uint8_t *s;
  uint16_t *usPalette, usTemp[320];
  int x, y, iWidth;

  iWidth = pDraw->iWidth;
  if (iWidth > 320) iWidth = 320;

  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y;

  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2) { 
    for (x = 0; x < iWidth; x++) {
      if (s[x] == pDraw->ucTransparent) s[x] = pDraw->ucBackground;
    }
    pDraw->ucHasTransparency = 0;
  }

  if (pDraw->ucHasTransparency) { 
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    int iOffset = 0;
    pEnd = s + iWidth;
    x = 0;
    while (s < pEnd) {
      c = *s++;
      if (c != ucTransparent) {
        tft.drawPixel(pDraw->iX + x + X_OFFSET, y + Y_OFFSET, usPalette[c]);
      }
      x++;
    }
  } else {
    for (x = 0; x < iWidth; x++) usTemp[x] = usPalette[*s++];
    tft.drawRGBBitmap(pDraw->iX + X_OFFSET, y + Y_OFFSET, usTemp, iWidth, 1);
  }
}

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  gif.begin(LITTLE_ENDIAN_PIXELS);
}

void loop() {
  if (gif.open((uint8_t *)my_gif, sizeof(my_gif), GIFDraw)) {
    tft.fillScreen(ILI9341_BLACK);
    while (gif.playFrame(true, NULL)) {
    }
    gif.close();
  }
}