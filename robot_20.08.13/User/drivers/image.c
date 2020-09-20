#include "image.h"
#include "app_uart.h"
#include "bsp_led.h"
#include "bsp_oled.h"

rt_uint8_t image_refresh,image_result;


void image_thread_entry(void* parameter)
{
    rt_uint8_t uart_rx_data[3],temp;
    if (uart_open("uart2") != RT_EOK)
    {
        rt_kprintf("uart open error.\n");
         while (1)
         {
					 rt_thread_delay(100);
         }
    }
    while(1)
    {
        uart_rx_data[0]  = uart_getchar();
        uart_rx_data[1]  = uart_getchar();
        uart_rx_data[2]  = uart_getchar();
        image_result=uart_rx_data[0];
        image_refresh=1;
//			rt_thread_delay(1);
    }
}


rt_uint8_t get_image(char platform)
{


	 uart_putchar(platform);
    while(1)
    {
        if(image_refresh)
        {
            image_refresh=0;
            break;
        }
    }
		rt_thread_delay(1);

		switch(platform)
		{
			case 97:
				if(image_result==48)	image_result=3;
				else if(image_result==49) image_result=4;
				break;
			case 98:
				if(image_result==48)	image_result=5;
				else if(image_result==49) image_result=6;
			case 99:
				if(image_result==48)	image_result=5;
				else if(image_result==49) image_result=6;
			case 100:
				if(image_result==48)image_result=7;
				else if(image_result==49) image_result=8;
			case 101:
				if(image_result==48)	image_result=7;
				else if(image_result==49)image_result=8;
		}
		rt_kprintf("result:%d\n",image_result);
//		OLED_ShowNum(0,20,platform,4,12);
		OLED_WR_Byte(0xA1,OLED_CMD);
		OLED_ShowNum(20,0,image_result,4,12);
    return image_result;
}


void image_thread_creat(void)
{
    rt_thread_t tid;
    // 创建compass线程 
    tid = rt_thread_create("compass",
                    image_thread_entry, 
                    RT_NULL,
                    512, 
                    10,
                    6);
    // 创建成功则启动线程 
    if (tid != RT_NULL)
        rt_thread_startup(tid);
}