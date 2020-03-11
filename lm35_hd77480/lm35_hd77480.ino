#include <LiquidCrystal.h>

/* Celcius<->Farenheit button */
const uint8_t changeBtn = 2;
const uint8_t lm35 = A0;

/*
   Be aware that DisplayTech 162B has different pinout
   than similar displays

   LCD            - Arduino
   1 (backlight+) - 5V
   2,3 (GND)      - GND
   4 (Vcc)        - 5V
   5 (contrast)   - 20k Potentiometer washer
   6 (RS)         - Pin 3
   7 (R/W)        - GND
   8 (EN)         - Pin 4
   9-12 (D0-D3)   - N/C
   13 (D4)        - Pin 5
   14 (D5)        - Pin 6
   15 (D6)        - Pin 7
   16 (D7)        - Pin 8
*/
LiquidCrystal lcd(3, 4, 5, 6, 7, 8); // RS, EN, D4-D7
volatile bool useCelsius = true;

/* Button routine with SW debouncing */
volatile void celtof() {
  noInterrupts();
  static uint32_t last = 0;
  uint32_t now = millis();

  /* Jitter out, we've got 200ms pulse! */
  if (now - last > 200)
    useCelsius = !useCelsius;

  last = now;
  interrupts();
}

void setup() {
  /* LM35 */
  pinMode(lm35, INPUT);
  /* No extra resistor required (for now) */
  pinMode(changeBtn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(changeBtn), celtof, FALLING);
  lcd.begin(16, 2);
  lcd.clear();
  interrupts();
}

void loop() {
  /* Convert analog reading to voltage */
  double temperature = analogRead(lm35) * 4.8828; // 5000mV/1024samples
  temperature /= 10; // 10mV per degree C

  /* Don't convert to Farenheit unless we must */
  if (!useCelsius) {
    temperature *= (double) 1.8;
    temperature += 32;
  }

  lcd.setCursor(0, 0);
  lcd.print(F("Temperature: "));
  lcd.setCursor(0, 1);
  lcd.print(temperature);
  lcd.print((char)223); // Degree symbol

  /* [condition] ? [if true] : [if false] */
  lcd.print(useCelsius ? F("C") : F("F"));

  /* delay() bad */
  while (millis() % 1000 != 0);
}
