#include <Arduino.h>

class SoftI2C {
  private:
    uint8_t _sda, _scl;
    const uint8_t _delay = 4; // Approx 100kHz

    void i2c_start() {
      digitalWrite(_sda, HIGH);
      digitalWrite(_scl, HIGH);
      delayMicroseconds(_delay);
      digitalWrite(_sda, LOW);
      delayMicroseconds(_delay);
      digitalWrite(_scl, LOW);
    }

    void i2c_stop() {
      digitalWrite(_sda, LOW);
      delayMicroseconds(_delay);
      digitalWrite(_scl, HIGH);
      delayMicroseconds(_delay);
      digitalWrite(_sda, HIGH);
    }

    bool i2c_write(uint8_t data) {
      for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(_sda, (data & 0x80) ? HIGH : LOW);
        data <<= 1;
        digitalWrite(_scl, HIGH);
        delayMicroseconds(_delay);
        digitalWrite(_scl, LOW);
      }
      // Read ACK
      pinMode(_sda, INPUT);
      digitalWrite(_scl, HIGH);
      bool ack = !digitalRead(_sda);
      digitalWrite(_scl, LOW);
      pinMode(_sda, OUTPUT);
      return ack;
    }

    uint8_t i2c_read(bool ack) {
      uint8_t data = 0;
      pinMode(_sda, INPUT);
      for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(_scl, HIGH);
        if (digitalRead(_sda)) data |= (1 << (7 - i));
        digitalWrite(_scl, LOW);
      }
      pinMode(_sda, OUTPUT);
      digitalWrite(_sda, ack ? LOW : HIGH); // Send ACK or NACK
      digitalWrite(_scl, HIGH);
      delayMicroseconds(_delay);
      digitalWrite(_scl, LOW);
      return data;
    }

  public:
    SoftI2C(uint8_t sda, uint8_t scl) : _sda(sda), _scl(scl) {
      pinMode(_sda, OUTPUT);
      pinMode(_scl, OUTPUT);
    }

    void writeReg16(uint8_t addr, uint8_t reg, uint16_t val) {
      i2c_start();
      i2c_write(addr << 1);
      i2c_write(reg);
      i2c_write(val & 0xFF);
      i2c_write(val >> 8);
      i2c_stop();
    }

    uint16_t readReg16(uint8_t addr, uint8_t reg) {
      i2c_start();
      i2c_write(addr << 1);
      i2c_write(reg);
      i2c_start(); // Repeated start
      i2c_write((addr << 1) | 1);
      uint8_t low = i2c_read(true);
      uint8_t high = i2c_read(false);
      i2c_stop();
      return (high << 8) | low;
    }
};

class PCA9555_NoWire {
  private:
    SoftI2C _i2c;
    uint8_t _addr;
    uint16_t _outCache;

  public:
    PCA9555_NoWire(uint8_t sda, uint8_t scl, uint8_t addr = 0x20) 
      : _i2c(sda, scl), _addr(addr) {}

    void init(uint16_t config = 0xFFFF) {
      _i2c.writeReg16(_addr, 0x06, config);
    }

    void setPin(uint8_t pin, bool state) {
      if (state) _outCache |= (1 << pin);
      else _outCache &= ~(1 << pin);
      _i2c.writeReg16(_addr, 0x02, _outCache);
    }

    bool getPin(uint8_t pin) {
      return (_i2c.readReg16(_addr, 0x00) >> pin) & 0x01;
    }

    void setAll(uint16_t val) {
      _outCache = val;
      _i2c.writeReg16(_addr, 0x02, _outCache);
    }

    uint16_t getAll() {
      return _i2c.readReg16(_addr, 0x00);
    }
};
