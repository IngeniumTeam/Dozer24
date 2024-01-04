#include <Bluetooth.h>

#define NUM_VALUES 9

int sizes[NUM_VALUES] = { 1, 4, 8, 9, 9, 1, 9, 9, 1 };
Bluetooth bluetooth(&Serial1, sizes, NUM_VALUES);

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
}

void loop() {
  if (bluetooth.receive()) {
    for (int i = 0; i < NUM_VALUES; i++) {
      Serial.print(bluetooth.message.get(i));
      Serial.print(" | ");
    }
    Serial.println();
  }
}
