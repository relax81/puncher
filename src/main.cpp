////pinout notes
//Button
// up 23
// enter 19
// left 18
// down 4
// right 13

//// BTS7960
// sense pins 35
// en_L 33
// en_R 25
// pwm_L 27
// pwm_R 14

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
#include <U8g2lib.h>
#include "BTS7960.h"

// button pins
#define btn_up 23
#define btn_enter 19
#define btn_left 18
#define btn_down 4
#define btn_right 13

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
const uint8_t EN_L = 33;
const uint8_t EN_R = 25;
const uint8_t L_PWM = 27;
const uint8_t R_PWM = 14;
const uint8_t R_IS = 35; // unused - parallel with 35 
const uint8_t L_IS = 35;
BTS7960 motorController(EN_L, EN_R, L_PWM, R_PWM, L_IS, R_IS);

// variables
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long lastSpankMillis = 0;
unsigned long startStrokingCurrentMeasuringDelay = 300; // milliseconds
unsigned long startReturningCurrentMeasuringDelay = 300; // milliseconds

bool motorStopped = false;
bool stroking = false;
bool returning = false;
bool running = false;
bool startFirstSpankDelay = false;
int reading = 0; // current reading
int currentSum = 0; // sum of current readings
float averageCurrent = 0;
int startDelay = 5; // start delay in seconds
int pauseBetweenStrokes = 20; // delay between strokes in seconds
int amountOfStrokes = 1; // number of strokes
int strokeSpeed = 200; // speed of the stroke pwm 0-255
int returnSpeed = 50; // speed when resetting the motor position pwm 0-255
int strokeCurrentLimit = 50; // current threshold to stop motor during stroke
int returnCurrentLimit = 50; // current threshold to stop motor during return

// fonts
#define normalFont u8g2_font_t0_11b_mf
#define settingsFont u8g2_font_NokiaSmallBold_tf
#define selectFont u8g2_font_Born2bSportyV2_tf

// menu
int menuLevel = 1;
int currentMenuIndex = 3;
const int MainMenuNumItems = 3; // number of items in the list 
const int MainMenuMaxItemLength = 20; // maximum characters for the item name
char MainMenuItems [MainMenuNumItems] [MainMenuMaxItemLength] = {"Start","Settings","Info"};
int MainMenu_Selected = 1;
int MainMenu_Previous;
int MainMenu_Next;
int StartMenu_Selected = 1;

//// settings menu array
// Define the struct for menu items
struct MenuItem {
  const char* name;
  int value;
};
// Define the menu array
MenuItem menu[] = {
  {"stroke Speed", strokeSpeed},
  {"return Speed", returnSpeed},
  {"stroke Limit", strokeCurrentLimit},
  {"return Limit", returnCurrentLimit},
  {"stroke pause", pauseBetweenStrokes},
  {"start delay", startDelay},
  {"stroke IS delay", startStrokingCurrentMeasuringDelay},
  {"return IS delay", startReturningCurrentMeasuringDelay}
};
// Total number of menu items
const int menuSize = sizeof(menu) / sizeof(menu[0]);

void strokeRoutine();
void stroke();
void returnMotor();


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
        menuLevel = 1;
      }
      if (btn_left_short) {
        menu[currentMenuIndex].value--;
      }

      if (btn_left_long) {
        menu[currentMenuIndex].value = menu[currentMenuIndex].value - 10;
      }

      if (btn_right_short) {
        menu[currentMenuIndex].value++;
      }

      if (btn_right_long) {
        menu[currentMenuIndex].value = menu[currentMenuIndex].value + 10;
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



  // int currentSum = 0;
  // if ((motorStopped == false) && (amountOfStrokes > 0)) {
  //   motorController.TurnLeft(strokeSpeed);
  //   amountOfStrokes--;
  //   delay(200);
  //   }
  // else if (motorStopped == true)
  //   {
  //     motorController.Stop();
  //     delay(100);
  //     motorController.Enable();
  //     motorController.TurnRight(returnSpeed);
  //     debugln("motorcontroler right turn enabled");
  //     delay(3000);
  //     motorController.Stop();
  //     debugln("motorcontroller right turn stopped");
  //     delay(5000);
  //     motorStopped = false;
  //   }

  // // take 10 readings and add them up
  // for (int i = 0; i < 2; i++) {
  //   reading = motorController.CurrentSenseLeft();
  //   currentSum += reading;
  //   }
  // // calculate the average
  // averageCurrent = (float) currentSum / 2;

  // // int reading = analogRead(L_IS);

  // if (averageCurrent > 500 && motorStopped == false)
  //   {
  //     motorStopped = true;
  //     debugln("current sense triggered !!!!!!!!!!!!!!");
  //   }


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
  u8g2.setCursor(55,14);
  u8g2.print(amountOfStrokes);
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
  u8g2.clearBuffer();
  u8g2.setFont(normalFont);
  u8g2.drawStr(20,10,"Strokes Left");
  u8g2.setCursor(50,35);
  u8g2.print(amountOfStrokes);
  u8g2.sendBuffer();
  running = true;
}

// new style
void displayMenuSettings() {
  u8g2.clearBuffer();
  
  // frame
  u8g2.drawFrame(0,19,126,19);
  // left column
  u8g2.setFont(settingsFont);
  u8g2.setCursor(2,8);
  if (currentMenuIndex > 1) {
    u8g2.print(menu[currentMenuIndex-2].name);
    }
  u8g2.setCursor(2,18);
  if (currentMenuIndex > 0){
  u8g2.print(menu[currentMenuIndex-1].name);
  }
  u8g2.setFont(selectFont);
  u8g2.setCursor(2,33);
  u8g2.print(menu[currentMenuIndex].name);
  u8g2.setFont(settingsFont);
  u8g2.setCursor(2,48);
  if (currentMenuIndex < (menuSize - 1)) {
  u8g2.print(menu[currentMenuIndex+1].name);
  }
  u8g2.setCursor(2,58);
  if (currentMenuIndex < (menuSize - 2)){
  u8g2.print(menu[currentMenuIndex+2].name);
  }
  
  // right column
  u8g2.setCursor(100,8);
  if (currentMenuIndex > 1) {
  u8g2.print(menu[currentMenuIndex-2].value);
  }
  u8g2.setCursor(100,18);
  if (currentMenuIndex > 0){
  u8g2.print(menu[currentMenuIndex-1].value);
  }
  u8g2.setFont(selectFont);
  u8g2.setCursor(100,33);
  u8g2.print(menu[currentMenuIndex].value);
  u8g2.setFont(settingsFont);
  u8g2.setCursor(100,48);
  if (currentMenuIndex < (menuSize - 1)) {
  u8g2.print(menu[currentMenuIndex+1].value);
  }
  u8g2.setCursor(100,58);
  if (currentMenuIndex < (menuSize - 2)){
  u8g2.print(menu[currentMenuIndex+2].value);
  }

  u8g2.sendBuffer();
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