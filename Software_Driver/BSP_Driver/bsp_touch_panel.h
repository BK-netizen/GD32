/*!
 * 文件名称： bsp_touch_panel.h
 * 描    述： 电阻屏触摸驱动文件
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

#ifndef BSP_TOUCH_PANEL_H
#define BSP_TOUCH_PANEL_H

#include "driver_public.h"

/* LCD参数结构体 */
typedef struct __typde_lcd_touch_panel_struct
{
    uint16_t AD_Left;     
    uint16_t AD_Right;    
    uint16_t AD_Top;     
    uint16_t AD_Bottom;  
    uint16_t LCD_X;      
    uint16_t LCD_Y;
    
    uint16_t Touch_x_First;      
    uint16_t Touch_y_First; 

    uint16_t Touch_x_Now;      
    uint16_t Touch_y_Now;      
    
    uint16_t Touch_x_Last;      
    uint16_t Touch_y_Last;     
    uint16_t TOUCH_State;    
}typde_lcd_touch_panel_struct;

extern typde_lcd_touch_panel_struct lcd_touch_panel_struct;


#define AD_Left_Default     300
#define AD_Right_Default    3850
#define AD_Top_Default      220
#define AD_Bottom_Default   3850

//#define LCD_X       320
//#define LCD_Y       480

#define CH_X        0XD0
#define CH_Y        0X90

/* number of filter reads */
#define FILTER_READ_TIMES           18
/* lost value of filter */
#define FILTER_LOST_VAL             4
/* error range of AD sample value */  
#define AD_ERR_RANGE                10


/* touch panel gpio configure */
void bsp_touch_panel_init(uint16_t lcd_width,uint16_t lcd_height);
/* get the AD sample value of touch location at X coordinate */
uint16_t bsp_touch_ad_x_get(void);
/* get the AD sample value of touch location at Y coordinate */
uint16_t bsp_touch_ad_y_get(void);
/* get channel X+ AD average sample value */
uint16_t bsp_touch_average_ad_x_get(void);
/* get channel Y+ AD average sample value */
uint16_t bsp_touch_average_ad_y_get(void);
/* get X coordinate value of touch point on LCD screen */
uint16_t bsp_touch_coordinate_x_get(uint16_t adx);
/* get Y coordinate value of touch point on LCD screen */
uint16_t bsp_touch_coordinate_y_get(uint16_t ady);

uint16_t bsp_touch_data_filter(uint8_t channel_select);
Drv_Err bsp_touch_ad_xy_get(uint16_t *ad_x, uint16_t *ad_y);
Drv_Err bsp_touch_scan(uint16_t *x, uint16_t *y);


#endif /* TOUCH_PANEL_H */
