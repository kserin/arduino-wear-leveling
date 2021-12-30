#ifndef WLEVELING_h
#define WLEVELING_h

#include "Arduino.h"
#include "EEPROM.h"

#define EEPROM_SIZE_ATmega328P 1024
#define EEPROM_SIZE_ATmega168 512
#define EEPROM_SIZE_ATmega8 512
#define EEPROM_SIZE_ATmega1280 4096
#define EEPROM_SIZE_ATmega2560 4096

class WLeveling {
  public:
    WLeveling(int eepromSize, int dataSize);
    void read(byte *data);
    void write(byte *data);

  private:
    byte dirtyFlag;
    int readAddress;
    int writeAddress;
    int eepromSize;
    int dataSize;
};

#endif
