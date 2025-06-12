#include "keypad.h"

typedef struct
{
   unsigned short sKeyRead;
   unsigned short sKeyReadTempPos;
   unsigned short sKeySend;
   unsigned short sKeyCol;
   char KeyLetter;
   unsigned short sKeyCommand;
} Key_Contorl_struct_t;

typedef enum KeyName
{
  ONE_command,
  FOUR_command,
  SEVEN_command,
  STAR_command,
  TWO_command,
  FIVE_command,
  EIGHT_command,
  ZERO_command,
  THREE_command,
  SIX_command,
  NINE_command,
  POUND_command
} KeyName;

static Key_Contorl_struct_t sKeyControl[Number_of_Keys]
={
 {PA10,0x8,PA4,0,'1',ONE_command},     // PA10 (read), PA4 (send)
 {PC0,0x4,PA4,0,'4',FOUR_command},     // PC0 (read), PA4 (send)
 {PC1,0x2,PA4,0,'7',SEVEN_command},    // PC1 (read), PA4 (send)
 {PB0,0x1,PA4,0,'*',STAR_command},     // PB0 (read), PA4 (send)

 {PA10,0x8,PA1,1,'2',TWO_command},     // PA10 (read), PA1 (send)
 {PC0,0x4,PA1,1,'5',FIVE_command},     // PC0 (read), PA1 (send)
 {PC1,0x2,PA1,1,'8',EIGHT_command},    // PC1 (read), PA1 (send)
 {PB0,0x1,PA1,1,'0',ZERO_command},     // PB0 (read), PA1 (send)

 {PA10,0x8,PA0,2,'3',THREE_command},   // PA10 (read), PA0 (send)
 {PC0,0x4,PA0,2,'6',SIX_command},      // PC0 (read), PA0 (send)
 {PC1,0x2,PA0,2,'9',NINE_command},     // PC1 (read), PA0 (send)
 {PB0,0x1,PA0,2,'#',POUND_command}     // PB0 (read), PA0 (send)
};

static unsigned short sKeyStatus;
static unsigned short sKeyCurrentCol[Number_of_Cols];
static unsigned short sKeyDebouncedCol[Number_of_Cols];
static unsigned short sKeyIssued;

static unsigned short sKeyPreviousCol[Number_of_Cols];
static unsigned short sKeyLow2HighCol[Number_of_Cols];

char scanKeypad()
{
    unsigned short sIndex;
    unsigned short Temp;

    // Clear all key records
    for (sIndex=0; sIndex<Number_of_Cols; sIndex++)
    {
      sKeyCurrentCol[sIndex] = 0x00;
    }

    // Read all 3 column
    for (sIndex=0; sIndex<Number_of_Keys; sIndex++)
    {
      GPIOA->ODR &=~(PA4 | PA1 | PA0);
      GPIOA->ODR |= sKeyControl[sIndex].sKeySend;
      HAL_Delay(0.5);

      switch (sKeyControl[sIndex].sKeyCommand)
	  {
      	  case ONE_command:
      	  case TWO_command:
      	  case THREE_command:
      		if (GPIOA->IDR & sKeyControl[sIndex].sKeyRead)
      		  sKeyCurrentCol[sKeyControl[sIndex].sKeyCol]= sKeyControl[sIndex].sKeyReadTempPos;
      		break;

      	  case FOUR_command:
      	  case FIVE_command:
      	  case SIX_command:
      	  case SEVEN_command:
      	  case EIGHT_command:
      	  case NINE_command:
        	if (GPIOC->IDR & sKeyControl[sIndex].sKeyRead)
        	  sKeyCurrentCol[sKeyControl[sIndex].sKeyCol] = sKeyControl[sIndex].sKeyReadTempPos;
      	    break;

      	  case STAR_command:
      	  case ZERO_command:
      	  case POUND_command:
      		if (GPIOB->IDR & sKeyControl[sIndex].sKeyRead)
      		  sKeyCurrentCol[sKeyControl[sIndex].sKeyCol] = sKeyControl[sIndex].sKeyReadTempPos;
	  }
    }

    // Check if a key is steadily read
    for (sIndex=0; sIndex<Number_of_Cols; sIndex++)
    {
      if ((sKeyCurrentCol[sIndex] == sKeyDebouncedCol[sIndex]) && (sKeyCurrentCol[sIndex] != 0x0000))
        break;
    }

    if (sIndex < Number_of_Cols)
    {
    	// Check for push on/ push off (Low To High)
    	for (sIndex=0; sIndex<Number_of_Cols; sIndex++)
    	{
    		Temp = sKeyCurrentCol[sIndex] ^ sKeyPreviousCol[sIndex];
    		sKeyLow2HighCol[sIndex] = (sKeyCurrentCol[sIndex] & Temp);
    	}

    	// Update Previous records
    	for (sIndex=0; sIndex<Number_of_Cols; sIndex++)
    	{
    	    sKeyPreviousCol[sIndex] = sKeyCurrentCol[sIndex];
    	}

      // Find which key is JUST depressed (Low To High) // or KeyRepeat detected
       for (sIndex=0 ; sIndex<Number_of_Keys; sIndex++)
       {
         if (sKeyLow2HighCol[sKeyControl[sIndex].sKeyCol] & sKeyControl[sIndex].sKeyReadTempPos)
         {
           sKeyIssued = sKeyControl[sIndex].sKeyCommand;
           sKeyStatus |= (KeyLow2High);
           break;
         }
         else
         sKeyIssued = 0xFFFF;
       }
    }
    else
    {
      sKeyStatus &= ~(KeyLow2High);

      for (sIndex=0; sIndex<Number_of_Cols; sIndex++)
       	  sKeyPreviousCol[sIndex] = 0;
    }


    // Transfer Current reading to debounced reading
    for (sIndex=0; sIndex<Number_of_Cols; sIndex++)
    {
      sKeyDebouncedCol[sIndex] = sKeyCurrentCol[sIndex];
      sKeyLow2HighCol[sIndex] = 0;
    }

    return processKey();
}


char processKey()
{
	uint16_t sIndex;

	if ((sKeyStatus & KeyLow2High))
	{
		char pressed = sKeyControl[sKeyIssued].KeyLetter;

		sKeyStatus &= ~(KeyLow2High);

		// Clear all Low-2-High and High-2-Low records
		for (sIndex=0; sIndex<Number_of_Cols; sIndex++)
		  sKeyLow2HighCol[sIndex] = 0x0000;

		return pressed;
	}

	else {
		return 0;
	}
}
