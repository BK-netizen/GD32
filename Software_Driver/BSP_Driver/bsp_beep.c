/*!
 * 文件名称： bsp_beep.c
 * 描    述： 蜂鸣器驱动文件
 * 版本：     2023-12-03, V1.0
*/

/*
* GD32H757海棠派开发板V1.0
* 聚沃科技官网：https://www.gd32bbs.com/
* B站视频教程：https://space.bilibili.com/475462605
* CSDN文档教程：https://gd32mcu.blog.csdn.net/
* 聚沃开发板购买：https://juwo.taobao.com
* GD技术交流QQ群：859440462
* GD官方开发板、样品购买，咨询Q：87205168  咨询V：13905102619
* GD32 MCU方案移植、方案开发，咨询Q：87205168  咨询V：13905102619
* Copyright@苏州聚沃电子科技有限公司，盗版必究。
*/

#include "driver_timer.h"
#include "bsp_beep.h"

/* 定义注册蜂鸣器对应PWM通道 */
TIMER_CH_DEF(PWM_BEEP,TIMER5,0,TIMER_CH_PWM_LOW,F,9,AF_PP,GPIO_AF_9);

/*!
* 说明     蜂鸣器初始化
* 输入[1]  counter_frequency 计数器频率
* 输入[2]  period 周期值
* 返回值   无
*/
void bsp_beep_init(uint32_t counter_frequency,uint16_t period)
{
    driver_timer_channel_init(&PWM_BEEP,counter_frequency,period);
}

/*!
* 说明     蜂鸣器设置
* 输入[1]  period 周期指（频率）
* 输入[2]  duty_value 占空比（0--100）
* 返回值   无
*/
void buzzer_set(uint16_t period, uint16_t duty_value)
{
    driver_timer_pwm_period_duty_set(&PWM_BEEP,period,duty_value);
}

/*!
* 说明     打开蜂鸣器
* 输入[1]  无
* 返回值   无
*/
void buzzer_on(void)
{
    driver_timer_pwm_on(&PWM_BEEP);
}

/*!
* 说明     关闭蜂鸣器
* 输入[1]  无
* 返回值   无
*/
void buzzer_off(void)
{
    driver_timer_pwm_off(&PWM_BEEP);
}


