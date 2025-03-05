/*!
 * �ļ����ƣ� bsp_oled.h
 * ��    ���� OLED�����ļ�
 * �汾��     2023-12-03, V1.0
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

#ifndef BSP_OLED_H
#define BSP_OLED_H

#include "driver_gpio.h"

#define MAX_ROW                     (64)
#define MAX_COLUMN                  (128)
#define MAX_PAGE                    (MAX_ROW/8)
#define BRIGHTNESS                  (0xff)
#define OLED_ADDR 0x78

/* ��������ö�� */
typedef enum {
    FONT_6_8 = 0,
    FONT_8_16
}oled_font;

/* ���� ����ö�� */
typedef enum {
    OLED_CMD = 0,
    OLED_DATA
}oled_cd_selection;

Drv_Err bsp_oled_init(void);
void bsp_oled_logo(uint8_t x, uint8_t y, uint8_t x_res, uint8_t y_res, uint8_t *pbuff);
void bsp_oled_clear(uint8_t start_page);
void bsp_oled_showchar(uint8_t x, uint8_t y, uint8_t ch, oled_font size);
void bsp_oled_shownum(uint8_t x, uint8_t y, uint16_t num, uint8_t len, oled_font size);
void bsp_oled_showstring(uint8_t x, uint8_t y, uint8_t *p, oled_font size);
#endif /* BSP_OLED_H */
