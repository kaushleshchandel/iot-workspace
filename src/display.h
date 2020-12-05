#include <M5StickC.h>


int btn_home_press = 0; // Track Home button press 

struct iButton {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

iButton buttonhome = {BUTTON_A_PIN, 0, false};
iButton buttonside = {BUTTON_B_PIN, 0, false};

void IRAM_ATTR isrh() {
    buttonhome.numberKeyPresses += 1;
    buttonhome.pressed = true;
  //  buttonside.numberKeyPresses = 0;

  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setCursor(35, 150);
  M5.Lcd.print(buttonhome.numberKeyPresses);
 ;
}

void IRAM_ATTR isrs() {

    buttonside.numberKeyPresses += 1;
    buttonside.pressed = true;

  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(60, 60);
  M5.Lcd.print(buttonside.numberKeyPresses);
 
 
}


void startupScreen(String sw_version)
{

  M5.Lcd.fillScreen(TFT_BLACK); // Black screen fill
  M5.Lcd.setCursor(2, 20);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print(get_beacon_id().substring(6));

  M5.Lcd.setCursor(20, 80);
 
  M5.Lcd.print(sw_version);
}

void wifi_setup_screen(String tssid, String tpass )
{
  M5.Lcd.fillRect(0, 0, 80, 160, TFT_WHITE);

  M5.Lcd.qrcode("WIFI:S:" + tssid + ";T:WPA;P:" +  tpass + ";", 0, 0, 80, 3);
  Serial.print("WIFI:" + tssid + ";P:" +  tpass );

  M5.Lcd.setTextColor(TFT_BLUE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(8, 120);
  M5.Lcd.print(tssid);

  M5.Lcd.setCursor(15, 145);
  M5.Lcd.print(tpass);

  M5.Lcd.setTextColor(0x7bef);
  M5.Lcd.setCursor(25, 110);
  M5.Lcd.print("WiFi");

  M5.Lcd.setCursor(15, 135);
  M5.Lcd.print("Password");


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

  M5.Lcd.setCursor(25, 140);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print(occupancy);
}



void initButtons()
{
  //Home Button assignment
  pinMode(buttonhome.PIN, INPUT);
  attachInterrupt(buttonhome.PIN, isrh, FALLING);
  //Side Button assignment
  pinMode(buttonside.PIN, INPUT);
  attachInterrupt(buttonside.PIN, isrs, FALLING);

}