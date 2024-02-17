#include <Servo.h>

Servo servo;

int testPin = 6;

void setup() {
  Serial.begin(9600);
  servo.attach(testPin);
  servo.write(30);
}

void loop() {
  servo.write(0);
  Serial.println("0");
  delay(2000);
  servo.write(30);
  Serial.println("180");
  delay(2000);
}
