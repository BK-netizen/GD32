/*!
 * Copyright (c) 2023,苏州聚沃电子科技有限公司
 * All rights reserved.
 *
 * 文件名称： bsp_adc.h
 * 描    述： adc interface
 * 版本：     2023-09-10, V0.1
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


#ifndef BSP_ADC_H
#define BSP_ADC_H
#include "driver_public.h"
#include "driver_adc.h"
extern uint16_t BSP_ADC_data[2];
extern typdef_adc_ch_general  BSP_ADC;
extern typdef_adc_ch_parameter BSP_ADC_ch[2];
void bsp_ADC_config(void);
#endif /* BSP_ADC_H*/
