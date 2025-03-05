/*!
 * Copyright (c) 2023,苏州聚沃电子科技有限公司
 * All rights reserved.
 *
 * 文件名称： bsp_beep.h
 * 描    述： 蜂鸣器驱动文件
 * 版本：     2023-09-10, V0.1
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
#ifndef BSP_BEEP_H
#define BSP_BEEP_H

#include "driver_public.h"
#include "driver_timer.h"

#define  BEEP_CNT_HZ   10000000 
                                            
#define  L1       ((BEEP_CNT_HZ/262)-1)//中调　do 的频率
#define  L2       ((BEEP_CNT_HZ/296)-1)//中调　re 的频率
#define  L3       ((BEEP_CNT_HZ/330)-1)//中调　mi 的频率
#define  L4       ((BEEP_CNT_HZ/349)-1)//中调　fa 的频率
#define  L5       ((BEEP_CNT_HZ/392)-1)//中调　sol的频率
#define  L6       ((BEEP_CNT_HZ/440)-1)//中调　la 的频率
#define  L7       ((BEEP_CNT_HZ/494)-1)//中调　si 的频率                                           
#define  M1       ((BEEP_CNT_HZ/523)-1)//高调　do 的频率
#define  M2       ((BEEP_CNT_HZ/587)-1)//高调　re 的频率
#define  M3       ((BEEP_CNT_HZ/659)-1)//高调　mi 的频率
#define  M4       ((BEEP_CNT_HZ/699)-1)//高调　fa 的频率
#define  M5       ((BEEP_CNT_HZ/784)-1)//高调　sol的频率
#define  M6       ((BEEP_CNT_HZ/880)-1)//高调　la 的频率
#define  M7       ((BEEP_CNT_HZ/988)-1)//高调　si 的频率
#define  H1       ((BEEP_CNT_HZ/1048)-1)//高调　do 的频率
#define  H2       ((BEEP_CNT_HZ/1176)-1)//高调　re 的频率
#define  H3       ((BEEP_CNT_HZ/1320)-1)//高调　mi 的频率
#define  H4       ((BEEP_CNT_HZ/1480)-1)//高调　fa 的频率
#define  H5       ((BEEP_CNT_HZ/1640)-1)//高调　sol的频率
#define  H6       ((BEEP_CNT_HZ/1760)-1)//高调　la 的频率
#define  H7       ((BEEP_CNT_HZ/1976)-1)//高调　si 的频率
#define  Z0       0//

TIMER_CH_DEF_EXTERN(PWM_BEEP);

void bsp_beep_init(uint32_t counter_frequency,uint16_t period);
void buzzer_set(uint16_t period, uint16_t duty_value);
void buzzer_on(void);
void buzzer_off(void);
#endif /* BSP_BEEP_H*/
