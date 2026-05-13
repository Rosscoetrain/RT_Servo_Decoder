#include <Arduino.h>

class PCA9555_Fast {
  private:
    uint8_t _addr;
    uint16_t _outCache;

    // Direct mapping for Nano: SDA = A4 (PC4), SCL = A5 (PC5)
    inline void sda_high() { DDRC &= ~(1 << 4); } // Input (pulled high by resistor)
    inline void sda_low()  { DDRC |= (1 << 4); PORTC &= ~(1 << 4); }
    inline void scl_high() { DDRC &= ~(1 << 5); } 
    inline void scl_low()  { DDRC |= (1 << 5); PORTC &= ~(1 << 5); }
    inline bool sda_read() { return (PINC & (1 << 4)); }

    // Fast delay for ~400kHz (Adjust NOPs if needed)
    inline void i2c_delay() {
      asm volatile("nop\nnop\nnop\nnop\n"); 
    }

    void i2c_start() {
      sda_high(); scl_high(); i2c_delay();
      sda_low();  i2c_delay();
      scl_low();  i2c_delay();
    }

    void i2c_stop() {
      sda_low();  scl_high(); i2c_delay();
      sda_high(); i2c_delay();
    }

    bool i2c_write(uint8_t data) {
      for (uint8_t i = 0; i < 8; i++) {
        (data & 0x80) ? sda_high() : sda_low();
        data <<= 1;
        i2c_delay();
        scl_high(); i2c_delay();
        scl_low();
      }
      // Check ACK
      sda_high();
      scl_high();
      bool ack = !sda_read();
      i2c_delay();
      scl_low();
      return ack;
    }

    uint8_t i2c_read(bool ack) {
      uint8_t data = 0;
      sda_high();
      for (uint8_t i = 0; i < 8; i++) {
        scl_high();
        if (sda_read()) data |= (1 << (7 - i));
        i2c_delay();
        scl_low();
      }
      ack ? sda_low() : sda_high();
      scl_high(); i2c_delay();
      scl_low();
      return data;
    }

  public:
    PCA9555_Fast(uint8_t addr = 0x20) : _addr(addr) {}

    void init(uint16_t config = 0xFFFF) {
      // Set pins as inputs initially (high impedance)
      DDRC &= ~((1 << 4) | (1 << 5));
      PORTC &= ~((1 << 4) | (1 << 5));
      
      writeReg16(0x06, config);
    }

    void writeReg16(uint8_t reg, uint16_t val) {
      i2c_start();
      i2c_write(_addr << 1);
      i2c_write(reg);
      i2c_write(val & 0xFF);
      i2c_write(val >> 8);
      i2c_stop();
    }

    uint16_t readReg16(uint8_t reg) {
      i2c_start();
      i2c_write(_addr << 1);
      i2c_write(reg);
      i2c_start();
      i2c_write((_addr << 1) | 1);
      uint8_t low = i2c_read(true);
      uint8_t high = i2c_read(false);
      i2c_stop();
      return (high << 8) | low;
    }

    void setPin(uint8_t pin, bool state) {
      if (state) _outCache |= (1 << pin);
      else _outCache &= ~(1 << pin);
      writeReg16(0x02, _outCache);
    }

    bool getPin(uint8_t pin) {
      return (readReg16(0x00) >> pin) & 0x01;
    }
};
