#include "wleveling.h"
#include "EEPROM.h"

#define DIRTY_FLAG_1 0b01010101
#define DIRTY_FLAG_2 0b10101010

WLeveling::WLeveling(int eepromSize, int dataSize) {
  this->eepromSize = eepromSize;
  this->dataSize = dataSize;

  // Get dirty flag value
  this->dirtyFlag = EEPROM.read(0);
  if (this->dirtyFlag != DIRTY_FLAG_1 && this->dirtyFlag != DIRTY_FLAG_2) {
    // Not a correct dirty flag: the EEPROM must be initialized
    this->dirtyFlag = DIRTY_FLAG_1;
    
    EEPROM.write(0, this->dirtyFlag);           // Write first dirty flag
    for (int i = 0; i < this->dataSize; i++) {  // First data is empty
      EEPROM.write(i + 1, 0);
    }
    EEPROM.write(this->dataSize + 1, 0);        // Next flag must not be a dirty flag
  }

  // Search the next address to write
  this->writeAddress = 0;
  this->readAddress = 0;
  while (this->writeAddress < this->eepromSize) {
    byte flag = EEPROM.read(this->writeAddress);
    if (flag != this->dirtyFlag) {
      break;
    }

    this->readAddress = this->writeAddress;
    this->writeAddress += this->dataSize + 1;
  }
  if (this->writeAddress >= this->eepromSize) {
    // Reset at first address
    this->dirtyFlag = (this->dirtyFlag == DIRTY_FLAG_1) ? DIRTY_FLAG_2 : DIRTY_FLAG_1;
    this->writeAddress = 0;
  }
}

void WLeveling::read(byte *data) {
  for (int i = 0; i < this->dataSize; i++) {
    data[i] = EEPROM.read(this->readAddress + i + 1);
  }
}

void WLeveling::write(byte *data) {
  EEPROM.write(this->writeAddress, this->dirtyFlag);
  for (int i = 0; i < this->dataSize; i++) {
    EEPROM.write(this->writeAddress + i + 1, data[i]);
  }
  this->readAddress = this->writeAddress;
  this->writeAddress += dataSize + 1;
  if (this->writeAddress >= this->eepromSize) {
    this->dirtyFlag = (this->dirtyFlag == DIRTY_FLAG_1) ? DIRTY_FLAG_2 : DIRTY_FLAG_1;
    this->writeAddress = 0;
  }
}
