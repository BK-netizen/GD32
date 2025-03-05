/*!
 * �ļ����ƣ� bsp_cmp.c
 * ��    ���� bsp cmp interface
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

#include "bsp_cmp.h"
#include "driver_cmp.h"

//CMP�����������Ŷ���
typdef_gpio_general BOARD_CMP_FORWORD_IN_PIN = 
{
		.rcu_port = RCU_GPIOB,
		.port = GPIOB,
		.pin = GPIO_PIN_0,
};

//CMP������Ŷ���
typdef_gpio_general BOARD_CMP_OUTPUT_PIN =  
{
		.rcu_port = RCU_GPIOC,
		.port = GPIOC,
		.pin = GPIO_PIN_5,
	  .gpio_mode = AF_PP,
	  .afio_mode = GPIO_AF_13,
};

//CMP�ṹ�嶨��
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
* ˵��     ����CMP��ʼ������
* ����[1]  cmpx��CMP�ṹ��ָ��
* ����ֵ   ��
*/
void bsp_cmp_init(typdef_cmp_struct *cmpx)
{
    driver_cmp_init(cmpx);
}


/*!
* ˵��     CMP�жϷ�����
* ����     ��
* ����ֵ   ��
*/
void CMP0_1_IRQHandler(void)
{
    driver_cmp_int_handler(&BOARD_CMP);
}
           
