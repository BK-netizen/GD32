/*!
 * 文件名称：  bsp_rgb_lcd.h
 * 描    述：  bsp lcd interface
 * 版本：      2023-12-03, V1.0
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

#ifndef BSP_RGB_LCD_H
#define BSP_RGB_LCD_H

#include "driver_public.h"

#ifndef __BSP_LCD_FONT_H
    #include "./bsp_lcd_font.h"
#endif


#ifndef __GD_LOG_H
    #include "gd_log.h"
#endif


#define LCD_WIDTH 800
#define LCD_HEIGHT 480
#define LCD_FB_BYTE_PER_PIXEL 1


#define HORIZONTAL_SYNCHRONOUS_PULSE 10
#define HORIZONTAL_BACK_PORCH 150
#define ACTIVE_WIDTH LCD_WIDTH
#define HORIZONTAL_FRONT_PORCH 15

#define VERTICAL_SYNCHRONOUS_PULSE 10
#define VERTICAL_BACK_PORCH 140
#define ACTIVE_HEIGHT LCD_HEIGHT
#define VERTICAL_FRONT_PORCH 40


/* LCD参数结构体 */
typedef struct __typde_rgb_lcd_parameter_struct
{
    uint16_t width;     /* LCD 宽度 */
    uint16_t height;    /* LCD 高度 */
    uint8_t  dir;       /* 横屏还是竖屏控制：0，竖屏；1，横屏 */
    
    /* LCD的画笔颜色和背景色 */
    __IO uint32_t g_point_color;    /* 画笔颜色 */
    __IO uint32_t g_back_color;    /* 背景色 */  
    
    uint16_t* display_ram0;    /* 显存地址 */ 
    uint16_t* display_ram1;    /* 显存地址 */     
}typde_rgb_lcd_parameter_struct;


/* LCD prinft参数结构体 */
typedef struct __typde_rgb_lcd_printf_parameter_struct
{
    uint16_t x_start;    
    uint16_t y_start;    
    uint16_t x_end;    
    uint16_t  y_end;    
    uint16_t x_now;  
    uint16_t y_now;  
    
    FONT_ASCII size; 

    /* LCD的画笔颜色和背景色 */
    __IO uint32_t point_color;    /* 画笔颜色 */
    __IO uint32_t back_color;    /* 背景色 */     
}typde_rgb_lcd_printf_parameter_struct;
/******************************************************************************************/
/* LCD参数 */
extern typde_rgb_lcd_parameter_struct bsp_rgb_lcd_parameter; /* 管理LCD重要参数 */

extern typde_rgb_lcd_printf_parameter_struct bsp_rgb_lcd_pritnf_parameter;


/* 常用画笔颜色 */
#define WHITE           0xFFFF      /* 白色 */
#define BLACK           0x0000      /* 黑色 */
#define RED             0xF800      /* 红色 */
#define GREEN           0x07E0      /* 绿色 */
#define BLUE            0x001F      /* 蓝色 */ 
#define MAGENTA         0XF81F      /* 品红色/紫红色 = BLUE + RED */
#define YELLOW          0XFFE0      /* 黄色 = GREEN + RED */
#define CYAN            0X07FF      /* 青色 = GREEN + BLUE */  

/* 非常用颜色 */
#define BROWN           0XBC40      /* 棕色 */
#define BRRED           0XFC07      /* 棕红色 */
#define GRAY            0X8430      /* 灰色 */ 
#define DARKBLUE        0X01CF      /* 深蓝色 */
#define LIGHTBLUE       0X7D7C      /* 浅蓝色 */ 
#define GRAYBLUE        0X5458      /* 灰蓝色 */ 
#define LIGHTGREEN      0X841F      /* 浅绿色 */  
#define LGRAY           0XC618      /* 浅灰色(PANNEL),窗体背景色 */ 
#define LGRAYBLUE       0XA651      /* 浅灰蓝色(中间层颜色) */ 
#define LBBLUE          0X2B12      /* 浅棕蓝色(选择条目的反色) */ 

/******************************************************************************************/

  
/******************************************************************************************/
/* 函数申明 */

void bsp_rgb_lcd_backlight_off(void);
void bsp_rgb_lcd_backlight_on(void);
void bsp_rgb_lcd_backlight_duty_set(uint8_t backlight_value);


void bsp_rgb_lcd_wr_data(volatile uint16_t data);            /* LCD写数据 */
void bsp_rgb_lcd_wr_regno(volatile uint16_t regno);          /* LCD写寄存器编号/地址 */
void bsp_rgb_lcd_write_reg(uint16_t regno, uint16_t data);   /* LCD写寄存器的值 */


uint32_t bsp_rgb_lcd_init(void);                        /* 初始化LCD */ 
void bsp_rgb_lcd_display_on(void);                  /* 开显示 */ 
void bsp_rgb_lcd_display_off(void);                 /* 关显示 */
void bsp_rgb_lcd_scan_dir(uint8_t dir);             /* 设置屏扫描方向 */ 
void bsp_rgb_lcd_display_dir(uint8_t dir);          /* 设置屏幕显示方向 */ 
void bsp_rgb_lcd_ssd_backlight_set(uint8_t pwm);    /* SSD1963 背光控制 */ 

void bsp_rgb_lcd_set_cursor(uint16_t x, uint16_t y);    /* 设置光标 */ 
uint32_t bsp_rgb_lcd_read_point(uint16_t x, uint16_t y);/* 读点(32位颜色,兼容LTDC)  */
void bsp_rgb_lcd_draw_point(uint16_t x, uint16_t y, uint32_t color);/* 画点(32位颜色,兼容LTDC) */

void bsp_rgb_lcd_clear(uint16_t color);     /* LCD清屏 */
void bsp_rgb_lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);                   /* 填充实心圆 */
void bsp_rgb_lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);                  /* 画圆 */
void bsp_rgb_lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);                  /* 画水平线 */
void bsp_rgb_lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);             /* 设置窗口 */
void bsp_rgb_lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);          /* 纯色填充矩形(32位颜色,兼容LTDC) */
void bsp_rgb_lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);   /* 彩色填充矩形 */
void bsp_rgb_lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);     /* 画直线 */
void bsp_rgb_lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);/* 画矩形 */


void bsp_rgb_lcd_show_char(uint16_t x, uint16_t y, char chr, FONT_ASCII size, uint8_t mode, uint16_t color,uint16_t back_color);
void bsp_rgb_lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, FONT_ASCII size, uint16_t color);
void bsp_rgb_lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, FONT_ASCII size, uint8_t mode, uint16_t color);
void bsp_rgb_lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, FONT_ASCII size, char *p, uint16_t color);


void bsp_rgb_lcd_printf_init(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end,FONT_ASCII char_size,uint16_t back_color,uint16_t point_color);
void bsp_rgb_lcd_printf(const char * sFormat, ...);
void bsp_rgb_lcd_show_log(void);

void bsp_rgb_lcd_sram_mode_updata(void);

        
#endif /* BSP_OLED_H */
