#include "HX711.h"                                           
#include "Wire.h" 
#include "LiquidCrystal_I2C.h"
#include "A4988.h"
//define some useful words
#define MOTOR_STEPS 200
#define RPM 2
#define MICROSTEPS 16
//stress gauge pins
#define DT_L  12                                                
#define SCK_L 13
#define DT_R 10
#define SCK_R 11     
//button pin
#define BUTTON  2
//step motor driver pins
#define STEP_1 3
#define DIR_1 4
#define SLP_1 5
// initiate classes
BasicStepperDriver stepper_1(MOTOR_STEPS, DIR_1, STEP_1);
BasicStepperDriver stepper_2(MOTOR_STEPS, DIR_1, STEP_1);
HX711 scale_L;
HX711 scale_R;                                                    
LiquidCrystal_I2C lcd(0x27,16,2);
//vars
float calibration_factor_L = -14.15;
float calibration_factor_R = -14.15;                                         
float units;                                                  
float ounces; 
int buttonState = 0;
bool isCalibrated = false;                                                

void calibrate_1() {
  if (isCalibrated == false) {
    for(int i = 1; i <= 200; i++) {
    stepper_1.rotate(-3);
    buttonState = digitalRead(BUTTON);
    if (buttonState == LOW) {
      stepper_1.rotate(20);
      delay(1000);
      isCalibrated = true;
      break;
      }
      delay(50);
    }
  }
}

void setup() {   
  // initialize and calibrate scales                                    
  scale_L.begin(DT_L, SCK_L);
  scale_L.set_scale();
  scale_L.tare();
  scale_L.set_scale(calibration_factor_L);
  scale_R.begin(DT_R, SCK_R);  
  scale_R.set_scale();   
  scale_R.tare();
  scale_R.set_scale(calibration_factor_R);
  // initialize lcd
  lcd.backlight();
  // initialize stepper motors
  stepper_1.begin(RPM, MICROSTEPS);
  stepper_2.begin(RPM, MICROSTEPS);
  pinMode(BUTTON, INPUT);
  calibrate_1();
  delay(1000);
}

void loop() {     
  //loop scale read L                           
  for (int i = 0; i < 10; i ++) {                             
    units = + scale_L.get_units(), 10;                          
  }
  units = units / 10;                                         
  ounces = units * 0.035274;
  lcd.setCursor(0,0);                                 
  lcd.print(ounces);                                            
  //loop scale read R
  for (int i = 0; i < 10; i ++) {                             
    units = + scale_R.get_units(), 10;                          
  }
  units = units / 10;                                         
  ounces = units * 0.035274;
  lcd.setCursor(1,0);                                 
  lcd.print(ounces);
  //main movement operation
  stepper_1.rotate(-100);
  delay(1000);
  stepper_1.rotate(-80);
  delay(1000);
  stepper_1.rotate(-60);
  delay(1000);
  stepper_1.rotate(-22);
  delay(1000);
  stepper_1.rotate(-100);
  delay(1000);
  isCalibrated = false;
  calibrate();
  delay(1000);                                                               
}

//possible modes for A4988 driver: 
// L L L FULL STEP 1
// H L L HALF STEP 2
// L H L QUARTER STEP 4
// H H L EIGTH STEP 8
// H H H SIXTEENTH STEP 16
// Set MICROSTEPS as DIVIDER of step
// EXAMPLE
// For quarter step mode, do MICROSTEPS = 4