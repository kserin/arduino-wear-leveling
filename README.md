# arduino-wear-leveling

Arduino library to prolong the EEPROM life.
Arduino EEPROM has a specified life of 100 000 write cycles ( https://www.arduino.cc/en/Reference/EEPROMWrite ).

## Installation

- Download release here : https://github.com/kserin/arduino-wear-leveling/releases/download/v0.0.1/wleveling.zip
- Put zip content on `libraries` folder of Arduino IDE
- Restart IDE

## Getting started

```cpp
#include "wleveling.h"

// Structure that will be stored in EEPROM
struct Test {
  int int1;
  int int2;
};

// Initialize wear leveling
WLeveling wleveling = WLeveling(EEPROM_SIZE_ATmega328P, sizeof(Test));

Test value;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) { // Wait for input to write a value
    Serial.read();

    // Read previous stored value (will be zero at the first execution)
    wleveling.read((byte*) &value);
    print(&value);

    // Write new values
    value.int1 = 123;
    value.int2 = 456;
    wleveling.write((byte*) &value);
  }
  delay(1);
}

void print(Test *value) {
  Serial.print("int1: ");
  Serial.print(value->int1);
  Serial.print(" int2: ");
  Serial.println(value->int2);
}
```

## How it works

The goal is to use all the EEPROM available. Each new write is done on a new address instead of re-using always the same address.
This is why, at initialization, `eepromSize` and `dataSize` must be given:

```cpp
/*
 * The following constant can be used for eepromSize : 
 * EEPROM_SIZE_ATmega328P, EEPROM_SIZE_ATmega168, EEPROM_SIZE_ATmega8, EEPROM_SIZE_ATmega1280, EEPROM_SIZE_ATmega2560
 */
WLeveling wleveling = WLeveling(EEPROM_SIZE_ATmega328P, sizeof(YourDataStruct));
```

At each new writing, the next EEPROM address to write is computed and a flag (called "dirty flag") is added before the data.
The dirty flag is a `byte` that can have two possible values (`0b01010101` and `0b10101010`). When all the EEPROM is full, the dirty flag value swap and writings restart at the first EEPROM address.

At initialization, to find the next address to write, the first `byte` is readed (it always contains the current dirty flag value). Then the EEPROM is readed until a different value than the dirty flag is found.

Supposing `sizeof(DataStruct)` is 4, the EEPROM records look like this:
<table>
  <tr>
    <th width="100">#</td>
    <th width="100">Address</td>
    <th width="100">Flag</td>
    <th width="500">Data</td>
  </tr>
  <tr>
    <td>1st writing</td>
    <td>0000</td>
    <td>01010101</td>
    <td>Data</td>
  </tr>
  <tr>
    <td>2nd writing</td>
    <td>0005</td>
    <td>01010101</td>
    <td>Data</td>
  </tr>
  <tr>
    <td>free</td>
    <td>0010</td>
    <td>11111111</td>
    <td>empty</td>
  </tr>
  <tr align="center">
    <td colspan="4">...</td>
  </tr>
</table>