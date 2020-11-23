#include <M5StickC.h>

void startupScreen(String sw_version)
{

  M5.Lcd.fillScreen(TFT_BLACK); // Black screen fill
  M5.Lcd.setCursor(22, 20);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print(get_beacon_id());
}

void display_occupancy(int occupancy, int oColor)
{

  if (oColor == 0)
  {
    M5.Lcd.fillScreen(TFT_BLACK); // Black screen fill
  }
  else if (oColor == 1)
  {
    M5.Lcd.fillScreen(TFT_GREEN); // Black screen fill
  }
  else if (oColor == 2)
  {
    M5.Lcd.fillScreen(TFT_YELLOW); // Black screen fill
  }
  else if (oColor == 3)
  {
    M5.Lcd.fillScreen(TFT_RED); // Black screen fill
  }
}