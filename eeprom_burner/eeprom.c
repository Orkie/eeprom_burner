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

#include <stdint.h>
#include <stdbool.h>
#include <hw_types.h>
#include <hw_memmap.h>
#include <sysctl.h>
#include <gpio.h>
#include <ssi.h>
#include <pin_map.h>
#include "eeprom.h"

// spi
void spi_wait_transmit_complete();
uint8_t spi_get();
uint8_t spi_recv_byte();
void spi_send_byte(uint8_t byte);

// EEPROM
static uint32_t __eeprom_cs_gpio_port;
static uint8_t __eeprom_cs_gpio_pin;
static uint32_t __spi_base;
static uint16_t __page_size;

// eeprom operations
#define WRSR	0x01 // write status register
#define PP		0x02 // page program
#define READ	0x03 // read
#define WRDI	0x04 // write disable
#define RDSR	0x05 // read status register
#define WREN	0x06 // write enable
#define CE		0x60 // chip erase
#define REMS	0x90 // read electronic manufacturer and device id sequence
#define RDID 	0x9F // read identification

void spi_wait_transmit_complete() {
	while(SSIBusy(__spi_base));
}

uint8_t spi_get() {
	uint32_t dest = 0x00;
	SSIDataGet(__spi_base, &dest);
	return (0xFF&dest);
}

uint8_t spi_recv_byte() {
	SSIDataPut(__spi_base, 0xFF);
	spi_wait_transmit_complete();
    return spi_get();
}

void spi_send_byte(uint8_t byte) {
	SSIDataPut(__spi_base, byte);
	spi_wait_transmit_complete();
}

// setup
void eeprom_init(uint32_t spi_base, uint32_t eeprom_cs_gpio_port, uint8_t eeprom_cs_gpio_pin, uint16_t page_size) {
	__spi_base = spi_base;
	__eeprom_cs_gpio_port = eeprom_cs_gpio_port;
	__eeprom_cs_gpio_pin = eeprom_cs_gpio_pin;
	__page_size = page_size;
}

// program logic
void eeprom_start_command() {
	GPIOPinWrite(__eeprom_cs_gpio_port, __eeprom_cs_gpio_pin, 0);
}

void eeprom_end_command() {
	GPIOPinWrite(__eeprom_cs_gpio_port, __eeprom_cs_gpio_pin, __eeprom_cs_gpio_pin);
}

void eeprom_read_identification(eeprom_identification_t* dest) {
	eeprom_start_command();

	spi_send_byte(RDID); spi_get();
	dest->manufacturer = spi_recv_byte();
	dest->memory_type = spi_recv_byte();
	dest->memory_density = spi_recv_byte();

	eeprom_end_command();
}

uint8_t eeprom_read_status() {
	eeprom_start_command();

	spi_send_byte(RDSR); spi_get();
	uint8_t data = spi_recv_byte();

	eeprom_end_command();
	return data;
}

void eeprom_write_enable() {
	eeprom_start_command();

	spi_send_byte(WREN); spi_get();

	eeprom_end_command();
}

void eeprom_write_disable() {
	eeprom_start_command();

	spi_send_byte(WRDI); spi_get();

	eeprom_end_command();

}

// note-start address is 24 bits
void eeprom_read(uint32_t start_address, uint32_t count, uint8_t* buffer) {
	eeprom_start_command();

	spi_send_byte(READ); spi_get();
	spi_send_byte((start_address>>16)&0xFF); spi_get();
	spi_send_byte((start_address>>8)&0xFF); spi_get();
	spi_send_byte(start_address&0xFF); spi_get();

	int i;
	for(i = 0 ; i < count ; i++) {
		buffer[i] = spi_recv_byte();
	}

	eeprom_end_command();
}

void eeprom_erase() {
	eeprom_start_command();

	spi_send_byte(CE); spi_get();

	eeprom_end_command();
}

// note-count must be 256 or less
void eeprom_write_page(uint32_t start_address, uint32_t count, uint8_t* buffer) {
	eeprom_start_command();

	spi_send_byte(PP); spi_get();
	spi_send_byte((start_address>>16)&0xFF); spi_get();
	spi_send_byte((start_address>>8)&0xFF); spi_get();
	spi_send_byte(start_address&0xFF); spi_get();
	int i;
	for(i = 0 ; i < __page_size ; i++) {
		spi_send_byte(i < count ? buffer[i] : 0xFF); spi_get();
	}

	eeprom_end_command();
}
