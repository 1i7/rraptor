/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

int motor1_pin1 = 0;
int motor1_pin2 = 1;
int motor1_pin3 = 3;


void setup() {
    pinMode(motor1_pin1, OUTPUT);
    pinMode(motor1_pin2, OUTPUT);
    pinMode(motor1_pin3, OUTPUT);    
}

void loop() {
    digitalWrite(motor1_pin2, 1);
    digitalWrite(motor1_pin3, 1);
    
    while(true) {
        digitalWrite(motor1_pin1, HIGH);
        delay(1);
        digitalWrite(motor1_pin1, LOW);
        delay(1);
    }
}
