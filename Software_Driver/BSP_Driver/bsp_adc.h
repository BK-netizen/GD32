/*!
 * Copyright (c) 2023,���ݾ��ֵ��ӿƼ����޹�˾
 * All rights reserved.
 *
 * �ļ����ƣ� bsp_adc.h
 * ��    ���� adc interface
 * �汾��     2023-09-10, V0.1
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


#ifndef BSP_ADC_H
#define BSP_ADC_H
#include "driver_public.h"
#include "driver_adc.h"
extern uint16_t BSP_ADC_data[2];
extern typdef_adc_ch_general  BSP_ADC;
extern typdef_adc_ch_parameter BSP_ADC_ch[2];
void bsp_ADC_config(void);
#endif /* BSP_ADC_H*/
