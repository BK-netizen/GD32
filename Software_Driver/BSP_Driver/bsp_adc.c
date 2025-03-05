/*!
 * 文件名称： bsp_adc.c
 * 描    述： bsp adc interface
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

#include "driver_adc.h"
#include "bsp_adc.h"

SET_DMA_AREA_RAM uint16_t BSP_ADC_data[2] ;



typdef_adc_ch_general  BSP_ADC= {
	.rcu_adc = RCU_ADC2,															/* ADC2的时钟 */
	.adc_psc = ADC_CLK_SYNC_HCLK_DIV6,								/* ADC2设置为HCLK 6分频 */
	.adc_port = ADC2,																	/* ADC口为ADC2 */
	.adc_mode = ADC_SYNC_MODE_INDEPENDENT,						/* ADC模式为独立模式 */
	.adc_channel_group = ADC_REGULAR_CHANNEL,					/* 使用规则组 */
	.adc_scan_function = ENABLE,											/* 开启扫描模式 */
	.adc_continuous_function = ENABLE,								/* 开启循环模式 */
	.ch_count = 2,																		/* 转换长度为2 */
	.adc_external_trigger_mode = EXTERNAL_TRIGGER_DISABLE,
	.dma_parameter = 
	{
		.rcu_dma = RCU_DMA1,														/* DMA1的时钟 */
		.dma_periph = DMA1,															/* 使用DMA1 */
		.dma_channel = DMA_CH0,													/* 使用通道4 */
		.dma_number = 2,																/* DMA传输长度为2 */
		.request = DMA_REQUEST_ADC2,
		.dma_priority = DMA_PRIORITY_HIGH,							/* DMA通道优先级 */
		.dma_circulation_mode = ENABLE									/* DMA循环模式打开 */
	},
	.DMA_mode = ENABLE																/* 使用DMA */
};



typdef_adc_ch_parameter BSP_ADC_ch[2] = 
{
	{
	.rcu_port = RCU_GPIOC,														/* GPIOC时钟 */
	.port = GPIOC,																		/* GPIO port */
	.pin = GPIO_PIN_2,																/* PC2 */
	.gpio_speed = GPIO_OSPEED_12MHZ,									/* PC2速度设置为12MHz */
	.adc_channel = ADC_CHANNEL_0,											/* PC2是ADC2的通道0 */
	.sample_time = 240																/* 设置采样周期为240 */
	}
	,
	{
	.rcu_port = RCU_GPIOC,														/* GPIOC时钟 */
	.port = GPIOC,																		/* GPIO port */
	.pin = GPIO_PIN_3,																/* PC3 */
	.gpio_speed = GPIO_OSPEED_12MHZ,										/* PC3速度设置为12MHz */
	.adc_channel = ADC_CHANNEL_1,											/* PC3是ADC2的通道1 */
	.sample_time = 240						/* 设置采样周期为55.5 */
	}
};/* ADC通道参数配置，包括IO口，和对应通道以及采样周期 */




/*!
* 说明     bsp ADC配置函数
* 输入     无
* 返回值   无
*/
void bsp_ADC_config()
{
	driver_adc_regular_ch_dma_config(&BSP_ADC,BSP_ADC_ch,(uint16_t*)BSP_ADC_data);
	driver_adc_software_trigger_enable(&BSP_ADC);	
}
