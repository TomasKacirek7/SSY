/*
 * I2C.h
 *
 * Created: 3/10/2025 12:48:34
 *  Author: Student
 */

#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

// Function to initialize the I2C
static inline void i2cInit(void) {
    // Set SCL to 400kHz
    TWSR = 0x00;
    TWBR = 0x02;
    // Enable I2C
    TWCR = (1 << TWEN);
}

// Function to start I2C communication
static inline void i2cStart(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));  // Wait for transmission to complete
}

// Function to stop I2C communication
static inline void i2cStop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

// Function to write a byte to the I2C bus
static inline void i2cWrite(uint8_t byte) {
    TWDR = byte;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));  // Wait for transmission to complete
}

// Function to read a byte from the I2C bus with ACK
static inline uint8_t i2cReadACK(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));  // Wait for reception to complete
    return TWDR;
}

// Function to read a byte from the I2C bus without ACK
static inline uint8_t i2cReadNACK(void) {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));  // Wait for reception to complete
    return TWDR;
}

// Function to get the I2C status
static inline uint8_t i2cGetStatus(void) {
    return TWSR & 0xF8;  // Return the status bits
}

#endif /* I2C_H */
