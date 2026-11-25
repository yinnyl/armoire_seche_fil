#include "HX711.h"
#include <LiquidCrystal_I2C.h>
#include <Pushbutton.h>

HX711 scale_right;
HX711 scale_left;

float calibration_factor_right = 188.4*2; // multiplication of both calibration factors by 2 as they were calculated while already installed under the rack
float calibration_factor_left = 188.55*2;

int lcd_columns = 16;
int lcd_rows = 2;
LiquidCrystal_I2C lcd(0x27, lcd_columns, lcd_rows); 

#define BUTTON_PIN_TARE 23
Pushbutton button_tare(BUTTON_PIN_TARE);

#define BUTTON_PIN_MEM 26
Pushbutton button_mem(BUTTON_PIN_MEM);

int press_count = 0;
float weight1 = 0;
float weight2 = 0;

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  scale_right.begin(16, 4);   // DT = GPIO 16, SCK = GPIO 4
  scale_left.begin(18, 4);  // DT = GPIO 18, SCK = GPIO 19 or also 4 as the load cells can work on the same SCK pin

  scale_right.set_scale(calibration_factor_right);
  scale_left.set_scale(calibration_factor_left);

  scale_right.tare(); // reset scale
  scale_left.tare(); // reset scale
}

void loop() {

  if (button_tare.getSingleDebouncedPress())
  {
    Serial.print("tare...");
    scale_right.tare();
    scale_left.tare();
  }

  // read each sensor (averaging 5 readings for stability)
  float weight_right = scale_right.get_units(5);
  float weight_left = scale_left.get_units(5);

  // combine into a single total weight reading
  float total_weight = weight_right + weight_left;

if (button_mem.getSingleDebouncedPress()) {
    press_count++;
    if (press_count == 1) 
    {
      weight1 = total_weight;
    } 
    else if (press_count == 2) 
    {
      weight2 = total_weight - weight1;
    } 
    else 
    {
      press_count = 0;
      weight1 = 0;
      weight2 = 0;
    }
  }

  Serial.print("Total Weight: ");
  Serial.print(total_weight, 2); // two decimal places precision
  Serial.println(" g");
  Serial.print("Weight Object 1: ");
  Serial.print(weight1, 2); // two decimal places precision
  Serial.println(" g");
  Serial.print("Weight Object 2: ");
  Serial.print(weight2, 2); // two decimal places precision
  Serial.println(" g");

  lcd.setCursor(0, 0);
  lcd.print("Total [g] ");
  lcd.print(total_weight, 2);
  lcd.setCursor(0, 1);
  lcd.print("1:");
  lcd.print(weight1, 2);
  lcd.print(" 2:");
  lcd.print(weight2, 2);

  delay(200);
}
