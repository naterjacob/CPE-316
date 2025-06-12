#define _EEPROM_C

#include "eeprom.h"
#include "timer.h"

static uint8_t EE_Data[3];

void EraseLeaderboard(unsigned short numScores) {
	for (int i = 0; i < numScores; i++) {
		write_score(0, '\0', i);
	}
}

void write_score(int value, char *user, unsigned short pos) {
	// int is 4 bytes
	// name(initials) are 4 bytes
	Save_EEPROM((uint8_t *) &value, 4, EEPROM_USER + (8 * pos));
	Save_EEPROM((uint8_t *) user, 4, EEPROM_USER + (8 * pos) + 4);
}

uint8_t *EEPROM_GET_MEM(unsigned short addr, unsigned short len, uint8_t *buf)
{
	// len must be an even number!!
	uint8_t temp[len];
	uint8_t i;

	// Read two bytes. Check separately until null is found
	for (i=0; i<(len/2); i++)
	{
	  EEPROM_MEM_HELPER(addr + (2 * i));		// read two bytes

	  temp[2*i] = EE_Data[0];
	  temp[2*i+1] = EE_Data[1];
	}

	memcpy(buf, temp, len);
	return buf;
}

void EEPROM_MEM_HELPER(uint8_t EE_Addr)
{
    uint8_t buf[2] = {0};

    HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_SET);         // Chip Select Enable
    buf[0] = EEPROM_START;
    buf[1] = (EEPROM_READ | (EE_Addr & 0x3F));
    HAL_SPI_Transmit(&hspi3, buf, 2, 100);

    HAL_SPI_Receive(&hspi3, EE_Data, 3, 100);
    HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_RESET);           // Chip Select disable
    Delay_1_plus_us();

    EE_Data[0] <<= 1;
    if (EE_Data[1] & 0x80)
    	EE_Data[0] |= 0x01;

    EE_Data[1] <<= 1;
        if (EE_Data[2] & 0x80)
        EE_Data[1] |= 0x01;
}

void Save_EEPROM(uint8_t *data, unsigned short len, unsigned short addr)
{
  uint8_t temp0, temp1;

  for (int j = 0; j < (len / 2); j++)
  {
    temp0 = data[2 * j];
    temp1 = data[2 * j + 1];

	EEPROM_SEND(addr, temp0, temp1);
	addr += 2;
  }
}

void EEPROM_SEND(char EE_Addr, char EE_Data1, char EE_Data2)
{
    uint8_t buf[4];

    HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_SET);         // Chip Select Enable
    buf[0] = EEPROM_START;
    buf[1] = EEPROM_EWEN; //EEPROM_EWEN;
    HAL_SPI_Transmit(&hspi3, buf, 2, 100);
    HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_RESET);       // Chip Select disable

    Delay_1_plus_us();

    HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_SET);         // Chip Select Enable
    buf[0] = EEPROM_START;
    buf[1] = (0x40 | (EE_Addr & 0x03F));        // WRITE command OR with 6-bit address
    buf[2] = EE_Data1;
    buf[3] = EE_Data2;
    HAL_SPI_Transmit(&hspi3, buf, 4, 100);
    HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_RESET);       // Chip Select disable

    HAL_Delay(20);

    HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_SET);         // Chip Select Enable
    buf[0] = EEPROM_START;
    buf[1] = EEPROM_EWDS;
    HAL_SPI_Transmit(&hspi3, buf, 2, 100);
    HAL_GPIO_WritePin(CS_GPIO, CS_PIN, GPIO_PIN_RESET);           // Chip Select disable

    Delay_1_plus_us();
}


