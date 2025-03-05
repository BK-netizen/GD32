/*!
 * �ļ����ƣ� bsp_beep.c
 * ��    ���� �����������ļ�
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
#include "bsp_beep.h"

/* ����ע���������ӦPWMͨ�� */
TIMER_CH_DEF(PWM_BEEP,TIMER5,0,TIMER_CH_PWM_LOW,F,9,AF_PP,GPIO_AF_9);

/*!
* ˵��     ��������ʼ��
* ����[1]  counter_frequency ������Ƶ��
* ����[2]  period ����ֵ
* ����ֵ   ��
*/
void bsp_beep_init(uint32_t counter_frequency,uint16_t period)
{
    driver_timer_channel_init(&PWM_BEEP,counter_frequency,period);
}

/*!
* ˵��     ����������
* ����[1]  period ����ָ��Ƶ�ʣ�
* ����[2]  duty_value ռ�ձȣ�0--100��
* ����ֵ   ��
*/
void buzzer_set(uint16_t period, uint16_t duty_value)
{
    driver_timer_pwm_period_duty_set(&PWM_BEEP,period,duty_value);
}

/*!
* ˵��     �򿪷�����
* ����[1]  ��
* ����ֵ   ��
*/
void buzzer_on(void)
{
    driver_timer_pwm_on(&PWM_BEEP);
}

/*!
* ˵��     �رշ�����
* ����[1]  ��
* ����ֵ   ��
*/
void buzzer_off(void)
{
    driver_timer_pwm_off(&PWM_BEEP);
}


