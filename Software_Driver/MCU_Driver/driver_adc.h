/*!
 * �ļ����ƣ� driver_adc.h
 * ��    ���� adc �����ļ�
 * �汾��     2023-12-03, V1.0
*/

/*
* GD32H757�����ɿ�����V1.0
* �Ա��̳ǣ�   https://juwo.taobao.com
* ����������   https://www.gd32bbs.com/ask/
* ��Ƶѧϰ��   https://space.bilibili.com/475462605
* ΢�Ź��ںţ� gd32bbs
* Copyright    ���ݾ��ֵ��ӿƼ����޹�˾
* ��Ȩ���У�����ؾ���
*/

#ifndef DRIVER_ADC_H
#define DRIVER_ADC_H
#include "driver_public.h"

/*ADC IO�ڼ�ͨ������*/
typedef struct __typdef_adc_ch_parameter
{
	rcu_periph_enum rcu_port;		//IO��ʱ��
	uint32_t port;							//IO port
	uint32_t pin;								//IO pin
	uint32_t gpio_speed;				//IO ����
	uint8_t  adc_channel;				//IO��Ӧ��ADCͨ��
	uint32_t sample_time;				//IO�Ĳ�������
}typdef_adc_ch_parameter;

typedef struct __typdef_adc_dma_parameter
{
	rcu_periph_enum rcu_dma;		//DMAʱ��
	uint32_t dma_periph;				//DMA��
	dma_channel_enum dma_channel;//DMAͨ����
	uint32_t request;
	uint32_t dma_number;				//DMA�������
	uint32_t dma_priority;			//DMAͨ�����ȼ�
	EventStatus dma_circulation_mode;//ѭ��ģʽ
}typdef_adc_dma_parameter;

typedef struct __typdef_adc_general
{
	rcu_periph_enum rcu_adc;		//ADCʱ�ӿ�
	uint32_t adc_psc;						//ADCʱ��Դ��Ƶϵ��
	uint32_t adc_port;					//ADC��
	uint32_t adc_mode;					//ADC����ģʽ��ADC_MODE_FREE,ADC_DAUL_REGULAL_PARALLEL
	uint8_t adc_channel_group;	//ADC�����飺�������ע����
	EventStatus adc_scan_function;//����ɨ��ģʽ
	EventStatus adc_continuous_function;//����ѭ��ģʽ
	uint32_t adc_external_trigger_mode;
	uint8_t ch_count;						//����ת��ͨ������
	typdef_adc_dma_parameter dma_parameter;//��ʹ��DMA,����Ҫ����dma
	uint32_t trigger_source;			//ADC����Դ
	EventStatus DMA_mode;					//�Ƿ�ʹ��DMA
}typdef_adc_ch_general;

void driver_adc_config(typdef_adc_ch_general *ADC,typdef_adc_ch_parameter *ADC_CH);
void driver_adc_inserted_ch_config(typdef_adc_ch_general *ADC,typdef_adc_ch_parameter *ADC_CH);
void driver_adc_software_trigger_enable(typdef_adc_ch_general *ADC);
void driver_adc_regular_ch_dma_config(typdef_adc_ch_general *ADC, typdef_adc_ch_parameter *ADC_CH,void *buffer);
uint16_t driver_adc_transform_polling(typdef_adc_ch_general *ADC,typdef_adc_ch_parameter *ADC_CH);
void driver_adc_int_handler(typdef_adc_ch_general *ADC,void *buffer);
void driver_adc_dma_int_handler(typdef_adc_ch_general *ADC);
void ADC_int_enable(typdef_adc_ch_general *ADC);
void ADC_dma_int_enable(typdef_adc_ch_general *ADC);
#endif /* DRIVER_ADC_H*/
