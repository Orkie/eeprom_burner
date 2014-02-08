/*
Copyright 2014 Adan Scotney

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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
