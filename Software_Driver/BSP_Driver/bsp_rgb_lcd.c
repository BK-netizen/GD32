/*!
 * 文件名称：  bsp_rgb_lcd.c
 * 描    述：  bsp lcd interface
 * 版本：      2023-12-03, V1.0
*/

/*
* GD32H757海棠派开发板V1.0
* 聚沃科技官网：https://www.gd32bbs.com/
* B站视频教程：https://space.bilibili.com/475462605
* CSDN文档教程：https://gd32mcu.blog.csdn.net/
* 聚沃开发板购买：https://juwo.taobao.com
* GD技术交流QQ群：859440462
* GD官方开发板、样品购买，咨询Q：87205168  咨询V：13905102619
* GD32 MCU方案移植、方案开发，咨询Q：87205168  咨询V：13905102619
* Copyright@苏州聚沃电子科技有限公司，盗版必究。
*/

#include "bsp_rgb_lcd.h"
#include "driver_public.h"
#include "driver_gpio.h"
#include "driver_timer.h"
#include "driver_dma.h"

#include <stdarg.h>



//用户配置定义区
#define BACK_LIGHT_DUTY 80

TIMER_CH_DEF(LCD_RGB_BL,TIMER3,1,TIMER_CH_PWM_HIGH,D,13,AF_PP,GPIO_AF_2);

SET_DMA_AREA_SDRAM uint16_t ltdc_lcd_framebuf0[LCD_HEIGHT][LCD_WIDTH];

typde_rgb_lcd_printf_parameter_struct bsp_rgb_lcd_pritnf_parameter;


/* 管理LCD重要参数 */
typde_rgb_lcd_parameter_struct bsp_rgb_lcd_parameter;


static __IO uint8_t lcd_backlight_duty=0;

static void bsp_rgb_lcd_tli_gpio_config(void)
{
    /* enable the periphral clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);

    /* configure HSYNC(PC6), VSYNC(PA4), PCLK(PG7), DE(PF10) */
    /* configure LCD_R7(PG6), LCD_R6(PA8), LCD_R5(PA12), LCD_R4(PA11), LCD_R3(PB0), 
                 LCD_G7(PD3), LCD_G6(PC7), LCD_G5(PB11), LCD_G4(PB10), LCD_G3(PG10), LCD_G2(PA6),
                 LCD_B7(PB9), LCD_B6(PB8), LCD_B5(PA3), LCD_B4(PG12), LCD_B3(PG11) */
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_3);
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_4);
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_6);
//    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_8);
//    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_11);
//    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_12);
    
    gpio_af_set(GPIOA,GPIO_AF_14,GPIO_PIN_5);
    gpio_af_set(GPIOC,GPIO_AF_14,GPIO_PIN_0);    
    
    gpio_af_set(GPIOB,GPIO_AF_9,GPIO_PIN_0);
    gpio_af_set(GPIOB,GPIO_AF_9,GPIO_PIN_1);    
    gpio_af_set(GPIOB,GPIO_AF_14,GPIO_PIN_8);
    gpio_af_set(GPIOB,GPIO_AF_14,GPIO_PIN_9);
    gpio_af_set(GPIOB,GPIO_AF_14,GPIO_PIN_10);
    gpio_af_set(GPIOB,GPIO_AF_14,GPIO_PIN_11);
    
    gpio_af_set(GPIOC,GPIO_AF_14,GPIO_PIN_6);
    gpio_af_set(GPIOC,GPIO_AF_14,GPIO_PIN_7);
    
    gpio_af_set(GPIOD,GPIO_AF_14,GPIO_PIN_3);

    gpio_af_set(GPIOF,GPIO_AF_14,GPIO_PIN_10);
    
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_6);
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_7);
    gpio_af_set(GPIOG,GPIO_AF_9,GPIO_PIN_10);
    gpio_af_set(GPIOG,GPIO_AF_14,GPIO_PIN_11);
    gpio_af_set(GPIOG,GPIO_AF_9,GPIO_PIN_12);

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                                |GPIO_PIN_11|GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ,GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_4
                                |GPIO_PIN_6|GPIO_PIN_11|GPIO_PIN_12);

    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                                |GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                                |GPIO_PIN_11);

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0|GPIO_PIN_6|GPIO_PIN_7);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP,GPIO_OSPEED_85MHZ, GPIO_PIN_0|GPIO_PIN_6|GPIO_PIN_7);

    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP,GPIO_OSPEED_85MHZ, GPIO_PIN_3);

    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP,GPIO_OSPEED_85MHZ, GPIO_PIN_10);

    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10|GPIO_PIN_11
                                |GPIO_PIN_12);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ,GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10|GPIO_PIN_11
                                |GPIO_PIN_12);
}

static void bsp_rgb_lcd_tli_init(void)
{
    tli_parameter_struct tli_init_struct;
    tli_layer_parameter_struct tli_layer_init_struct;

    rcu_periph_clock_enable(RCU_TLI);
    
    bsp_rgb_lcd_tli_gpio_config();    

    rcu_pll_input_output_clock_range_config(IDX_PLL2, RCU_PLL2RNG_1M_2M, RCU_PLL2VCO_150M_420M);
    
    /* configure the PLL2 clock: CK_PLL2P/CK_PLL2Q/CK_PLL2R = HXTAL_VALUE / 25 * 150 / 3 */
    if(ERROR == rcu_pll2_config(25, 240, 3, 3, 2)) {
        while(1) {
        }
    }
    rcu_pll_clock_output_enable(RCU_PLL2R);
    
    rcu_tli_clock_div_config(RCU_PLL2R_DIV4);

    rcu_osci_on(RCU_PLL2_CK);

    if(ERROR == rcu_osci_stab_wait(RCU_PLL2_CK)) {
        while(1) {
        }
    }

    /* configure TLI parameter struct */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;
    /* LCD display timing configuration */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH - 1;
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT - 1;
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;
    /* configure LCD background R,G,B values */
    tli_init_struct.backcolor_red = 0xFF;
    tli_init_struct.backcolor_green = 0xFF;
    tli_init_struct.backcolor_blue = 0xFF;
    tli_init(&tli_init_struct);


    /* TLI layer0 configuration */
    /* TLI window size configuration */
    tli_layer_init_struct.layer_window_leftpos = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_rightpos = (ACTIVE_WIDTH + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
    tli_layer_init_struct.layer_window_toppos = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_bottompos = (ACTIVE_HEIGHT + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);
    /* TLI window pixel format configuration */
    tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
    /* TLI window specified alpha configuration */
    tli_layer_init_struct.layer_sa = 255;
    /* TLI layer default alpha R,G,B value configuration */
    tli_layer_init_struct.layer_default_blue = 0x00;
    tli_layer_init_struct.layer_default_green = 0x00;
    tli_layer_init_struct.layer_default_red = 0x00;
    tli_layer_init_struct.layer_default_alpha = 0x00;
    /* TLI window blend configuration */
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_SA;
    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_SA;
    /* TLI layer frame buffer base address configuration */
    tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)ltdc_lcd_framebuf0;
    tli_layer_init_struct.layer_frame_line_length = ((ACTIVE_WIDTH * 2) + 3);
    tli_layer_init_struct.layer_frame_buf_stride_offset = (ACTIVE_WIDTH * 2);
    tli_layer_init_struct.layer_frame_total_line_number = ACTIVE_HEIGHT;
    tli_layer_init(LAYER0, &tli_layer_init_struct);
    tli_dither_config(TLI_DITHER_ENABLE);
}

void bsp_rgb_lcd_backlight_on(void)
{    
    driver_timer_channel_init(&LCD_RGB_BL,10000000,100);//1M/1000 pwm ck=1K
       
    driver_timer_pwm_on(&LCD_RGB_BL);
	
    driver_timer_pwm_on(&LCD_RGB_BL);
    driver_timer_pwm_duty_set(&LCD_RGB_BL,lcd_backlight_duty);    
}

void bsp_rgb_lcd_backlight_off(void)
{   
    driver_timer_pwm_off(&LCD_RGB_BL);    
    driver_timer_pwm_duty_set(&LCD_RGB_BL,0);
}

void bsp_rgb_lcd_backlight_duty_set(uint8_t backlight_value)
{    
    lcd_backlight_duty=backlight_value;
    driver_timer_pwm_duty_set(&LCD_RGB_BL,lcd_backlight_duty);
}


static void bsp_rgb_lcd_port_init(void)
{    
    driver_timer_channel_init(&LCD_RGB_BL,10000000,100);//1M/1000 pwm ck=1K
       
    driver_timer_pwm_on(&LCD_RGB_BL);
    
    driver_dma_mem_init(&DMA_MEM);
    
    bsp_rgb_lcd_tli_init();    

}


/**
 * 说明       初始化LCD
 *   @note    
 *
 * 输入       无
 * 返回值      无
 */
uint32_t bsp_rgb_lcd_init(void)
{
    
    bsp_rgb_lcd_parameter.display_ram0 = ltdc_lcd_framebuf0[0];
    
    bsp_rgb_lcd_port_init();
    
    bsp_rgb_lcd_backlight_duty_set(BACK_LIGHT_DUTY);
    
    bsp_rgb_lcd_backlight_on();          /* 点亮背光 */      
    
    delay_ms(10);        
    
    /* LCD的画笔颜色和背景色 */
    bsp_rgb_lcd_parameter.g_point_color = WHITE;    /* 画笔颜色 */
    bsp_rgb_lcd_parameter.g_back_color  = BLACK;    /* 背景色 */    
    
    delay_ms(1);     /* 初始化FSMC后,必须等待一定时间才能开始初始化 */

    bsp_rgb_lcd_display_dir(0); /* 默认为竖屏 */  
    
    bsp_rgb_lcd_clear(WHITE);
    
    tli_layer_enable(LAYER0);
    tli_reload_config(TLI_FRAME_BLANK_RELOAD_EN);
    tli_enable();  

  
    
    return DRV_SUCCESS;
}


/**
 * 说明       读取个某点的颜色值
 * 输入       x,y:坐标
 * 返回值      此点的颜色(32位颜色,方便兼容LTDC)
 */
uint32_t bsp_rgb_lcd_read_point(uint16_t x, uint16_t y)
{
    
    if(x>bsp_rgb_lcd_parameter.width || y>bsp_rgb_lcd_parameter.height)
    {
        return 0;
    }
    
    if(bsp_rgb_lcd_parameter.display_ram0 != NULL)
    {
        return bsp_rgb_lcd_parameter.display_ram0[y*bsp_rgb_lcd_parameter.width+x];
    }        
    return 0; 
}

/**
 * 说明       LCD开启显示
 * 输入       无
 * 返回值      无
 */
void bsp_rgb_lcd_display_on(void)
{
    bsp_rgb_lcd_backlight_on();          /* 点亮背光 */      
    tli_enable();
}

/**
 * 说明       LCD关闭显示
 * 输入       无
 * 返回值      无
 */
void bsp_rgb_lcd_display_off(void)
{
    bsp_rgb_lcd_backlight_off();          /* 点亮背光 */      
    tli_disable();
}


/**
 * 说明       画点
 * 输入       x,y: 坐标
 * 输入       color: 点的颜色(32位颜色,方便兼容LTDC)
 * 返回值      无
 */
void bsp_rgb_lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
//    bsp_rgb_lcd_parameter.display_ram0
    if(x>bsp_rgb_lcd_parameter.width || y>bsp_rgb_lcd_parameter.height)
    {
        return;
    }
    
    if(bsp_rgb_lcd_parameter.display_ram0 != NULL)
    {    

        if(bsp_rgb_lcd_parameter.dir==0)
        {
            *(__IO uint16_t*)(((uint32_t)(bsp_rgb_lcd_parameter.display_ram0)) + 2*((bsp_rgb_lcd_parameter.width*y) + x)) = color; 
        }
        else
        {
            *(__IO uint16_t*)(((uint32_t)(bsp_rgb_lcd_parameter.display_ram0)) + 2*((bsp_rgb_lcd_parameter.height*x) + y)) = color;         
        }
    }        
}


/**
 * 说明       设置LCD显示方向
 * 输入       dir:0,竖屏; 1,横屏
 * 返回值      无
 */
void bsp_rgb_lcd_display_dir(uint8_t dir)
{
    bsp_rgb_lcd_parameter.dir = dir;   /* 竖屏/横屏 */

    if (dir == 0)      /* 横屏 */ 
    {
        bsp_rgb_lcd_parameter.width = LCD_WIDTH;
        bsp_rgb_lcd_parameter.height = LCD_HEIGHT;  
    }
    else               /* 竖屏 */ 
    {
        bsp_rgb_lcd_parameter.width = LCD_HEIGHT;
        bsp_rgb_lcd_parameter.height = LCD_WIDTH;
    }
}


/**
 * 说明       清屏函数
 * 输入       color: 要清屏的颜色
 * 返回值      无
 */
void bsp_rgb_lcd_clear(uint16_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = bsp_rgb_lcd_parameter.width;
    bsp_rgb_lcd_parameter.g_back_color = color;
    
    bsp_rgb_lcd_parameter.g_point_color = ~color;

    totalpoint *= bsp_rgb_lcd_parameter.height;    /* 得到总点数 */
    
    if(bsp_rgb_lcd_parameter.display_ram0 != NULL)
    { 
        for (index = 0; index < totalpoint; index++)
        {
            *(__IO uint16_t*)((uint32_t)(bsp_rgb_lcd_parameter.display_ram0)+2*index)=color;
        }
    }    
}

/**
 * 说明       在指定区域内填充单个颜色
 * 输入       (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex - sx + 1) * (ey - sy + 1)
 * 输入       color: 要填充的颜色(32位颜色,方便兼容LTDC)
 * 返回值      无
 */
void bsp_rgb_lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    uint16_t height, width,x,y;

    if(sx>bsp_rgb_lcd_parameter.width || ex>bsp_rgb_lcd_parameter.width || sy>bsp_rgb_lcd_parameter.height || ey>bsp_rgb_lcd_parameter.height)
    {
        return;
    }
    width = ex - sx + 1;            /* 得到填充的宽度 */
    height = ey - sy + 1;           /* 高度 */
    
    if(bsp_rgb_lcd_parameter.display_ram0 != NULL)
    { 
        if(bsp_rgb_lcd_parameter.dir==0)
        {
            for(y = sy; y < sy + height; y++){      
                for(x = sx; x < sx + width; x++){ 
                    *(__IO uint16_t*)( (uint32_t)(bsp_rgb_lcd_parameter.display_ram0) + 2*((bsp_rgb_lcd_parameter.width*y) + x) ) = color;
                }                
            }
        }
        else
        {
            for(y = sy; y < sy + height; y++){            
                for(x = sx; x < sx + width; x++){     
                    *(__IO uint16_t*)( (uint32_t)(bsp_rgb_lcd_parameter.display_ram0) + 2*((bsp_rgb_lcd_parameter.height*x) + y) ) = color;
                }                
            }        
        }
    }        
}

/**
 * 说明       在指定区域内填充指定颜色块
 * 输入       (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex - sx + 1) * (ey - sy + 1)
 * 输入       color: 要填充的颜色数组首地址
 * 返回值      无
 */
void bsp_rgb_lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t height, width,x,y;
    
    if(sx>bsp_rgb_lcd_parameter.width || ex>bsp_rgb_lcd_parameter.width || sy>bsp_rgb_lcd_parameter.height || ey>bsp_rgb_lcd_parameter.height)
    {
        return;
    }    

    width = ex - sx + 1;            /* 得到填充的宽度 */
    height = ey - sy + 1;           /* 高度 */
    
    if(bsp_rgb_lcd_parameter.display_ram0 != NULL)
    { 
        if(bsp_rgb_lcd_parameter.dir==0)
        {        
            for(y = sy; y < sy + height; y++){      
                for(x = sx; x < sx + width; x++){ 
                    *(__IO uint16_t*)( (uint32_t)(bsp_rgb_lcd_parameter.display_ram0) + 2*((bsp_rgb_lcd_parameter.width*y) + x) ) = *(color++);
                }                
            }
        }        
        else
        {
            for(y = sy; y < sy + height; y++){            
                for(x = sx; x < sx + width; x++){                       
                    *(__IO uint16_t*)( (uint32_t)(bsp_rgb_lcd_parameter.display_ram0) + 2*((bsp_rgb_lcd_parameter.height*x) + y) ) = *(color++);
                }                
            }        
        }        
    }  
}


/**
 * 说明       画线
 * 输入       x1,y1: 起点坐标
 * 输入       x2,y2: 终点坐标
 * 输入       color: 线的颜色
 * 返回值      无
 */
void bsp_rgb_lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    delta_x = x2 - x1;          /* 计算坐标增量 */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)incx = 1;   /* 设置单步方向 */
    else if (delta_x == 0)incx = 0; /* 垂直线 */
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; /* 水平线 */
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x;  /* 选取基本增量坐标轴 */
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )   /* 画线输出 */
    {
        bsp_rgb_lcd_draw_point(row, col, color); /* 画点 */
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * 说明       画水平线
 * 输入       x0,y0: 起点坐标
 * 输入       len  : 线长度
 * 输入       color: 矩形的颜色
 * 返回值      无
 */
void bsp_rgb_lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > bsp_rgb_lcd_parameter.width) || (y > bsp_rgb_lcd_parameter.height))return;

    bsp_rgb_lcd_fill(x, y, x + len - 1, y, color);
}

/**
 * 说明       画矩形
 * 输入       x1,y1: 起点坐标
 * 输入       x2,y2: 终点坐标
 * 输入       color: 矩形的颜色
 * 返回值      无
 */
void bsp_rgb_lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    bsp_rgb_lcd_draw_line(x1, y1, x2, y1, color);
    bsp_rgb_lcd_draw_line(x1, y1, x1, y2, color);
    bsp_rgb_lcd_draw_line(x1, y2, x2, y2, color);
    bsp_rgb_lcd_draw_line(x2, y1, x2, y2, color);
}

/**
 * 说明       画圆
 * 输入       x,y  : 圆中心坐标
 * 输入       r    : 半径
 * 输入       color: 圆的颜色
 * 返回值      无
 */
void bsp_rgb_lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       /* 判断下个点位置的标志 */

    while (a <= b)
    {
        bsp_rgb_lcd_draw_point(x0 + a, y0 - b, color);  /* 5 */
        bsp_rgb_lcd_draw_point(x0 + b, y0 - a, color);  /* 0 */
        bsp_rgb_lcd_draw_point(x0 + b, y0 + a, color);  /* 4 */
        bsp_rgb_lcd_draw_point(x0 + a, y0 + b, color);  /* 6 */
        bsp_rgb_lcd_draw_point(x0 - a, y0 + b, color);  /* 1 */
        bsp_rgb_lcd_draw_point(x0 - b, y0 + a, color);
        bsp_rgb_lcd_draw_point(x0 - a, y0 - b, color);  /* 2 */
        bsp_rgb_lcd_draw_point(x0 - b, y0 - a, color);  /* 7 */
        a++;

        /* 使用Bresenham算法画圆 */
        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/**
 * 说明       填充实心圆
 * 输入       x0,y0: 圆中心坐标
 * 输入       r    : 半径
 * 输入       color: 圆的颜色
 * 返回值      无
 */
void bsp_rgb_lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    uint32_t i;
    uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
    uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    uint32_t xr = r;

    bsp_rgb_lcd_draw_hline(x - r, y, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
        if ((i * i + xr * xr) > sqmax)
        {
            /* draw lines from outside */
            if (xr > imax)
            {
                bsp_rgb_lcd_draw_hline (x - i + 1, y + xr, 2 * (i - 1), color);
                bsp_rgb_lcd_draw_hline (x - i + 1, y - xr, 2 * (i - 1), color);
            }

            xr--;
        }

        /* draw lines from inside (center) */
        bsp_rgb_lcd_draw_hline(x - xr, y + i, 2 * xr, color);
        bsp_rgb_lcd_draw_hline(x - xr, y - i, 2 * xr, color);
    }
}

/**
 * 说明       在指定位置显示一个字符
 * 输入       x,y  : 坐标
 * 输入       chr  : 要显示的字符:" "--->"~"
 * 输入       size : 字体大小 FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * 输入       mode : 叠加方式(1); 非叠加方式(0);
 * 返回值      无
 */
void bsp_rgb_lcd_show_char(uint16_t x, uint16_t y, char chr, FONT_ASCII size, uint8_t mode, uint16_t color,uint16_t back_color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* 得到字体一个字符对应点阵集所占的字节数 */
    chr = chr - ' ';    /* 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库） */

    switch (size)
    {
        case FONT_ASCII_12_6:
            pfont = (uint8_t *)ascii_12_6[chr];  /* 调用1206字体 */
            break;

        case FONT_ASCII_16_8:
            pfont = (uint8_t *)ascii_16_8[chr];  /* 调用1608字体 */
            break;

        case FONT_ASCII_24_12:
            pfont = (uint8_t *)ascii_24_12[chr];  /* 调用2412字体 */
            break;

        case FONT_ASCII_32_16:
            pfont = (uint8_t *)ascii_32_16[chr];  /* 调用3216字体 */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];    /* 获取字符的点阵数据 */

        for (t1 = 0; t1 < 8; t1++)   /* 一个字节8个点 */
        {
            if (temp & 0x80)        /* 有效点,需要显示 */
            {
                bsp_rgb_lcd_draw_point(x, y, color);        /* 画点出来,要显示这个点 */
            }
            else if (mode == 0)     /* 无效点,不显示 */
            {
                bsp_rgb_lcd_draw_point(x, y, back_color); /* 画背景色,相当于这个点不显示(注意背景色由全局变量控制) */
            }

            temp <<= 1; /* 移位, 以便获取下一个位的状态 */
            y++;

            if (y >= bsp_rgb_lcd_parameter.height)return;  /* 超区域了 */

            if ((y - y0) == size)   /* 显示完一列了? */
            {
                y = y0; /* y坐标复位 */
                x++;    /* x坐标递增 */

                if (x >= bsp_rgb_lcd_parameter.width)return;   /* x坐标超区域了 */

                break;
            }
        }
    }
}

/**
 * 说明       平方函数, m^n
 * 输入       m: 底数
 * 输入       n: 指数
 * 返回值      m的n次方
 */
static uint32_t bsp_rgb_lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)result *= m;

    return result;
}

/**
 * 说明       显示len个数字
 * 输入       x,y : 起始坐标
 * 输入       num : 数值(0 ~ 2^32)
 * 输入       len : 显示数字的位数
 * 输入       size: 选择字体 FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * 返回值      无
 */
void bsp_rgb_lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, FONT_ASCII size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* 按总显示位数循环 */
    {
        temp = (num / bsp_rgb_lcd_pow(10, len - t - 1)) % 10;   /* 获取对应位的数字 */

        if (enshow == 0 && t < (len - 1))   /* 没有使能显示,且还有位要显示 */
        {
            if (temp == 0)
            {
                bsp_rgb_lcd_show_char(x + (size / 2)*t, y, ' ', size, 0, color,bsp_rgb_lcd_parameter.g_back_color);/* 显示空格,占位 */
                continue;   /* 继续下个一位 */
            }
            else
            {
                enshow = 1; /* 使能显示 */
            }

        }

        bsp_rgb_lcd_show_char(x + (size / 2)*t, y, temp + '0', size, 0, color,bsp_rgb_lcd_parameter.g_back_color); /* 显示字符 */
    }
}

/**
 * 说明       扩展显示len个数字(高位是0也显示)
 * 输入       x,y : 起始坐标
 * 输入       num : 数值(0 ~ 2^32)
 * 输入       len : 显示数字的位数
 * 输入       size: 选择字体 FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * 输入       mode: 显示模式
 *              [7]:0,不填充;1,填充0.
 *              [6:1]:保留
 *              [0]:0,非叠加显示;1,叠加显示.
 *
 * 返回值      无
 */
void bsp_rgb_lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, FONT_ASCII size, uint8_t mode, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* 按总显示位数循环 */
    {
        temp = (num / bsp_rgb_lcd_pow(10, len - t - 1)) % 10;    /* 获取对应位的数字 */

        if (enshow == 0 && t < (len - 1))   /* 没有使能显示,且还有位要显示 */
        {
            if (temp == 0)
            {
                if (mode & 0X80)   /* 高位需要填充0 */
                {
                    bsp_rgb_lcd_show_char(x + (size / 2)*t, y, '0', size, mode & 0X01, color,bsp_rgb_lcd_parameter.g_back_color);  /* 用0占位 */
                }
                else
                {
                    bsp_rgb_lcd_show_char(x + (size / 2)*t, y, ' ', size, mode & 0X01, color,bsp_rgb_lcd_parameter.g_back_color);  /* 用空格占位 */
                }

                continue;
            }
            else
            {
                enshow = 1; /* 使能显示 */
            }

        }

        bsp_rgb_lcd_show_char(x + (size / 2)*t, y, temp + '0', size, mode & 0X01, color,bsp_rgb_lcd_parameter.g_back_color);
    }
}

/**
 * 说明       显示字符串
 * 输入       x,y         : 起始坐标
 * 输入       width,height: 区域大小
 * 输入       size        : 选择字体 FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * 输入       p           : 字符串首地址
 * 返回值      无
 */
void bsp_rgb_lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, FONT_ASCII size, char *p, uint16_t color)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while( ((*p <= '~') && (*p >= ' ')) || (*p =='\r') || (*p =='\n') )   /* 判断是不是非法字符! */
    {
        if((*p =='\r'))
        {
            x = x0;
        }        
        else if((*p =='\n'))
        {
            y += size;        
        }
        else if (x >= width)
        {
            x = x0;
            y += size;
        }
        if (y >= height)break;  /* 退出 */

        bsp_rgb_lcd_show_char(x, y, *p, size, 0, color,bsp_rgb_lcd_parameter.g_back_color);
        x += size / 2;
        p++;
    }
}

/**
 * 说明       LCD打印窗口初始化
 * 输入       x,y         : 起始坐标
 * 输入       x_end,y_end:  结束点坐标
 * 输入       size        : 选择字体 FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * 输入       back_color  : 背景颜色
 * 输入       point_color   字体颜色
 * 返回值      无
 */
void bsp_rgb_lcd_printf_init(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end,FONT_ASCII char_size,uint16_t back_color,uint16_t point_color)
{
    bsp_rgb_lcd_pritnf_parameter.x_start=x_start;
    bsp_rgb_lcd_pritnf_parameter.y_start=y_start;

    bsp_rgb_lcd_pritnf_parameter.x_end=x_end;
    bsp_rgb_lcd_pritnf_parameter.y_end=y_end;

    bsp_rgb_lcd_pritnf_parameter.x_now=x_start;
    bsp_rgb_lcd_pritnf_parameter.y_now=y_start;

    bsp_rgb_lcd_pritnf_parameter.size=char_size;

    bsp_rgb_lcd_pritnf_parameter.back_color=back_color;   
    bsp_rgb_lcd_pritnf_parameter.point_color=point_color; 

    bsp_rgb_lcd_fill(bsp_rgb_lcd_pritnf_parameter.x_start,bsp_rgb_lcd_pritnf_parameter.y_start,bsp_rgb_lcd_pritnf_parameter.x_end,bsp_rgb_lcd_pritnf_parameter.y_end,bsp_rgb_lcd_pritnf_parameter.back_color);    
}


/**
 * 说明       LCD打印
 * 输入       ...和printf相同用法，自动换行
 * 返回值      无
 */
void bsp_rgb_lcd_printf(const char * sFormat, ...) 
{
    #define PRINTF_MAX_LEN 100
    char printf_buffer[PRINTF_MAX_LEN];
    char* p=printf_buffer;
    uint16_t len=0,count=0;
    va_list ParamList;
    va_start(ParamList, sFormat);     
    vsprintf(printf_buffer,sFormat, ParamList);     
    va_end(ParamList); 

    len=strlen(printf_buffer);    

    while( ( ((*p <= '~') && (*p >= ' ')) || (*p =='\r') || (*p =='\n') ) && (count<len) )   /* 判断是不是非法字符! */
    {
        if((*p =='\r'))
        {
            bsp_rgb_lcd_pritnf_parameter.x_now = bsp_rgb_lcd_pritnf_parameter.x_start;
        }        
        else if((*p =='\n'))
        {
            bsp_rgb_lcd_pritnf_parameter.y_now += bsp_rgb_lcd_pritnf_parameter.size; 
            bsp_rgb_lcd_fill(bsp_rgb_lcd_pritnf_parameter.x_now,bsp_rgb_lcd_pritnf_parameter.y_now,bsp_rgb_lcd_pritnf_parameter.x_end,bsp_rgb_lcd_pritnf_parameter.y_now+bsp_rgb_lcd_pritnf_parameter.size,bsp_rgb_lcd_pritnf_parameter.back_color);            
        }
        else if( (bsp_rgb_lcd_pritnf_parameter.x_now + bsp_rgb_lcd_pritnf_parameter.size/2) > bsp_rgb_lcd_pritnf_parameter.x_end)
        {
            bsp_rgb_lcd_pritnf_parameter.x_now = bsp_rgb_lcd_pritnf_parameter.x_start;
            bsp_rgb_lcd_pritnf_parameter.y_now += bsp_rgb_lcd_pritnf_parameter.size;
            bsp_rgb_lcd_fill(bsp_rgb_lcd_pritnf_parameter.x_now,bsp_rgb_lcd_pritnf_parameter.y_now,bsp_rgb_lcd_pritnf_parameter.x_end,bsp_rgb_lcd_pritnf_parameter.y_now+bsp_rgb_lcd_pritnf_parameter.size,bsp_rgb_lcd_pritnf_parameter.back_color);                        
        }
        else if ( (bsp_rgb_lcd_pritnf_parameter.y_now+bsp_rgb_lcd_pritnf_parameter.size) > bsp_rgb_lcd_pritnf_parameter.y_end)
        {
            bsp_rgb_lcd_pritnf_parameter.x_now = bsp_rgb_lcd_pritnf_parameter.x_start;
            bsp_rgb_lcd_pritnf_parameter.y_now = bsp_rgb_lcd_pritnf_parameter.y_start;            
            bsp_rgb_lcd_fill(bsp_rgb_lcd_pritnf_parameter.x_start,bsp_rgb_lcd_pritnf_parameter.y_start,bsp_rgb_lcd_pritnf_parameter.x_end,bsp_rgb_lcd_pritnf_parameter.y_end,bsp_rgb_lcd_pritnf_parameter.back_color);
        }

        if((*p !='\r')&&(*p !='\n'))
        {
            bsp_rgb_lcd_show_char(bsp_rgb_lcd_pritnf_parameter.x_now, bsp_rgb_lcd_pritnf_parameter.y_now, *p, bsp_rgb_lcd_pritnf_parameter.size, 0, bsp_rgb_lcd_pritnf_parameter.point_color,bsp_rgb_lcd_pritnf_parameter.back_color);
            bsp_rgb_lcd_pritnf_parameter.x_now += bsp_rgb_lcd_pritnf_parameter.size / 2;            
        }
        

        p++;
        count++;
    }

}


void bsp_rgb_lcd_show_log(void)
{         
    //显示log图片
    bsp_rgb_lcd_color_fill(100,0,273,99,(uint16_t*)gd_log_picture);
}



