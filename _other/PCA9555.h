#include <Arduino.h>
#include <Wire.h>

class PCA9555 {
  private:
    uint8_t _address;
    uint16_t _configCache;
    uint16_t _outputCache;

    void write16(uint8_t reg, uint16_t value) {
      Wire.beginTransmission(_address);
      Wire.write(reg);
      Wire.write(value & 0xFF);
      Wire.write((value >> 8) & 0xFF);
      Wire.endTransmission();
    }

    uint16_t read16(uint8_t reg) {
      Wire.beginTransmission(_address);
      Wire.write(reg);
      Wire.endTransmission();
      Wire.requestFrom(_address, (uint8_t)2);
      uint16_t low = Wire.read();
      uint16_t high = Wire.read();
      return (high << 8) | low;
    }

  public:
    PCA9555(uint8_t address = 0x20) : _address(address) {}

    void init(uint16_t config = 0xFFFF) {
      _configCache = config;
      _outputCache = 0x0000;
      write16(0x06, _configCache); // Set I/O direction
      write16(0x02, _outputCache); // Set all outputs LOW
      clearInterrupt();            // Initial read to clear INT pin
    }

    // Reading the input register clears the PCA9555 interrupt signal
    uint16_t clearInterrupt() {
      return read16(0x00);
    }

    void setPin(uint8_t pin, bool state) {
      if (pin > 15) return;
      if (state) _outputCache |= (1 << pin);
      else _outputCache &= ~(1 << pin);
      write16(0x02, _outputCache);
    }

    bool getPin(uint8_t pin) {
      return (read16(0x00) >> pin) & 0x01;
    }

    void setAll(uint16_t values) {
      _outputCache = values;
      write16(0x02, _outputCache);
    }

    uint16_t getAll() {
      return read16(0x00);
    }
};
