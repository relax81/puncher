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

// butttons
#define btn_up 23
#define btn_enter 19
#define btn_left 18
#define btn_down 4
#define btn_right 13

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
unsigned long elapsedMillis = 0;
int readingDelay = 0; // reading delay after enable to ignore inrush current
bool motorStopped = false;
int reading = 0; // current reading
int currentSum = 0; // sum of current readings
float averageCurrent = 0;
int startDelay = 5; // start delay in seconds
int strokes = 1; // number of strokes
int strokePause = 8; // pause between strokes in seconds
int strokeSpeed = 150; // speed of the stroke pwm 0-255
int returnSpeed = 50; // speed when resetting the motor position pwm 0-255
int strokeCurrentLimit = 0; // current threshold to stop motor during stroke
int returnCurrentLimit = 0; // current threshold to stop motor during return
// buttons
bool buttonLeft = false;
bool buttonRight = false;
bool buttonUp = false;
bool buttonDown = false;
bool buttonEnter = false;
// fonts
#define normalFont u8g2_font_t0_11b_mf
#define settingsFont u8g2_font_NokiaSmallBold_tf
// menu
int settingsSelect = 1;
int menuLevel = 1;
const int MainMenuNumItems = 3; // number of items in the list 
const int MainMenuMaxItemLength = 20; // maximum characters for the item name
char MainMenuItems [MainMenuNumItems] [MainMenuMaxItemLength] = {"Start","Settings","Info"};
int MainMenu_Selected = 0;
int MainMenu_Previous;
int MainMenu_Next;


// Display Type
  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void readButtonState() {
 buttonLeft = digitalRead(btn_left) == LOW;
 buttonRight = digitalRead(btn_right) == LOW;
 buttonUp = digitalRead(btn_up) == LOW;
 buttonDown = digitalRead(btn_down) == LOW;
 buttonEnter = digitalRead(btn_enter) == LOW;


  switch (menuLevel) {
    case 1:
        if (buttonUp) {
          if (MainMenu_Selected < MainMenuNumItems - 1) {
            MainMenu_Selected++;
            delay(100);
            }
          } 
        if (buttonDown) {
          if (MainMenu_Selected > 0) {
            MainMenu_Selected--;
            delay(100);
            }
          }
        if (buttonEnter){
          menuLevel = MainMenu_Selected + 10;
          delay(100);
        }
      break;

    case 11:
      if (buttonDown) {
        if (settingsSelect < 6) {
        settingsSelect++;
        delay(100);
        }
      }
      if (buttonUp) {
        if (settingsSelect > 1) {
        settingsSelect--;
        delay(100);
        }
      } 
      if (buttonEnter) {
        menuLevel = 1;
        delay(100);
      }

      if (buttonLeft){

      }

      if (buttonRight){
        
      }

      break;
  }

}

void strokeRoutine() {
  motorController.Enable();
  int currentSum = 0;

  if (motorStopped == false) {
    motorController.TurnLeft(strokeSpeed);
    delay(200);
    }
  else if (motorStopped == true)
    {
      motorController.Stop();
      delay(100);
      motorController.Enable();
      motorController.TurnRight(returnSpeed);
      debugln("motorcontroler right turn enabled");
      delay(3000);
      motorController.Stop();
      debugln("motorcontroller right turn stopped");
      delay(5000);
      motorStopped = false;
    }

  // take 10 readings and add them up
  for (int i = 0; i < 2; i++) {
    reading = motorController.CurrentSenseLeft();
    currentSum += reading;
    }
  // calculate the average
  averageCurrent = (float) currentSum / 2;

  // int reading = analogRead(L_IS);

  if (averageCurrent > 500 && motorStopped == false)
    {
      motorStopped = true;
      debugln("current sense triggered !!!!!!!!!!!!!!");
    }
  debug("reading: ");
  debugln(reading);
  debugln(motorController.CurrentSenseLeft());
  debug("averageCurrent: ");
  debugln(averageCurrent);
  delay(100);
}

void displayMenu() {
  MainMenu_Previous = (MainMenu_Selected - 1);
  MainMenu_Next = (MainMenu_Selected + 1);

  u8g2.clearBuffer();
  u8g2.drawHLine(0,12,128);
  u8g2.drawStr(47,8, "Spanky");
  u8g2.drawFrame(10,31,108,19);
  u8g2.drawStr(38,24,MainMenuItems[MainMenu_Previous]);
  u8g2.drawStr(38,44,MainMenuItems[MainMenu_Selected]);
  u8g2.drawStr(38,64,MainMenuItems[MainMenu_Next]);
  u8g2.sendBuffer();
}

void displayMenuSettings() {
  u8g2.clearBuffer();
  u8g2.drawVLine(78, 0, 64);
  u8g2.setFont(settingsFont);
  u8g2.drawStr(0,8,"stroke Speed");
  u8g2.drawStr(0,18,"return Speed");
  u8g2.drawStr(0,28,"stroke Limit");
  u8g2.drawStr(0,38,"return Limit");
  u8g2.drawStr(0,48,"stroke pause");
  u8g2.drawStr(0,58,"start delay");

  u8g2.setCursor(90,8);
  u8g2.print(strokeSpeed);
  u8g2.setCursor(90,18);
  u8g2.print(returnSpeed);
  u8g2.setCursor(90,28);
  u8g2.print(strokeCurrentLimit);
  u8g2.setCursor(90,38);
  u8g2.print(returnCurrentLimit);
  u8g2.setCursor(90,48);
  u8g2.print(strokePause);
  u8g2.setCursor(90,58);
  u8g2.print(startDelay);
  

  switch (settingsSelect) {
    case 1: 
      u8g2.drawStr(120,8,"*");
      break;
    case 2: 
      u8g2.drawStr(120,18,"*");
      break;
    case 3: 
      u8g2.drawStr(120,28,"*");
      break;
    case 4: 
      u8g2.drawStr(120,38,"*");
      break;
    case 5: 
      u8g2.drawStr(120,48,"*");
      break;
    case 6: 
      u8g2.drawStr(120,58,"*");
      break;
    default:
      break;
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
  readButtonState();
  // strokeRoutine();

  // Display selected menu
  switch (menuLevel) {
    case 1:
      displayMenu();
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