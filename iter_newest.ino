#include "HX711.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "A4988.h"

//define some useful words
#define MOTOR_STEPS 200
#define RPM_V 3
#define RPM_H 60
#define MICROSTEPS 16
//button pin
#define BUTTON A3
//step motor driver pins
#define STEP_1 4
#define DIR_1 5
#define EN_1 6
#define STEP_2 7
#define DIR_2 8
#define EN_2 9
//initiate classes
BasicStepperDriver stepper_1(MOTOR_STEPS, DIR_1, STEP_1);
BasicStepperDriver stepper_2(MOTOR_STEPS, DIR_2, STEP_2);
int buttonState = 0;
bool isCalibrated = false;

//calibrates vertical stepper motor
void calibrate_1() {
  if (isCalibrated == false) {
    for (int i = 1; i <= 200; i++) {
      stepper_1.rotate(-3);
      buttonState = digitalRead(BUTTON);
      if (buttonState == LOW) {
        stepper_1.rotate(2);
        delay(1000);
        isCalibrated = true;
        break;
      }
      delay(50);
    }
  }
}

void setup() {
  stepper_1.begin(RPM_V, MICROSTEPS);
  stepper_2.begin(RPM_H, MICROSTEPS);
  pinMode(BUTTON, INPUT);
}

void loop() {
  calibrate_1();
  delay(1000);
  stepper_2.rotate(8000);
  delay(1000);
  stepper_2.rotate(-8000);
  delay(1000);
  stepper_1.rotate(-103);
  delay(1000);
  stepper_1.rotate(-80);
  delay(1000);
  stepper_1.rotate(-60);
  delay(1000);
  stepper_1.rotate(-20);
  isCalibrated = false;
  delay(500);
}
