/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
 */
#if !defined(PIN_LED1)
  #define PIN_LED1 13
#endif

int STEP_PIN = 1;
int DIR_PIN = 0;

void setup() {
  // initialize the digital pin as an output.
  // Pin PIN_LED1 has an LED connected on most Arduino and compatible boards:
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  
}

void loop() {
  digitalWrite(DIR_PIN, 1);
  for(int i = 0; i < 1000; i++) {
  digitalWrite(STEP_PIN, HIGH);   // set the LED on
  delayMicroseconds(1000);              // wait for a second
  digitalWrite(STEP_PIN, LOW);    // set the LED off
  delayMicroseconds(1000);              // wait for a second
  }
  
  digitalWrite(DIR_PIN, 0);  
  for(int i = 0; i < 1000; i++) {
  digitalWrite(STEP_PIN, HIGH);   // set the LED on
  delayMicroseconds(1000);              // wait for a second
  digitalWrite(STEP_PIN, LOW);    // set the LED off
  delayMicroseconds(1000);              // wait for a second
  }
}

