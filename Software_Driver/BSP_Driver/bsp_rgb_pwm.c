/*!
 * �ļ����ƣ� bsp_rgb_pwm.c
 * ��    ���� rgb���������ļ�
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
#include "bsp_rgb_pwm.h"

/* ����ע��RGB��ӦPWMͨ�� */
TIMER_CH_DEF(RGB_TIMER_R,TIMER0,0,TIMER_CH_PWM_LOW,A,8,AF_PP,GPIO_AF_1);
TIMER_CH_DEF(RGB_TIMER_G,TIMER0,1,TIMER_CH_PWM_LOW,A,9,AF_PP,GPIO_AF_1);
TIMER_CH_DEF(RGB_TIMER_B,TIMER0,2,TIMER_CH_PWM_LOW,A,10,AF_PP,GPIO_AF_1);

/*!
* ˵��     RGB��ʼ��
* ����[1]  counter_frequency ������Ƶ��
* ����[2]  period ����ֵ
* ����ֵ   ��
*/
void bsp_rgb_init(uint32_t counter_frequency,uint16_t period)
{
    driver_timer_multi_channel_init(&RGB_TIMER_R,&RGB_TIMER_G,&RGB_TIMER_B,NULL,counter_frequency,period);
    driver_timer_pwm_on(&RGB_TIMER_R);
    driver_timer_pwm_on(&RGB_TIMER_G);
    driver_timer_pwm_on(&RGB_TIMER_B);    
}


/*!
* ˵��     RGB����
* ����[1]  r_duty_value ����ָ��Ƶ�ʣ�
* ����[2]  g_duty_value ռ�ձȣ�0--100��
* ����[3]  b_duty_value ռ�ձȣ�0--100��
* ����ֵ   ��
*/
void bsp_rgb_duty_set(uint8_t r_duty_value, uint8_t g_duty_value, uint8_t b_duty_value)
{
    driver_timer_pwm_duty_set(&RGB_TIMER_R,r_duty_value);
    driver_timer_pwm_duty_set(&RGB_TIMER_G,g_duty_value); 
    driver_timer_pwm_duty_set(&RGB_TIMER_B,b_duty_value);
}
