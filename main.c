/*!
* �ļ���     main.c
* ����       ���ֿƼ�
* �汾       V1.0
* ʵ����   DCI-����ͷʵ��
*/

/*
* ��Ȩ˵��
* ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* GD32H757�����ɿ�����V1
* ���߹���   http://juwo.taobao.com
* ����������   http://www.gd32bbs.com/ask/
* ��Ƶѧϰ��   https://space.bilibili.com/475462605
* ΢�Ź��ںţ� gd32bbs
* ��������:    2023/8/30
* Copyright    ���ݾ��ֵ��ӿƼ����޹�˾
* ��Ȩ���У�����ؾ���
*/

#include "bsp_8080_lcd.h"
#include "bsp_led.h"
#include "bsp_uart.h"
#include "bsp_ov7670.h"

int main(void)
{     
    uint8_t ov_flag=0;
    driver_init();       /* ��ʱ�͹����������ֳ�ʼ�� */
    
    bsp_uart_init(&BOARD_UART);     /* ��ӡ���ڳ�ʼ�� */      
    
    bsp_led_group_init();           /* ��ʼ��LED�� */  

    bsp_led_on(&LED1);
    bsp_led_off(&LED2);
        
    bsp_8080_lcd_init();                 /* ��ʼ��LCD */
    
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
            bsp_8080_lcd_color_fill(0,100,320-1,240-1+100,(uint16_t*)camera_buff);      //��ʾ����ͷ��ȡ����
			
            if(ov_flag!=3){
                while(dci_check_ready()!=3);            
            }
            
            dci_restart(); 
            
            //bsp_8080_lcd_printf(" ** Camera refresh tic is  %lld \r\n",driver_tick);
        }    
    }
}

