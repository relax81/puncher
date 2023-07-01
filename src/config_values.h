#pragma once


// fonts
#define normalFont u8g2_font_t0_11b_mf
#define settingsFont u8g2_font_NokiaSmallBold_tf
#define selectFont u8g2_font_Born2bSportyV2_tf
#define countdownFont u8g2_font_fub20_tf

// variables
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long lastSpankMillis = 0;

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
int startStrokingCurrentMeasuringDelay = 300; // milliseconds
int startReturningCurrentMeasuringDelay = 300; // milliseconds

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