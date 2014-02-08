#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <stdint.h>

typedef struct {
	uint8_t manufacturer;
	uint8_t memory_type;
	uint8_t memory_density;
} eeprom_identification_t;

void eeprom_init(uint32_t spi_base, uint32_t eeprom_cs_gpio_port, uint8_t eeprom_cs_gpio_pin, uint16_t page_size);
void eeprom_start_command();
void eeprom_end_command();
void eeprom_read_identification(eeprom_identification_t* dest);
uint8_t eeprom_read_status();
void eeprom_write_enable();
void eeprom_write_disable();
void eeprom_read(uint32_t start_address, uint32_t count, uint8_t* buffer);
void eeprom_erase();
void eeprom_write_page(uint32_t start_address, uint32_t count, uint8_t* buffer);

#endif
