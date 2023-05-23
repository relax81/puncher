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
int strokes = 0; // number of strokes
int pauseBetweenStrokes = 0; // pause between strokes
int strokeSpeed = 200; // speed of the stroke
int returnSpeed = 50; // speed when resetting the motor position

// Display Type
  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void button() {
  if (digitalRead(btn_up) == LOW){
    u8g2.clearBuffer();
    u8g2.drawStr(0,40,"up");
    u8g2.sendBuffer();
  } 
  else if (digitalRead(btn_down) == LOW){
    u8g2.clearBuffer();
    u8g2.drawStr(0,40,"down");
    u8g2.sendBuffer();
  } 
  else if (digitalRead(btn_right) == LOW){
    u8g2.clearBuffer();
    u8g2.drawStr(0,40,"right");
    u8g2.sendBuffer();
  }
  else if (digitalRead(btn_left) == LOW){
    u8g2.clearBuffer();
    u8g2.drawStr(0,40,"left");
    u8g2.sendBuffer();
  }
    else if (digitalRead(btn_enter) == LOW){
    u8g2.clearBuffer();
    u8g2.drawStr(0,40,"enter");
    u8g2.sendBuffer();
  }
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
  u8g2.setFont(u8g2_font_t0_11b_mf);
  u8g2.drawStr(2,30,"Hello World!");
  u8g2.sendBuffer();
}

void loop() {
  // put your main code here, to run repeatedly:
  currentMillis = millis();
  button();


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