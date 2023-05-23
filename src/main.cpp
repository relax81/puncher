////pinout notes
//Button
// up 23
// center 19
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
#include "BTS7960.h"

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


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  currentMillis = millis();

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