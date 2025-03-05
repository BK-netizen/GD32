#ifndef __BSP_TOUCH_CAP_H__
#define __BSP_TOUCH_CAP_H__

#include "driver_public.h"

#define g_gt_tnum  5 // 默认支持的触摸屏点数(5点触摸)

#define TP_PRES_DOWN            0x80 // 触屏被按下
#define TP_CATH_PRES            0x40 // 有按键按下了
#define CT_MAX_TOUCH 5    // 电容屏支持的点数,固定为5点


//I2C读写命令	
#define GT_CMD_WR 		0XBA    //写命令
#define GT_CMD_RD 		0XBB		//读命令
  
//GT9147 部分寄存器定义 
#define GT_CTRL_REG 	0X8040   	//GT911控制寄存器
#define GT_CFGS_REG 	0X8047   	//GT911配置起始地址寄存器
#define GT_CHECK_REG 	0X80FF   	//GT911校验和寄存器
#define GT_PID_REG 		0X8140   	//GT911产品ID寄存器

#define GT_GSTID_REG 	0X814E   	//GT911当前检测到的触摸情况
#define GT_TP1_REG 		0X8150  	//第一个触摸点数据地址
#define GT_TP2_REG 		0X8158		//第二个触摸点数据地址
#define GT_TP3_REG 		0X8160		//第三个触摸点数据地址
#define GT_TP4_REG 		0X8168		//第四个触摸点数据地址
#define GT_TP5_REG 		0X8170		//第五个触摸点数据地址  
// 触摸屏控制器
typedef struct
{
    //    uint8_t (*init)(void);			//初始化触摸屏控制器
    //    uint8_t (*scan)(uint8_t);				//扫描触摸屏.0,屏幕扫描;1,物理坐标;
    //    void (*adjust)(void);		//触摸屏校准
    uint16_t x[CT_MAX_TOUCH]; // 当前坐标
    uint16_t y[CT_MAX_TOUCH]; // 电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
    uint8_t sta;              // 笔的状态
    float xfac;
    float yfac;
    short xoff;
    short yoff;
    uint8_t touchtype;
    uint16_t width;     /* LCD 宽度 */
    uint16_t height;    /* LCD 高度 */
} typde_lcd_touch_cap_struct;

extern typde_lcd_touch_cap_struct lcd_touch_cap_struct; // 触屏控制器在touch.c里面定义

uint8_t bsp_gt911_init(uint16_t width,uint16_t height);
uint8_t bsp_gt911_scan(uint8_t mode);

#endif
