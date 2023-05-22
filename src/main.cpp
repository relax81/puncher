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


const uint8_t EN_L = 33;
const uint8_t EN_R = 25;
const uint8_t L_PWM = 26;
const uint8_t R_PWM = 15;
const uint8_t R_IS = 34;
const uint8_t L_IS = 35;

BTS7960 motorController(EN_L, EN_R, L_PWM, R_PWM, L_IS, R_IS);

bool motorStopped = false;
int reading = 0;
float averageCurrent = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  motorController.Enable();
  int speed = 200;
  int currentSum = 0;

  if (motorStopped == false) {
    speed = 1000;
    motorController.TurnLeft(speed);
    delay(200);
    }
  else if (motorStopped == true)
    {
      motorController.Stop();
      debugln("stopped");
      speed = 50;
      debug("speed = ");
      debugln(speed);
      delay(100);
      motorController.Enable();
      motorController.TurnRight(speed);
      debugln("motorcontroler right turn enabled");
      delay(500);
      debugln("motorcontroller right turn stopped");
      motorController.Stop();
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

  if (averageCurrent > 1000 && motorStopped == false)
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