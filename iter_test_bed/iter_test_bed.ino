#include "HX711.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "A4988.h"

//define some useful words
#define MOTOR_STEPS 200
#define RPM 2
#define MICROSTEPS 16
//stress gauge pins
#define DT_L 12
#define SCK_L 13
#define DT_R 10
#define SCK_R 11
//button pin
#define BUTTON A3
//step motor driver pins
#define STEP_1 7
#define DIR_1 8
#define EN_1 9
#define STEP_2 4
#define DIR_2 5
#define EN_2 6
//initiate classes
BasicStepperDriver stepper_1(MOTOR_STEPS, DIR_1, STEP_1);
BasicStepperDriver stepper_2(MOTOR_STEPS, DIR_2, STEP_2);
HX711 scale_1;
HX711 scale_2;
LiquidCrystal_I2C lcd(0x27, 16, 2);
//some more useful vars
float calibration_factor_L = 2.53;
float calibration_factor_R = -0.08;
float units_1 = 0.0;
float units_2 = 0.0;
float screen_var_1;
float screen_var_2;
int buttonState = 0;
bool isCalibrated = false;

//calibrates vertical stepper motor
void calibrate_1() {
  if (isCalibrated == false) {
    for (int i = 1; i <= 200; i++) {
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
  Serial.begin(9600);
  scale_1.begin(DT_L, SCK_L);
  scale_1.set_scale();
  scale_1.tare();
  scale_1.set_scale(calibration_factor_L);

  scale_2.begin(DT_R, SCK_R);
  scale_2.set_scale();
  scale_2.tare();
  scale_2.set_scale(calibration_factor_R);
  lcd.init();
  lcd.backlight();

  stepper_1.begin(RPM, MICROSTEPS);
  stepper_2.begin(RPM, MICROSTEPS);
  pinMode(BUTTON, INPUT);
  calibrate_1();
}

void loop() {
  stepper_1.rotate(-100);

  for (int i = 0; i < 500; i++) {
    units_1 += 0.1 * (scale_1.read() - units_1);
    screen_var_1 = units_1 / 13749.6216 - 31 - 1.2 - 1.5;
    units_2 += 0.3 * (scale_2.read() - units_2);
    screen_var_2 = -(units_2 / 15937.8787 - 69.73);
    lcd.setCursor(0, 0);
    lcd.print(screen_var_1);
    lcd.setCursor(0, 1);
    lcd.print(screen_var_2);
  }
  delay(2000);
  stepper_1.rotate(-80);
  delay(2000);
  stepper_1.rotate(-60);
  delay(2000);
  stepper_1.rotate(-22);

  for (int i = 0; i < 500; i++) {
    units_1 += 0.1 * (scale_1.read() - units_1);
    screen_var_1 = units_1 / 13749.6216 - 31 - 1.2 - 1.5;
    units_2 += 0.3 * (scale_2.read() - units_2);
    screen_var_2 = -(units_2 / 15937.8787 - 69.73);
    lcd.setCursor(0, 0);
    lcd.print(screen_var_1);
    lcd.setCursor(0, 1);
    lcd.print(screen_var_2);
  }
  delay(2000);
  stepper_1.rotate(-100);
  delay(2000);
  isCalibrated = false;
  calibrate_1();
  delay(2000);
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