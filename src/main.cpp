// activate deactivate serial output for debugging
#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)      
#define debugln(x)
#endif

#include <Arduino.h>
#include <Preferences.h>
#include <U8g2lib.h>
#include "BTS7960.h"
#include "display.h"
#include "pin_def.h"
#include "config_values.h"

// new button routine
bool btn_up_pressed = false;
bool btn_enter_pressed = false;
bool btn_left_pressed = false;
bool btn_down_pressed = false;
bool btn_right_pressed = false;
unsigned long pressStartTime;
unsigned long debounceTime = 100;
unsigned long longPressTime = 500;
bool btn_down_short = false;
bool btn_down_long = false;
bool btn_up_short = false;
bool btn_up_long = false;
bool btn_enter_short = false;
bool btn_enter_long = false;
bool btn_left_short = false;
bool btn_left_long = false;
bool btn_right_short = false;
bool btn_right_long = false;


// BTS7960
BTS7960 motorController(EN_L, EN_R, L_PWM, R_PWM, L_IS, R_IS);

//// settings menu array
// Define the struct for menu items
struct Settings {
  const char* name;
  int * value;
};
// Define the menu array
Settings menu[] = {
  {"stroke Speed", &strokeSpeed},
  {"return Speed", &returnSpeed},
  {"stroke Limit", &strokeCurrentLimit},
  {"return Limit", &returnCurrentLimit},
  {"stroke pause", &pauseBetweenStrokes},
  {"start delay", &startDelay},
  {"stroke IS delay", &startStrokingCurrentMeasuringDelay},
  {"return IS delay", &startReturningCurrentMeasuringDelay}
};
// Total number of menu items
const int menuSize = sizeof(menu) / sizeof(menu[0]);

void strokeRoutine();
void stroke();
void returnMotor();
void loadSettings();
void saveSettings();

// Preferences // save settings
Preferences preferences;


// Display Type
  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void newButtonRoutine(){
  //// button down
  // button down short press
  if (digitalRead(btn_down) == LOW && !btn_down_pressed) {
    btn_down_pressed = true;
    pressStartTime = millis();
  }
  // button down release
  else if (digitalRead(btn_down) == HIGH && btn_down_pressed){
    btn_down_pressed = false;
    btn_down_short = false;
    btn_down_long = false;
  }
  // button down long press check
  if (btn_down_pressed && millis() - pressStartTime >= longPressTime) {
    btn_down_long = true;
    pressStartTime = millis(); // Reset the start time
  }
  else if (btn_down_pressed && millis() - pressStartTime > debounceTime) {
    btn_down_short = true;
    delay(100);
  }

  //// button up
  // button up short press
  if (digitalRead(btn_up) == LOW && !btn_up_pressed) {
    btn_up_pressed = true;
    pressStartTime = millis();
  }
  // button up release
  else if (digitalRead(btn_up) == HIGH && btn_up_pressed){
    btn_up_pressed = false;
    btn_up_short = false;
    btn_up_long = false;
  }
  // button up long press check
  if (btn_up_pressed && millis() - pressStartTime >= longPressTime) {
    btn_up_long = true;
    pressStartTime = millis(); // Reset the start time
  }
  else if (btn_up_pressed && millis() - pressStartTime > debounceTime) {
      btn_up_short = true;
      delay(100);
  }

  //// button enter
  // button enter short press
  if (digitalRead(btn_enter) == LOW && !btn_enter_pressed) {
    btn_enter_pressed = true;
    pressStartTime = millis();
  }
  // button enter release
  else if (digitalRead(btn_enter) == HIGH && btn_enter_pressed){
    btn_enter_pressed = false;
    btn_enter_short = false;
    btn_enter_long = false;
  }
  // button enter long press check
  if (btn_enter_pressed && millis() - pressStartTime >= longPressTime) {
    btn_enter_long = true;
    pressStartTime = millis(); // Reset the start time
  }
  else if (btn_enter_pressed && millis() - pressStartTime > debounceTime) {
      btn_enter_short = true;
      delay(200);
  }

  //// button left
  // button left short press
  if (digitalRead(btn_left) == LOW && !btn_left_pressed) {
    btn_left_pressed = true;
    pressStartTime = millis();
  }
  // button enter release
  else if (digitalRead(btn_left) == HIGH && btn_left_pressed){
    btn_left_pressed = false;
    btn_left_short = false;
    btn_left_long = false;
  }
  // button left long press check
  if (btn_left_pressed && millis() - pressStartTime >= longPressTime) {
    btn_left_long = true;
    pressStartTime = millis(); // Reset the start time
  }
  else if (btn_left_pressed && millis() - pressStartTime > debounceTime) {
    btn_left_short = true;
  }

  //// button right
  // button right short press
  if (digitalRead(btn_right) == LOW && !btn_right_pressed) {
    btn_right_pressed = true;
    pressStartTime = millis();
  }
  // button enter release
  else if (digitalRead(btn_right) == HIGH && btn_right_pressed){
    btn_right_pressed = false;
    btn_right_short = false;
    btn_right_long = false;
  }
  // button right long press check
  if (btn_right_pressed && millis() - pressStartTime >= longPressTime) {
    btn_right_long = true;
    pressStartTime = millis(); // Reset the start time
  }
  else if (btn_right_pressed && millis() - pressStartTime > debounceTime) {
    btn_right_short = true;
  }

}

void newButtonNavigation() {
    switch (menuLevel) {
    case 1:
        if (btn_down_short) {
          if (MainMenu_Selected < MainMenuNumItems - 1) {
            MainMenu_Selected++;
            }
          } 
        if (btn_up_short) {
          if (MainMenu_Selected > 0) {
            MainMenu_Selected--;
            }
          }
        if (btn_enter_short){
          menuLevel = MainMenu_Selected + 10;
        }
      break;

    case 10:
        if (btn_right_short) {
          if ((amountOfStrokes < 255) && (StartMenu_Selected == 1)) {
          amountOfStrokes++;
          }
        }
        if (btn_right_long) {
          if ((amountOfStrokes < 245) && (StartMenu_Selected == 1)) {
          amountOfStrokes = amountOfStrokes + 10;
          }
        }
        if (btn_left_short) {
          if ((amountOfStrokes > 1) && (StartMenu_Selected == 1)) {
          amountOfStrokes--;
          }
        if (btn_left_long){
          if ((amountOfStrokes > 10) && (StartMenu_Selected == 1)) {
          amountOfStrokes = amountOfStrokes - 10;
          }
        }
        }
        if (btn_up_short) {
          if (StartMenu_Selected > 1) {
            StartMenu_Selected--;
            }
          }
        if (btn_down_short) {
          if (StartMenu_Selected < 3) {
            StartMenu_Selected++;
            }
          }
        if (btn_enter_short) {
          switch (StartMenu_Selected) {
            case 1:
            amountOfStrokes = 1;
            break;
            
            case 2:
            menuLevel = 1;
            break;

            case 3:
            // start stroking
            menuLevel = 20;
            startFirstSpankDelay = true;
            returning = false;
            previousMillis = millis();
            strokeRoutine();
            break;
          }
        }
        break; 

    case 20:
      if (btn_enter_short) {
        menuLevel = 1;
        running = false;
      }

    // Settings
    case 11: 
      if (btn_down_short) {
        if (currentMenuIndex < (menuSize - 1)) {
        currentMenuIndex++;
        }
      }
      if (btn_up_short) {
        if (currentMenuIndex > 0) {
        currentMenuIndex--;
        }
      } 
      if (btn_enter_short) {
        debugln("case 11 menu level 1");
        saveSettings();
        menuLevel = 1;
      }
      if (btn_left_short) {
        *menu[currentMenuIndex].value = *menu[currentMenuIndex].value - 1;
      }

      if (btn_left_long) {
        *menu[currentMenuIndex].value = *menu[currentMenuIndex].value - 10;
      }

      if (btn_right_short) {
        *menu[currentMenuIndex].value = *menu[currentMenuIndex].value + 1;
      }

      if (btn_right_long) {
        *menu[currentMenuIndex].value = *menu[currentMenuIndex].value + 10;
      }
  }
}

void returnMotor() {
  debugln("returnMotor function running");
  motorController.Enable();
  motorController.TurnRight(returnSpeed);
  delay(startReturningCurrentMeasuringDelay);
  if (motorController.CurrentSenseLeft() > returnCurrentLimit){
    debugln("return current limit triggered");
    motorController.Stop();
    delay(200);
    returning = false;
  }
}

void stroke() {
  debugln("stroke function running");
  motorController.Enable();
  motorController.TurnLeft(strokeSpeed);
  delay(startStrokingCurrentMeasuringDelay);
  if (motorController.CurrentSenseLeft() > strokeCurrentLimit) {
    debugln("stroke current limit triggered");
    motorController.Stop();
    lastSpankMillis = millis();
    amountOfStrokes--;
    delay(1000);
    stroking = false;
    returning = true;
    }
  }

void strokeRoutine() {
  // Display Countdown 
  if ((startFirstSpankDelay == true) &&(millis() - previousMillis < startDelay * 1000)) {
    u8g2.clearBuffer();
    u8g2.setFont(countdownFont);
    int remainingSeconds = (previousMillis + startDelay * 1000 - millis()) / 1000;
    if (remainingSeconds >= 0) {
    dispVar(50,40,remainingSeconds);
    }
    u8g2.sendBuffer();
  }
  if ((startFirstSpankDelay == true) && (millis() - previousMillis >= startDelay * 1000 )) {
    startFirstSpankDelay = false;
  }

  if ((amountOfStrokes > 0) && (returning == false) && (startFirstSpankDelay == false) && (millis() - lastSpankMillis >= pauseBetweenStrokes * 1000)) {
    stroking = true;
    stroke();
  }

  if (returning == true) {
    returnMotor();
  }

  if (amountOfStrokes == 0) {
    motorController.Stop();
    running = false;
    menuLevel = 1;
  }

  debugln(motorController.CurrentSenseLeft());
  delay(100);
}

void displayMenu() {
  running = false;
  MainMenu_Previous = (MainMenu_Selected - 1);
  MainMenu_Next = (MainMenu_Selected + 1);

  u8g2.clearBuffer();
  u8g2.setFont(normalFont);
  u8g2.drawHLine(0,12,128);
  u8g2.drawStr(47,8, "Spanky");
  u8g2.drawFrame(10,31,108,19);
  if (MainMenu_Selected > 1) {
    u8g2.drawStr(38,24,MainMenuItems[MainMenu_Previous]);
  }
  u8g2.drawStr(38,44,MainMenuItems[MainMenu_Selected]);
  u8g2.drawStr(38,64,MainMenuItems[MainMenu_Next]);
  u8g2.sendBuffer();
}

void displayStart() {
  u8g2.clearBuffer();
  u8g2.setFont(normalFont);
  dispVar(55,14,amountOfStrokes);
  
  u8g2.drawStr(40,38,"<< back");
  u8g2.drawStr(40,58,"start >>");

  switch (StartMenu_Selected) {
    case 1: 
      u8g2.drawFrame(30,01,68,19);
      break;
    case 2:
      u8g2.drawFrame(30,25,68,19);
      break;
    case 3:
      u8g2.drawFrame(30,45,68,19);
      break;
    default:
      u8g2.drawFrame(30,01,68,19);
      break;
  }
  u8g2.sendBuffer();
}

void displayRunning(){
  if (startFirstSpankDelay == false) {
  u8g2.clearBuffer();
  u8g2.setFont(normalFont);
  u8g2.drawStr(28,10,"Strokes Left");
  u8g2.setFont(countdownFont);
  dispVar(50,45,amountOfStrokes);
  u8g2.sendBuffer();
  }
  running = true;
}

void displayMenuSettings() {
  u8g2.clearBuffer();
  
  // frame
  u8g2.drawFrame(0,19,126,19);
  // left column
  u8g2.setFont(settingsFont);
  if (currentMenuIndex > 1) {
    dispVar(2,8,menu[currentMenuIndex-2].name);
    }
  if (currentMenuIndex > 0){
  dispVar(2,18,menu[currentMenuIndex-1].name);
  }
  u8g2.setFont(selectFont);
  dispVar(2,33,menu[currentMenuIndex].name);
  u8g2.setFont(settingsFont);
  if (currentMenuIndex < (menuSize - 1)) {
  dispVar(2,48,menu[currentMenuIndex+1].name);
  }
  if (currentMenuIndex < (menuSize - 2)){
  dispVar(2,58,menu[currentMenuIndex+2].name);
  }
  
  // right column
  if (currentMenuIndex > 1) {
  dispVar(100,8,*menu[currentMenuIndex-2].value);
  }
  if (currentMenuIndex > 0){
  dispVar(100,18,*menu[currentMenuIndex-1].value);
  }
  u8g2.setFont(selectFont);
  dispVar(100,33,*menu[currentMenuIndex].value);
  u8g2.setFont(settingsFont);
  if (currentMenuIndex < (menuSize - 1)) {
  dispVar(100,48,*menu[currentMenuIndex+1].value);
  }
  if (currentMenuIndex < (menuSize - 2)){
  dispVar(100,58,*menu[currentMenuIndex+2].value);
  }

  u8g2.sendBuffer();
}

//loadSettings
void loadSettings(){
    startDelay = preferences.getInt("s_StartDelay", 5);
    pauseBetweenStrokes = preferences.getInt("s_PauseBeSt", 20);
    strokeSpeed = preferences.getInt("s_StrSpeed", 200);
    returnSpeed = preferences.getInt("s_RetSpeed", 50);
    strokeCurrentLimit = preferences.getInt("s_StrCurLim", 50);
    returnCurrentLimit = preferences.getInt("s_RetCurLim", 50);
    startStrokingCurrentMeasuringDelay = preferences.getInt("s_StaStrCurMea", 300);
    startReturningCurrentMeasuringDelay = preferences.getInt("s_StaRetCurMea", 300);
} 

//saveSettings
void saveSettings(){
    preferences.putInt("s_StartDelay", startDelay);
    preferences.putInt("s_PauseBeSt", pauseBetweenStrokes);
    preferences.putInt("s_StrSpeed", strokeSpeed);
    preferences.putInt("s_RetSpeed", returnSpeed);
    preferences.putInt("s_StrCurLim", strokeCurrentLimit);
    preferences.putInt("s_RetCurLim", returnCurrentLimit);
    preferences.putInt("s_StaStrCurMea", startStrokingCurrentMeasuringDelay);
    preferences.putInt("s_StaRetCurMea", startReturningCurrentMeasuringDelay);
    preferences.end();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //buttons
  pinMode(btn_enter, INPUT);
  pinMode(btn_up, INPUT);
  pinMode(btn_down, INPUT);
  pinMode(btn_left, INPUT);
  pinMode(btn_right, INPUT);

  preferences.begin("savedSettings", false);
  loadSettings();
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(normalFont);
  u8g2.drawStr(35,40,"Setup done");
  u8g2.sendBuffer();
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  currentMillis = millis();
  // readButtonState();
  newButtonRoutine();
  newButtonNavigation();

  if (running == true) {
    strokeRoutine();
  }


  // Display selected menu 
  switch (menuLevel) {
    case 1:
      displayMenu();
      break;

    case 10:
      displayStart();
      break;

    case 20:
      displayRunning();
      break;
    
    case 11: 
      displayMenuSettings();
      break;
    
    default:
      displayMenu();
      menuLevel = 1;
      break;
  }

}