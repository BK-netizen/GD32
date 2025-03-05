/*!
 * Copyright (c) 2023,���ݾ��ֵ��ӿƼ����޹�˾
 * All rights reserved.
 *
 * �ļ����ƣ� bsp_beep.h
 * ��    ���� �����������ļ�
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
#ifndef BSP_BEEP_H
#define BSP_BEEP_H

#include "driver_public.h"
#include "driver_timer.h"

#define  BEEP_CNT_HZ   10000000 
                                            
#define  L1       ((BEEP_CNT_HZ/262)-1)//�е���do ��Ƶ��
#define  L2       ((BEEP_CNT_HZ/296)-1)//�е���re ��Ƶ��
#define  L3       ((BEEP_CNT_HZ/330)-1)//�е���mi ��Ƶ��
#define  L4       ((BEEP_CNT_HZ/349)-1)//�е���fa ��Ƶ��
#define  L5       ((BEEP_CNT_HZ/392)-1)//�е���sol��Ƶ��
#define  L6       ((BEEP_CNT_HZ/440)-1)//�е���la ��Ƶ��
#define  L7       ((BEEP_CNT_HZ/494)-1)//�е���si ��Ƶ��                                           
#define  M1       ((BEEP_CNT_HZ/523)-1)//�ߵ���do ��Ƶ��
#define  M2       ((BEEP_CNT_HZ/587)-1)//�ߵ���re ��Ƶ��
#define  M3       ((BEEP_CNT_HZ/659)-1)//�ߵ���mi ��Ƶ��
#define  M4       ((BEEP_CNT_HZ/699)-1)//�ߵ���fa ��Ƶ��
#define  M5       ((BEEP_CNT_HZ/784)-1)//�ߵ���sol��Ƶ��
#define  M6       ((BEEP_CNT_HZ/880)-1)//�ߵ���la ��Ƶ��
#define  M7       ((BEEP_CNT_HZ/988)-1)//�ߵ���si ��Ƶ��
#define  H1       ((BEEP_CNT_HZ/1048)-1)//�ߵ���do ��Ƶ��
#define  H2       ((BEEP_CNT_HZ/1176)-1)//�ߵ���re ��Ƶ��
#define  H3       ((BEEP_CNT_HZ/1320)-1)//�ߵ���mi ��Ƶ��
#define  H4       ((BEEP_CNT_HZ/1480)-1)//�ߵ���fa ��Ƶ��
#define  H5       ((BEEP_CNT_HZ/1640)-1)//�ߵ���sol��Ƶ��
#define  H6       ((BEEP_CNT_HZ/1760)-1)//�ߵ���la ��Ƶ��
#define  H7       ((BEEP_CNT_HZ/1976)-1)//�ߵ���si ��Ƶ��
#define  Z0       0//

TIMER_CH_DEF_EXTERN(PWM_BEEP);

void bsp_beep_init(uint32_t counter_frequency,uint16_t period);
void buzzer_set(uint16_t period, uint16_t duty_value);
void buzzer_on(void);
void buzzer_off(void);
#endif /* BSP_BEEP_H*/
