/*!
 * �ļ����ƣ� driver_cmp.c
 * ��    ���� cmp �����ļ�
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

#include "driver_public.h"
#include "driver_cmp.h"

/*!
* ˵��     CMP��ʼ��
* ����[1]  cmpx:   CMP���ýṹ��ָ��
* ����ֵ   ��
*/
void driver_cmp_init(typdef_cmp_struct *cmpx)
{	 
	  
	  rcu_periph_clock_enable(cmpx->rcu_cmpx);
		/* cmp  configure */
		usart_deinit(cmpx->cmp_x);

	  /* ����������������*/
	  if(cmpx->cmp_forword_input_gpio!=NULL)
		{
				rcu_periph_clock_enable(cmpx->cmp_forword_input_gpio->rcu_port);
			  gpio_mode_set(cmpx->cmp_forword_input_gpio->port,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,cmpx->cmp_forword_input_gpio->pin);
		}
		/* ���÷�����������*/
	  if(cmpx->cmp_reverse_input_gpio!=NULL)
		{
				rcu_periph_clock_enable(cmpx->cmp_forword_input_gpio->rcu_port);
			  gpio_mode_set(cmpx->cmp_forword_input_gpio->port,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,cmpx->cmp_forword_input_gpio->pin);
		}
		/* �����������*/
	  if(cmpx->cmp_output_gpio!=NULL)
		{
				driver_gpio_general_init(cmpx->cmp_output_gpio);
		}
		/* ����MUX�������*/
		if(cmpx->cmp_MUX_output_gpio!=NULL)
		{
				driver_gpio_general_init(cmpx->cmp_MUX_output_gpio);
		}
		
		
	  /* ȥ��ʼ�� CMPX */
    cmp_deinit(cmpx->cmp_x);
    /* ʹ��VREF��ѹ�궨�� */
		if(cmpx->VREF_scaler_en == ENABLE)
		{
				cmp_voltage_scaler_enable(CMP0);
				cmp_scaler_bridge_enable(CMP0);
		}

    /* ����CMPx�������� */
		if(cmpx->noninverting_input!=NULL)
		{
				cmp_noninverting_input_select(cmpx->cmp_x, cmpx->noninverting_input);
		}
		/* ����CMPx */
    cmp_mode_init(cmpx->cmp_x, cmpx->operating_mode, cmpx->inverting_input, cmpx->output_hysteresis);
    /* ����CMPx��� */
		if(cmpx->output_polarity != NULL)
		{
        cmp_output_init(cmpx->cmp_x, cmpx->output_polarity );
		}
		/* ����CMPx�ж� */
		if(cmpx->int_en == ENABLE)
		{
			if(cmpx->cmp_x == CMP0)
			{
					/* ����CMP0�жϣ���ӦEXTI20 */
					exti_flag_clear(EXTI_20);
					exti_init(EXTI_20, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
					exti_interrupt_enable(EXTI_20);
			}else{
					/* ����CMP1�жϣ���ӦEXTI21 */
					exti_flag_clear(EXTI_21);
					exti_init(EXTI_21, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
					exti_interrupt_enable(EXTI_21);
			}
			
					/* ���� CMP NVIC */
					nvic_irq_enable(CMP0_1_IRQn, 0, 0);
		}

    /* ʹ�� CMPX */
    cmp_enable(cmpx->cmp_x);
}

/*!
* ˵��     CMP�жϴ�����������
* ����[1]  cmpx:   CMP���ýṹ��ָ��
* ����ֵ   ��
*/
void driver_cmp_int_handler(typdef_cmp_struct *cmpx)
{
		if(cmpx->cmp_x== CMP0)
		{
				if(RESET != exti_interrupt_flag_get(EXTI_20)) {
						exti_interrupt_flag_clear(EXTI_20);
					
						if(cmpx->cmp_int_callback != NULL)
						{
								cmpx->cmp_int_callback(cmpx);
						}
				}
		}
		if(cmpx->cmp_x==CMP1)
		{
				if(RESET != exti_interrupt_flag_get(EXTI_21)) {
						exti_interrupt_flag_clear(EXTI_21);
					
						if(cmpx->cmp_int_callback != NULL)
						{
								cmpx->cmp_int_callback(cmpx);
						}
				}
	  }
}
    
