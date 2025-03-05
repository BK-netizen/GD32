/*!
 * Copyright (c) 2023,苏州聚沃电子科技有限公司
 * All rights reserved.
 *
 * 文件名称： bsp_timer.h
 * 描    述： timer interface
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
#ifndef BSP_TIMER_H
#define BSP_TIMER_H

#include "driver_timer.h"
#include "driver_public.h"

TIMER_BASE_DEF_EXTERN(TIMER_INT);
void bsp_timer_init(uint32_t counter_frequency,uint16_t period);
#endif /* BSP_TIMER_H*/
