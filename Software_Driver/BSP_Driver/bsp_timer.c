/*!
 * 文件名称： bsp_timer.c
 * 描    述： 定时器驱动文件
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
#include "bsp_timer.h"

/* 注册定义定时器中断使用的timer */
TIMER_BASE_DEF(TIMER_INT,TIMER5);

/*!
* 说明     定时器中断初始化
* 输入[1]  counter_frequency：计数器频率值
* 输入[2]  period：周期值
* 返回值   无
*/
void bsp_timer_init(uint32_t counter_frequency,uint16_t period)
{
    driver_timer_base_int_init(&TIMER_INT,counter_frequency,period);
    nvic_irq_enable(TIMER5_DAC_UDR_IRQn,0,0);
}

/*!
* 说明     定时器中断服务函数
* 输入[1]  无
* 返回值   无
*/
void TIMER5_DAC_UDR_IRQHandler(void)
{
    driver_timer_updata_int_handler(&TIMER_INT);                     
}


