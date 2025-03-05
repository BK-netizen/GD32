/*!
 * �ļ����ƣ� bsp_adc.c
 * ��    ���� bsp adc interface
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

#include "driver_adc.h"
#include "bsp_adc.h"

SET_DMA_AREA_RAM uint16_t BSP_ADC_data[2] ;



typdef_adc_ch_general  BSP_ADC= {
	.rcu_adc = RCU_ADC2,															/* ADC2��ʱ�� */
	.adc_psc = ADC_CLK_SYNC_HCLK_DIV6,								/* ADC2����ΪHCLK 6��Ƶ */
	.adc_port = ADC2,																	/* ADC��ΪADC2 */
	.adc_mode = ADC_SYNC_MODE_INDEPENDENT,						/* ADCģʽΪ����ģʽ */
	.adc_channel_group = ADC_REGULAR_CHANNEL,					/* ʹ�ù����� */
	.adc_scan_function = ENABLE,											/* ����ɨ��ģʽ */
	.adc_continuous_function = ENABLE,								/* ����ѭ��ģʽ */
	.ch_count = 2,																		/* ת������Ϊ2 */
	.adc_external_trigger_mode = EXTERNAL_TRIGGER_DISABLE,
	.dma_parameter = 
	{
		.rcu_dma = RCU_DMA1,														/* DMA1��ʱ�� */
		.dma_periph = DMA1,															/* ʹ��DMA1 */
		.dma_channel = DMA_CH0,													/* ʹ��ͨ��4 */
		.dma_number = 2,																/* DMA���䳤��Ϊ2 */
		.request = DMA_REQUEST_ADC2,
		.dma_priority = DMA_PRIORITY_HIGH,							/* DMAͨ�����ȼ� */
		.dma_circulation_mode = ENABLE									/* DMAѭ��ģʽ�� */
	},
	.DMA_mode = ENABLE																/* ʹ��DMA */
};



typdef_adc_ch_parameter BSP_ADC_ch[2] = 
{
	{
	.rcu_port = RCU_GPIOC,														/* GPIOCʱ�� */
	.port = GPIOC,																		/* GPIO port */
	.pin = GPIO_PIN_2,																/* PC2 */
	.gpio_speed = GPIO_OSPEED_12MHZ,									/* PC2�ٶ�����Ϊ12MHz */
	.adc_channel = ADC_CHANNEL_0,											/* PC2��ADC2��ͨ��0 */
	.sample_time = 240																/* ���ò�������Ϊ240 */
	}
	,
	{
	.rcu_port = RCU_GPIOC,														/* GPIOCʱ�� */
	.port = GPIOC,																		/* GPIO port */
	.pin = GPIO_PIN_3,																/* PC3 */
	.gpio_speed = GPIO_OSPEED_12MHZ,										/* PC3�ٶ�����Ϊ12MHz */
	.adc_channel = ADC_CHANNEL_1,											/* PC3��ADC2��ͨ��1 */
	.sample_time = 240						/* ���ò�������Ϊ55.5 */
	}
};/* ADCͨ���������ã�����IO�ڣ��Ͷ�Ӧͨ���Լ��������� */




/*!
* ˵��     bsp ADC���ú���
* ����     ��
* ����ֵ   ��
*/
void bsp_ADC_config()
{
	driver_adc_regular_ch_dma_config(&BSP_ADC,BSP_ADC_ch,(uint16_t*)BSP_ADC_data);
	driver_adc_software_trigger_enable(&BSP_ADC);	
}
