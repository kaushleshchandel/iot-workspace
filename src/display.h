#include <M5StickC.h>


void startupScreen()
{

  M5.Lcd.fillScreen(TFT_BLACK); // Black screen fill
 // int x = M5.Lcd.width() / 2  - logoWidth / 2;
 // int y = M5.Lcd.height() / 2 - logoHeight / 2;
 // M5.Lcd.drawXBitmap(x, y, logo, logoWidth, logoHeight, TFT_WHITE);

  M5.Lcd.setCursor(22, 120);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print(SW_VERSION);

}