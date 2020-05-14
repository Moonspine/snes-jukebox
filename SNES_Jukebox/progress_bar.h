#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include "jukebox_io.h"

class ProgressBar {
public:

  #define PROGRESS_BAR_HEIGHT 8
  
  ProgressBar(Adafruit_ST7735 &lcd_, byte x_, byte y_, byte width_, uint32_t maxProgress_) : lcd(lcd_), x(x_), y(y_), width(width_), maxProgress(maxProgress_) {
    currentProgress = 0;
    lastProgress = 0;
    
    beginLcdWrite();
    lcd.drawRect(x, y, width, PROGRESS_BAR_HEIGHT, ST7735_WHITE);
    lcd.fillRect(x + 1, y + 1, width - 2, PROGRESS_BAR_HEIGHT - 2, ST7735_BLACK);
    endLcdWrite();
  }
  
  void addProgress(uint32_t newProgress) {
    currentProgress += newProgress;
    if (currentProgress > maxProgress) {
      currentProgress = maxProgress;
    }
    
    int lastProgressWidth = (int)floor(((float)lastProgress / maxProgress) * (width - 2));
    int currentProgressWidth = (int)floor(((float)currentProgress / maxProgress) * (width - 2));
    if (currentProgressWidth > lastProgressWidth) {
      beginLcdWrite();
      lcd.fillRect(x + 1 + lastProgressWidth, y + 1, (currentProgressWidth - lastProgressWidth), PROGRESS_BAR_HEIGHT - 2, ST7735_GREEN);
      endLcdWrite();
    }
    
    lastProgress = currentProgress;
  }

  void setProgress(uint32_t newProgress) {
    currentProgress = min(newProgress, maxProgress);
    
    int lastProgressWidth = (int)floor(((float)lastProgress / maxProgress) * (width - 2));
    int currentProgressWidth = (int)floor(((float)currentProgress / maxProgress) * (width - 2));
    if (currentProgressWidth != lastProgressWidth) {
      beginLcdWrite();
      lcd.fillRect(x + 1, y + 1, width - 2, PROGRESS_BAR_HEIGHT - 2, ST7735_BLACK);
      lcd.fillRect(x + 1, y + 1, currentProgressWidth, PROGRESS_BAR_HEIGHT - 2, ST7735_GREEN);
      endLcdWrite();
    }
    
    lastProgress = currentProgress;
  }

private:
  Adafruit_ST7735 &lcd;
  byte x;
  byte y;
  byte width;

  uint32_t maxProgress;
  uint32_t currentProgress;
  uint32_t lastProgress;
};

#endif
