#include "wleveling.h"

struct Test {
  int int1;
  int int2;
};

WLeveling wleveling = WLeveling(EEPROM_SIZE_ATmega328P, sizeof(Test));
int i = 0;
Test value;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    Serial.read();
    wleveling.read((byte*) &value);
    print(&value);

    value.int1 = i;
    value.int2 = i;
    wleveling.write((byte*) &value);
    
    i++;
  }
  delay(10);
}

void print(Test *value) {
  Serial.print("int1: ");
  Serial.print(value->int1);
  Serial.print(" int2: ");
  Serial.println(value->int2);
}
