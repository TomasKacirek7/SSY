/*
 * atmel.c
 *
 * Created: 3/10/2025 13:04:20
 *  Author: Student
 */ 
#include "atmel.h"
#include "I2C.h"

// Nastavení pointer registru
void at30_write_pointer(uint8_t reg) {
	i2cStart();
	i2cWrite(TEMP_SENSOR_ADDR_W);
	i2cWrite(reg);
	i2cStop();
}

// Nastavení p?esnosti m??ení teploty
uint8_t at30_set_precision(uint8_t prec) {
	uint16_t config_register = (uint16_t)(prec << 6); // R1:R0 jsou bity 6 a 7

	i2cStart();
	i2cWrite(TEMP_SENSOR_ADDR_W);
	if (i2cGetStatus() != 0x18) return 0;

	i2cWrite(TEMP_CONFIG_REGISTER);
	if (i2cGetStatus() != 0x28) return 0;

	i2cWrite((uint8_t)(config_register >> 8));
	if (i2cGetStatus() != 0x28) return 0;

	i2cWrite((uint8_t)(config_register));
	if (i2cGetStatus() != 0x28) return 0;

	i2cStop();
	return 1;
}

// ?tení teploty
float at30_read_temp(void) {
	uint8_t buffer[2];
	int16_t temp_raw;

	at30_write_pointer(TEMP_TEMP_REGISTER);

	i2cStart();
	i2cWrite(TEMP_SENSOR_ADDR_R);
	if (i2cGetStatus() != 0x40) return -1.0;

	buffer[0] = i2cReadACK();
	if (i2cGetStatus() != 0x50) return -1.0;

	buffer[1] = i2cReadNACK();
	if (i2cGetStatus() != 0x58) return -1.0;

	i2cStop();

	temp_raw = (buffer[0] << 8) | buffer[1];
	return (float)temp_raw / 256.0;
}

// Zápis do EEPROM
uint8_t at30_eeprom_write(uint16_t addr, uint8_t data) {
	i2cStart();
	i2cWrite(EEPROM_ADDR_W);
	if (i2cGetStatus() != 0x18) return 0;

	i2cWrite((uint8_t)(addr >> 8));  // Horní byte adresy
	if (i2cGetStatus() != 0x28) return 0;

	i2cWrite((uint8_t)addr);         // Dolní byte adresy
	if (i2cGetStatus() != 0x28) return 0;

	i2cWrite(data);
	if (i2cGetStatus() != 0x28) return 0;

	i2cStop();
	return 1;
}

// ?tení z EEPROM
uint8_t at30_eeprom_read(uint16_t addr, uint8_t *data) {
	i2cStart();
	i2cWrite(EEPROM_ADDR_W);
	if (i2cGetStatus() != 0x18) return 0;

	i2cWrite((uint8_t)(addr >> 8));  // Horní byte adresy
	if (i2cGetStatus() != 0x28) return 0;

	i2cWrite((uint8_t)addr);         // Dolní byte adresy
	if (i2cGetStatus() != 0x28) return 0;

	i2cStart();
	i2cWrite(EEPROM_ADDR_R);
	if (i2cGetStatus() != 0x40) return 0;

	*data = i2cReadNACK();
	if (i2cGetStatus() != 0x58) return 0;

	i2cStop();
	return 1;
}
