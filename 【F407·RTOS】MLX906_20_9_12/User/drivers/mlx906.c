//2020.9.12
#include "rtthread.h"
#include "sys.h"
#include "mlx906.h"
#include "rt_oled.h"
#include "rt_led.h"
#include "delay.h"

ALIGN(RT_ALIGN_SIZE)

static struct rt_thread fan_thread;
static struct rt_thread detach_thread;

static rt_uint8_t fan_stack[1024];
static rt_uint8_t detach_stack[1024];
static rt_thread_t tid1 = RT_NULL;

#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5


void mlx906_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
//	
	SDA_H;
	SCL_H; 
}



void SMBus_StartBit(void)
{
	 SDA_H; 
   delay_us(5); 
   SCL_H; 
   delay_us(5); 
   SDA_L; 
   delay_us(5); 
   SCL_L; 
   delay_us(5);
}



void SMBus_StopBit(void)
{
	 SCL_L; 
   delay_us(5); 
   SDA_L; 
   delay_us(5); 
   SCL_H; 
   delay_us(5); 
   SDA_H; 
   delay_us(5); 
}



u8 SMBus_SendByte(u8 Tx_buffer)
{
    u8        Bit_counter;
    u8        Ack_bit;
    u8        bit_out;


    for(Bit_counter=8; Bit_counter; Bit_counter--)
    {
        if (Tx_buffer&0x80)
        {
            bit_out=1;       // If the current bit of Tx_buffer is 1 set bit_out
        }
        else
        {
            bit_out=0;      // else clear bit_out
        }
        SMBus_SendBit(bit_out);           // Send the current bit on SDA
        Tx_buffer<<=1;                    // Get next bit for checking
    }
    Ack_bit=SMBus_ReceiveBit();           // Get acknowledgment bit
    return        Ack_bit;

}



void SMBus_SendBit(u8 bit_out)
{
    if(bit_out==0)
    {
      SDA_L;   
    }
    else
    {
    SDA_H;
    }
    delay_us(5);                            // Tsu:dat = 250ns minimum
    SCL_H;                                     // Set SCK line
    delay_us(5);                           // High Level of Clock Pulse
    SCL_L;                                     // Clear SCK line
   delay_us(5);                           // Low Level of Clock Pulse
//        SMBUS_SDA_H();                       // Master release SDA line ,
    return;
}



u8 SMBus_ReceiveBit()
{
    u8 Ack_bit;


    SDA_H;             //引脚靠外部电阻上拉，当作输入
    SCL_H;             // Set SCL line
    delay_us(5);    // High Level of Clock Pulse
    if (SMBUS_SDA_PIN)
    {
        Ack_bit=1;
    }
    else
    {
        Ack_bit=0;
    }
    SCL_L;                    // Clear SCL line
    delay_us(5);           // Low Level of Clock Pulse
    return   Ack_bit;
}



u8 SMBus_ReceiveByte(u8 ack_nack)
{
    u8 	RX_buffer;
    u8	Bit_Counter;
    for(Bit_Counter=8; Bit_Counter; Bit_Counter--)
    {
        if(SMBus_ReceiveBit())// Get a bit from the SDA line
        {
            RX_buffer <<= 1;// If the bit is HIGH save 1  in RX_buffer
            RX_buffer |=0x01;//如果Ack_bit=1，把收到应答信号1与0000 0001 进行或运算，确保为1
        }
        else
        {
            RX_buffer <<= 1;// If the bit is LOW save 0 in RX_buffer
            RX_buffer &=0xfe;//如果Ack_bit=1，把收到应答信号0与1111 1110 进行与运算，确保为0
        }
    }
    SMBus_SendBit(ack_nack);//把应答信号发出去，如果0，就进行下一次通信，如果为1，就拜拜了。
    return RX_buffer;
}



void SMBus_Delay(u16 time)
{
    u16 i, j;
    for (i=0; i<4; i++)
    {
        for (j=0; j<time; j++);
    }
}


u16 SMBus_ReadMemory(u8 slaveAddress, u8 command)
{
    u16 data;               // Data storage (DataH:DataL)
    u8 Pec;                 // PEC byte storage
    u8 DataL=0;             // Low data byte storage
    u8 DataH=0;             // High data byte storage
    u8 arr[6];              // Buffer for the sent bytes
    u8 PecReg;              // Calculated PEC byte storage
    u8 ErrorCounter;        // Defines the number of the attempts for communication with MLX90614


    ErrorCounter=0x00;                                // Initialising of ErrorCounter
        slaveAddress <<= 1;        //2-7位表示从机地址

    do
    {
repeat:
        SMBus_StopBit();                //If slave send NACK stop comunication
        --ErrorCounter;                 //Pre-decrement ErrorCounter
        if(!ErrorCounter)               //ErrorCounter=0?
        {
            break;                      //Yes,go out from do-while{}
        }

        SMBus_StartBit();               //Start condition
        if(SMBus_SendByte(slaveAddress))//Send SlaveAddress 最低位Wr=0表示接下来写命令
        {
            goto  repeat;               //Repeat comunication again
        }
        if(SMBus_SendByte(command))     //Send command
        {
            goto    repeat;             //Repeat comunication again
        }

        SMBus_StartBit();                //Repeated Start condition
        if(SMBus_SendByte(slaveAddress+1))  //Send SlaveAddress 最低位Rd=1表示接下来读数据
        {
            goto        repeat;           //Repeat comunication again
        }

        DataL = SMBus_ReceiveByte(ACK);   //Read low data,master must send ACK
        DataH = SMBus_ReceiveByte(ACK);   //Read high data,master must send ACK
        Pec = SMBus_ReceiveByte(NACK);    //Read PEC byte, master must send NACK
        SMBus_StopBit();                  //Stop condition

        arr[5] = slaveAddress;        
        arr[4] = command;
        arr[3] = slaveAddress+1;         //Load array arr
        arr[2] = DataL;                 
        arr[1] = DataH;                
        arr[0] = 0;                   
        PecReg=PEC_Calculation(arr);     //Calculate CRC
    }
    while(PecReg != Pec);                //If received and calculated CRC are equal go out from do-while{}
        data = (DataH<<8) | DataL;       //data=DataH:DataL
    return data;
}



u8 PEC_Calculation(u8 pec[])
{
    u8         crc[6];
    u8        BitPosition=47;
    u8        shift;
    u8        i;
    u8        j;
    u8        temp;


    do
    {
        /*Load pattern value 0x000000000107*/
        crc[5]=0;
        crc[4]=0;
        crc[3]=0;
        crc[2]=0;
        crc[1]=0x01;
        crc[0]=0x07;
        /*Set maximum bit position at 47 ( six bytes byte5...byte0,MSbit=47)*/
        BitPosition=47;
        /*Set shift position at 0*/
        shift=0;
        /*Find first "1" in the transmited message beginning from the MSByte byte5*/
        i=5;
        j=0;
        while((pec[i]&(0x80>>j))==0 && i>0)
        {
            BitPosition--;
            if(j<7)
            {
                j++;
            }
            else
            {
                j=0x00;
                i--;
            }
        }/*End of while */


        /*Get shift value for pattern value*/
        shift=BitPosition-8;
        /*Shift pattern value */
        while(shift)
        {
            for(i=5; i<0xFF; i--)
            {
                if((crc[i-1]&0x80) && (i>0))
                {
                    temp=1;
                }
                else
                {
                    temp=0;
                }
                crc[i]<<=1;
                crc[i]+=temp;
            }/*End of for*/
            shift--;
        }/*End of while*/
        /*Exclusive OR between pec and crc*/
        for(i=0; i<=5; i++)
        {
            pec[i] ^=crc[i];
        }/*End of for*/
    }
    while(BitPosition>8); /*End of do-while*/

    return pec[0];
}



float SMBus_ReadTemp(void)
{   
    return (SMBus_ReadMemory(0x00, 0x07)*0.02-273.15)*100;
}



static void mlx906_entry(void *parameter)
{
	float tmp;
		int tempp,tem_xiaoshu,tem_zhengshu;
	while(1)
	{
		tmp=SMBus_ReadTemp();
		oled.show_number(48,1,tmp,4,14);
		delay_ms(100);
		
	}
}
	



int mlx906(void)
{
	tid1=rt_thread_create("mlx906",
												mlx906_entry,RT_NULL,
												THREAD_STACK_SIZE,
												15,THREAD_TIMESLICE);
	if(tid1!=RT_NULL)
		rt_thread_startup(tid1);
	
	return 0;
}
	
	
	
	
	
	




