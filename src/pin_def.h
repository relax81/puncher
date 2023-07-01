#pragma once

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

// button pins
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