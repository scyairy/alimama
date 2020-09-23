#include "rtthread.h"
#include "sys.h"

#define ACK           0
#define NACK          1
#define SA            0x00 //Slave address 单个MLX90614时地址为0x00,多个时地址默认为0x5a
#define RAM_ACCESS    0x00 //RAM access command
#define EEPROM_ACCESS 0x20 //EEPROM access command
#define RAM_TOBJ1     0x07 //To1 address in the eeprom
#define SDA_L     GPIO_ResetBits(GPIOD, GPIO_Pin_12)
#define SDA_H     GPIO_SetBits(GPIOD, GPIO_Pin_12)  
#define SCL_H     GPIO_SetBits(GPIOD, GPIO_Pin_13)
#define SCL_L     GPIO_ResetBits(GPIOD, GPIO_Pin_13)

#define SMBUS_SDA_PIN    GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12) //读取引脚电平
void mlx906_init(void);
void SMBus_StartBit(void);
void SMBus_StopBit(void);
void SMBus_SendBit(u8);
u8 SMBus_SendByte(u8);
u8 SMBus_ReceiveBit(void);
u8 SMBus_ReceiveByte(u8);
void SMBus_Delay(u16);
void SMBus_Init(void);
u16 SMBus_ReadMemory(u8, u8);
u8 PEC_Calculation(u8*);
float SMBus_ReadTemp(void);


int mlx906(void);   //测温线程
static void mlx906_entry(void *parameter);


