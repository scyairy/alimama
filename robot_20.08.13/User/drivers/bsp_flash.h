#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H	 
#include "sys.h" 

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
 
//FLASH �����ַ
#define FrontGrayscaleAddr  ADDR_FLASH_SECTOR_9
#define BackGrayscaleAddr   ADDR_FLASH_SECTOR_10
#define MidGrayscaleAddr    ADDR_FLASH_SECTOR_11



//FLASH ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes  

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
