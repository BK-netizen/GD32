/*!
 * Copyright (c) 2023,���ݾ��ֵ��ӿƼ����޹�˾
 * All rights reserved.
 *
 * �ļ����ƣ� bsp_timer.h
 * ��    ���� timer interface
 * �汾��     2023-09-10, V0.1
*/

/*
* ��Ȩ˵��
* ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* GD32H757�����ɿ�����V1
* ���߹���   http://juwo.taobao.com
* ����������   http://www.gd32bbs.com/ask/
* ��Ƶѧϰ��   https://space.bilibili.com/475462605
* ΢�Ź��ںţ� gd32bbs
* ��������:    2023/8/30
* Copyright    ���ݾ��ֵ��ӿƼ����޹�˾
* ��Ȩ���У�����ؾ���
*/
#ifndef BSP_TIMER_H
#define BSP_TIMER_H

#include "driver_timer.h"
#include "driver_public.h"

TIMER_BASE_DEF_EXTERN(TIMER_INT);
void bsp_timer_init(uint32_t counter_frequency,uint16_t period);
#endif /* BSP_TIMER_H*/
