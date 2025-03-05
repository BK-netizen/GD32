/*!
 * �ļ����ƣ�  bsp_lcd_font.h
 * ��    ����  lcd�ֿ�
 * �汾��      2023-12-03, V1.0
*/

/*
* GD32H757�����ɿ�����V1.0
* �Ա��̳ǣ�   https://juwo.taobao.com
* ����������   https://www.gd32bbs.com/ask/
* ��Ƶѧϰ��   https://space.bilibili.com/475462605
* ΢�Ź��ںţ� gd32bbs
* Copyright    ���ݾ��ֵ��ӿƼ����޹�˾
* ��Ȩ���У�����ؾ���
*/

#ifndef __BSP_LCD_FONT_H
#define __BSP_LCD_FONT_H

/* �����Сö�� */
typedef enum {FONT_ASCII_12_6 = 12,FONT_ASCII_16_8=16,FONT_ASCII_24_12=24,FONT_ASCII_32_16=32} FONT_ASCII;

/* 12*6 ASCII�ַ������� */
extern const unsigned char ascii_12_6[95][12];

/* 16*8 ASCII�ַ������� */
extern const unsigned char ascii_16_8[95][16];;

/* 24*12 ASICII�ַ������� */
extern const unsigned char ascii_24_12[95][36];

/* 32*16 ASCII�ַ������� */
extern const unsigned char ascii_32_16[95][64];



#endif





















