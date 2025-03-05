/*!
* 文件名     main.c
* 作者       聚沃科技
* 版本       V1.0
* 实验简介   DCI-摄像头实验
*/

/*
* 版权说明
* 本程序只供学习使用，未经作者许可，不得用于其它任何用途
* GD32H757海棠派开发板V1
* 在线购买：   http://juwo.taobao.com
* 技术交流：   http://www.gd32bbs.com/ask/
* 视频学习：   https://space.bilibili.com/475462605
* 微信公众号： gd32bbs
* 创建日期:    2023/8/30
* Copyright    苏州聚沃电子科技有限公司
* 版权所有，盗版必究。
*/

#include "bsp_8080_lcd.h"
#include "bsp_led.h"
#include "bsp_uart.h"
#include "bsp_ov7670.h"

int main(void)
{     
    uint8_t ov_flag=0;
    driver_init();       /* 延时和公共驱动部分初始化 */
    
    bsp_uart_init(&BOARD_UART);     /* 打印串口初始化 */      
    
    bsp_led_group_init();           /* 初始化LED组 */  

    bsp_led_on(&LED1);
    bsp_led_off(&LED2);
        
    bsp_8080_lcd_init();                 /* 初始化LCD */
    
    //bsp_8080_lcd_show_log();    
                
    ov7670_id_struct ov7670id;  
    
    bsp_ov7670_init();
    bsp_ov7670_id_read(&ov7670id);
    
    bsp_8080_lcd_printf_init(30,240-1+140,bsp_8080_lcd_parameter.width-1,bsp_8080_lcd_parameter.height-1,FONT_ASCII_32_16,WHITE,RED);
    
//    bsp_8080_lcd_printf(" ** Camera Manufacturer_ID1 is  %d \r\n",ov7670id.Manufacturer_ID1);
//    bsp_8080_lcd_printf(" ** Camera Manufacturer_ID2 is  %d \r\n",ov7670id.Manufacturer_ID2);    
//    bsp_8080_lcd_printf(" ** Camera Version is  %d \r\n",ov7670id.Version);
//    bsp_8080_lcd_printf(" ** Camera PID is %d \r\n",ov7670id.PID);
    bsp_8080_lcd_printf("Camera Experience\r\n");
    delay_ms(1500);
    
     
    while (1)
    {   
        ov_flag=dci_check_ready();        
        if(ov_flag>=3)
        {
            bsp_led_toggle(&LED1);
            bsp_led_toggle(&LED2);            
            bsp_8080_lcd_color_fill(0,100,320-1,240-1+100,(uint16_t*)camera_buff);      //显示摄像头读取数据
			
            if(ov_flag!=3){
                while(dci_check_ready()!=3);            
            }
            
            dci_restart(); 
            
            //bsp_8080_lcd_printf(" ** Camera refresh tic is  %lld \r\n",driver_tick);
        }    
    }
}

