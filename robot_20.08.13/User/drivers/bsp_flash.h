#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H	 
#include "sys.h" 

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
 
//FLASH 储存地址
#define FrontGrayscaleAddr  ADDR_FLASH_SECTOR_9
#define BackGrayscaleAddr   ADDR_FLASH_SECTOR_10
#define MidGrayscaleAddr    ADDR_FLASH_SECTOR_11



//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//扇区7起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//扇区11起始地址,128 Kbytes  

enum SystemparamType
{
   F_Grayscale	= 1,
   B_Grayscale,
	 M_Grayscale
};

uint16_t Flash_GetSector(uint32_t Address);
void Flash_EraseSector(uint16_t SectorNum);

void Flash_Write32BitDatas(uint32_t address, uint16_t length, int32_t* data_32);
void Flash_Read32BitDatas(uint32_t address, uint16_t length, int32_t* data_32);

void Flash_Write16BitDatas(uint32_t address, uint16_t length, int16_t* data_16);
void Flash_Read16BitDatas(uint32_t address, uint16_t length, int16_t* data_16);

void Flash_Write8BitDatas(uint32_t address, uint16_t length, int8_t* data_8);
void Flash_Read8BitDatas(uint32_t address, uint16_t length, int8_t* data_8);
void SaveSystemParam(uint8_t type);


#endif
