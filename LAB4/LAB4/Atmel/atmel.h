/*
 * atmel.h
 *
 * Created: 3/10/2025 12:56:36
 *  Author: Student
 */ 

#ifndef ATMEL_H_
#define ATMEL_H_

#include <stdint.h>

// I2C addresses for sensors and EEPROM
#define TEMP_SENSOR_ADDR_W  0b10010110  // Write address for temperature sensor
#define TEMP_SENSOR_ADDR_R  0b10010111  // Read address for temperature sensor
#define EEPROM_ADDR_W       0b10100110  // Write address for EEPROM
#define EEPROM_ADDR_R       0b10100111  // Read address for EEPROM

// Registers
#define TEMP_CONFIG_REGISTER  0x01  // Temperature sensor configuration register
#define TEMP_TEMP_REGISTER    0x00  // Temperature register
#define EEPROM_POINTER_REG    0x02  // EEPROM pointer register

// Function prototypes
void at30_write_pointer(uint8_t reg);
uint8_t at30_set_precision(uint8_t prec);
float at30_read_temp(void);
uint8_t at30_eeprom_write(uint16_t addr, uint8_t data);
uint8_t at30_eeprom_read(uint16_t addr, uint8_t *data);

#endif /* ATMEL_H_ */
