/*!
 * 文件名称： bsp_oled.h
 * 描    述： OLED驱动文件
 * 版本：     2023-12-03, V1.0
*/

/*
* GD32H757海棠派开发板V1.0
* 淘宝商城：   https://juwo.taobao.com
* 技术交流：   https://www.gd32bbs.com/ask/
* 视频学习：   https://space.bilibili.com/475462605
* 微信公众号： gd32bbs
* Copyright    苏州聚沃电子科技有限公司
* 版权所有，盗版必究。
*/

#ifndef BSP_OLED_H
#define BSP_OLED_H

#include "driver_gpio.h"

#define MAX_ROW                     (64)
#define MAX_COLUMN                  (128)
#define MAX_PAGE                    (MAX_ROW/8)
#define BRIGHTNESS                  (0xff)
#define OLED_ADDR 0x78

/* 数据类型枚举 */
typedef enum {
    FONT_6_8 = 0,
    FONT_8_16
}oled_font;

/* 命令 数据枚举 */
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
