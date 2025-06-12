#ifndef _EEPROM_H
#define _EEPROM_H

#ifdef _EEPROM_C
   #define SCOPE_EEPROM
#else
   #define SCOPE_EEPROM extern
#endif

#include <stdint.h>
#include <string.h>
#include "stm32l4xx_hal.h"

#define CS_GPIO GPIOD
#define CS_PIN GPIO_PIN_2

#define EEPROM_START 0x01
#define EEPROM_USER 0x0C
#define EEPROM_EWEN 0x3F
#define EEPROM_WRITE 0x40
#define EEPROM_EWDS 0x0F
#define EEPROM_READ 0x80

SCOPE_EEPROM SPI_HandleTypeDef hspi3;

void EraseLeaderboard(unsigned short numScores);
void write_score(int value, char *user, unsigned short pos);

uint8_t *EEPROM_GET_MEM(unsigned short addr, unsigned short len, uint8_t *buf);
void EEPROM_MEM_HELPER(uint8_t EE_Addr);
void Save_EEPROM(uint8_t *data, unsigned short len, unsigned short addr);
void EEPROM_SEND(char EE_Addr, char EE_Data1, char EE_Data2);

#endif
