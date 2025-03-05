/*!
 * 文件名称： bsp_cmp.c
 * 描    述： bsp cmp interface
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

#include "bsp_cmp.h"
#include "driver_cmp.h"

//CMP正向输入引脚定义
typdef_gpio_general BOARD_CMP_FORWORD_IN_PIN = 
{
		.rcu_port = RCU_GPIOB,
		.port = GPIOB,
		.pin = GPIO_PIN_0,
};

//CMP输出引脚定义
typdef_gpio_general BOARD_CMP_OUTPUT_PIN =  
{
		.rcu_port = RCU_GPIOC,
		.port = GPIOC,
		.pin = GPIO_PIN_5,
	  .gpio_mode = AF_PP,
	  .afio_mode = GPIO_AF_13,
};

//CMP结构体定义
typdef_cmp_struct BOARD_CMP=
{
	.rcu_cmpx = RCU_CMP,
	.cmp_x= CMP0,
	.operating_mode = CMP_MODE_HIGHSPEED,
	.inverting_input = CMP_INVERTING_INPUT_VREFINT,
	.output_hysteresis = CMP_HYSTERESIS_NO,
	.VREF_scaler_en = ENABLE,
	.int_en = DISABLE,
	.noninverting_input = CMP_NONINVERTING_INPUT_PB0_PE9,
	.output_polarity = CMP_OUTPUT_POLARITY_NONINVERTED,
	.cmp_forword_input_gpio = &BOARD_CMP_FORWORD_IN_PIN,
	.cmp_reverse_input_gpio = NULL,
	.cmp_output_gpio = &BOARD_CMP_OUTPUT_PIN,
	.cmp_MUX_output_gpio = NULL,
	.cmp_int_callback = NULL,
};

/*!
* 说明     板上CMP初始化函数
* 输入[1]  cmpx：CMP结构体指针
* 返回值   无
*/
void bsp_cmp_init(typdef_cmp_struct *cmpx)
{
    driver_cmp_init(cmpx);
}


/*!
* 说明     CMP中断服务函数
* 输入     无
* 返回值   无
*/
void CMP0_1_IRQHandler(void)
{
    driver_cmp_int_handler(&BOARD_CMP);
}
           
