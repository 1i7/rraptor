
// для поиска пина в диапазоне
int pin_from = 0;
int pin_to = 40;

// для для проверки конкретного пина
//int pin_from = 2;
//int pin_to = 2;

// Драйвер1 - step=36
//int pin_from = 36;
//int pin_to = 36;

// Драйвер2 - step=38
//int pin_from = 38;
//int pin_to = 38;

// Драйвер3 - step=40
//int pin_from = 40;
//int pin_to = 40;

// Драйвер4 - step=2
//int pin_from = 2;
//int pin_to = 2;

// Драйвер5 - step=4
//int pin_from = 4;
//int pin_to = 4;

// Драйвер6 - step=6
//int pin_from = 6;
//int pin_to = 6;


void setup() {
  for(int i=pin_from; i<=pin_to; i++) {
      pinMode(i, OUTPUT);
  }
}

// the loop function runs over and over again forever
void loop() {
  for(int i=pin_from; i<=pin_to; i++) {
      digitalWrite(i, HIGH); 
  }
  delay(1);       
  for(int i=pin_from; i<=pin_to; i++) {
      digitalWrite(i, LOW); 
  }
  delay(10);
}
