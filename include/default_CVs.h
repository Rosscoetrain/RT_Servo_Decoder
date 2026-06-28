/*
 * @file default_CVs.h
 */

#ifndef DEFAULT_CVS_H
#define DEFAULT_CVS_H

#include <Arduino.h>
#include <avr/pgmspace.h>

// 4 header items + 16 blocks of 5 servo items
// + 16 blocks of 5 servo items = 164 items
//constexpr size_t NUM_TOTAL_CVS = 4 + (16 * 5);
// defined in defines.h

// struct wrapper to hold a raw C-style array
struct CVArrayContainer {
    CVPair data[NUM_TOTAL_CVS];
};

// compile-time generator function returning the custom container struct
constexpr CVArrayContainer GenerateFactoryDefaultCVs() {
    CVArrayContainer container{};

    size_t index = 0;

    // Static headers
    container.data[index] = {CV_ACCESSORY_DECODER_ADDRESS_LSB, static_cast<uint16_t>(DEFAULT_ACCESSORY_DECODER_ADDRESS & 0xFF)};
    container.data[index++] = {CV_ACCESSORY_DECODER_ADDRESS_MSB, static_cast<uint16_t>(DEFAULT_ACCESSORY_DECODER_ADDRESS >> 8)};
    container.data[index++] = {CV_ACCESSORY_DECODER_SERVO_MOVE_TIME, SERVO_TIME};
    container.data[index++] = {CV_29_CONFIG, CV29_ACCESSORY_DECODER};

    // servo sections
    for (int i = 0; i < NUM_TURNOUTS; ++i) {
        int base = CV_USER_BASE_ADDRESS + (i * 5);
        container.data[index++] = {static_cast<uint16_t>(base + 0), SERVO_MIN};
        container.data[index++] = {static_cast<uint16_t>(base + 1), SERVO_MAX};
        container.data[index++] = {static_cast<uint16_t>(base + 2), SERVO_TIME};
        container.data[index++] = {static_cast<uint16_t>(base + 3), SERVO_CONFIG};
        container.data[index++] = {static_cast<uint16_t>(base + 4), SERVO_MIN};
    }

    for (int i = 0; i < NUM_TURNOUTS; ++i) {
        int base = CV_USER_BASE_ADDRESS_2 + (i * 5);
        container.data[index++] = {static_cast<uint16_t>(base + 0), SERVO_MIN};
        container.data[index++] = {static_cast<uint16_t>(base + 1), SERVO_MAX};
        container.data[index++] = {static_cast<uint16_t>(base + 2), SERVO_TIME};
        container.data[index++] = {static_cast<uint16_t>(base + 3), SERVO_CONFIG};
        container.data[index++] = {static_cast<uint16_t>(base + 4), SERVO_MIN};
    }

    container.data[index++] = {CV_ACCESSORY_DECODER_SERIAL_LSB, uint8_t(SERIAL_NUMBER - (SERIAL_NUMBER * 256))};
    container.data[index++] = {CV_ACCESSORY_DECODER_SERIAL_MSB, uint8_t(SERIAL_NUMBER / 256)};

    return container;
}

// Force compile-time calculation and store the array explicitly in FLASH memory
const CVArrayContainer FactoryDefaultContainer PROGMEM = GenerateFactoryDefaultCVs();

// Expose an easier reference alias to mimic your original array layout
const CVPair* const FactoryDefaultCVs = FactoryDefaultContainer.data;



#endif
