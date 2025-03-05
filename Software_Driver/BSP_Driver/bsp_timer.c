/*!
 * �ļ����ƣ� bsp_timer.c
 * ��    ���� ��ʱ�������ļ�
 * �汾��     2023-12-03, V1.0
*/

/*
* GD32H757�����ɿ�����V1.0
* ���ֿƼ�������https://www.gd32bbs.com/
* Bվ��Ƶ�̳̣�https://space.bilibili.com/475462605
* CSDN�ĵ��̳̣�https://gd32mcu.blog.csdn.net/
* ���ֿ����幺��https://juwo.taobao.com
* GD��������QQȺ��859440462
* GD�ٷ������塢��Ʒ������ѯQ��87205168  ��ѯV��13905102619
* GD32 MCU������ֲ��������������ѯQ��87205168  ��ѯV��13905102619
* Copyright@���ݾ��ֵ��ӿƼ����޹�˾������ؾ���
*/

#include "driver_timer.h"
#include "bsp_timer.h"

/* ע�ᶨ�嶨ʱ���ж�ʹ�õ�timer */
TIMER_BASE_DEF(TIMER_INT,TIMER5);

/*!
* ˵��     ��ʱ���жϳ�ʼ��
* ����[1]  counter_frequency��������Ƶ��ֵ
* ����[2]  period������ֵ
* ����ֵ   ��
*/
void bsp_timer_init(uint32_t counter_frequency,uint16_t period)
{
    driver_timer_base_int_init(&TIMER_INT,counter_frequency,period);
    nvic_irq_enable(TIMER5_DAC_UDR_IRQn,0,0);
}

/*!
* ˵��     ��ʱ���жϷ�����
* ����[1]  ��
* ����ֵ   ��
*/
void TIMER5_DAC_UDR_IRQHandler(void)
{
    driver_timer_updata_int_handler(&TIMER_INT);                     
}


