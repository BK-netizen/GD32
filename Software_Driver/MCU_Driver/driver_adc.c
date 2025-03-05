/*!
 * �ļ����ƣ� driver_adc.c
 * ��    ���� adc �����ļ�
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
#include "driver_adc.h"

/*!
* ˵��     ADC��ʼ��������ADCʱ��ʹ�ܺ͹�������
* ����[1]  ADC��   ADC���ýṹ��ָ��
* ����[2]  ADC_CH��ADCͨ�����ýṹ��ָ��
* ����ֵ   ��
*/
void driver_adc_config(typdef_adc_ch_general *ADC,typdef_adc_ch_parameter *ADC_CH)
{	 
	uint8_t i;
    adc_idx_enum idx_adc;
	adc_deinit(ADC->adc_port);
    /* ADC clock config */
    
    if(ADC->adc_port==ADC0){
        idx_adc=IDX_ADC0;    
    }else if(ADC->adc_port==ADC1){
        idx_adc=IDX_ADC1;    
    }else{
        idx_adc=IDX_ADC2;    
    }   
    rcu_adc_clock_config(idx_adc, RCU_ADCSRC_PER);    
    
	adc_clock_config(ADC->adc_port, ADC->adc_psc); 					/*����ADCʱ��Ƶ��*/
	rcu_periph_clock_enable(ADC->rcu_adc); 				/*ʹ��ADCʱ��*/
    
//    /* ADC clock config */
//    rcu_adc_clock_config(IDX_ADC2, RCU_ADCSRC_PER);
//    adc_clock_config(ADC2, ADC_CLK_ASYNC_DIV64);    
    
	/*����ADC���IO�ڣ�������ʱ�ӣ��ٽ�IO������Ϊģ������*/
	for(i=0 ;i<ADC->ch_count; i++)
	{
		if(ADC_CH[i].adc_channel < ADC_CHANNEL_17)
		{
			rcu_periph_clock_enable(ADC_CH[i].rcu_port);
			gpio_mode_set(ADC_CH[i].port, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_CH[i].pin);					
		}
		else
		{
			if(ADC->adc_port==ADC2)
			{
				if(ADC_CH[i].adc_channel == ADC_CHANNEL_17)
				{
					adc_internal_channel_config(ADC_CHANNEL_INTERNAL_VBAT, ENABLE);
				}
				else if(ADC_CH[i].adc_channel == ADC_CHANNEL_18)
				{
					adc_internal_channel_config(ADC_CHANNEL_INTERNAL_TEMPSENSOR, ENABLE);
				}
				else if(ADC_CH[i].adc_channel == ADC_CHANNEL_19)
				{
					adc_internal_channel_config(ADC_CHANNEL_INTERNAL_VREFINT, ENABLE);
				}
				else if(ADC_CH[i].adc_channel == ADC_CHANNEL_20)
				{
					adc_internal_channel_config(ADC_CHANNEL_INTERNAL_HP_TEMPSENSOR, ENABLE);
				}				
			}
			else
			{
				rcu_periph_clock_enable(ADC_CH[i].rcu_port);
				gpio_mode_set(ADC_CH[i].port, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_CH[i].pin);					
			}
		}		

	}
	adc_sync_mode_config(ADC->adc_mode);								/*����ADC����ģʽ�������ģʽ��������ģʽ��*/
	adc_special_function_config(ADC->adc_port, ADC_SCAN_MODE, ADC->adc_scan_function);	/*���ù������ɨ��ģʽ������ת��ģʽ*/
	if(ADC->adc_channel_group == ADC_REGULAR_CHANNEL)
	{
		adc_special_function_config(ADC->adc_port, ADC_CONTINUOUS_MODE, ADC->adc_continuous_function);	
	}
	adc_data_alignment_config(ADC->adc_port, ADC_DATAALIGN_RIGHT);											/*ѡ�������Ҷ���*/
	adc_channel_length_config(ADC->adc_port, ADC->adc_channel_group, ADC->ch_count);		/*����ת��ͨ����*/
	if(ADC->adc_channel_group == ADC_REGULAR_CHANNEL)																		/*����ת��˳��*/
	{
		for(i = 0;i< ADC->ch_count;i++)
		{
			adc_regular_channel_config(ADC->adc_port, i, ADC_CH[i].adc_channel,ADC_CH[i].sample_time);
		}
	}
	else if(ADC->adc_channel_group == ADC_INSERTED_CHANNEL)
	{
		for(i = 0;i< ADC->ch_count;i++)
		{
			adc_inserted_channel_config(ADC->adc_port, i, ADC_CH[i].adc_channel,ADC_CH[i].sample_time);
		}	
	}
	/*ѡ�񴥷�Դ��ʹ���ⲿ����ģʽ*/
	adc_external_trigger_config(ADC->adc_port, ADC->adc_channel_group, ADC->adc_external_trigger_mode);
	/*ѡ���Ƿ���Ҫʹ��DMA*/
	if(ADC->DMA_mode == ENABLE)
	{
		adc_dma_request_after_last_enable(ADC->adc_port);
		adc_dma_mode_enable(ADC->adc_port);
	}
	 /*ADC��ʹ�ܺ���У׼��ADCʹ�ܺ���Ҫ����һ����ADC_CLK�����У׼����ʾ����ֱ��ʹ��1ms��ʱ*/
	adc_enable(ADC->adc_port);
	delay_ms(1);
	/* ADC calibration mode config */
	adc_calibration_mode_config(ADC->adc_port, ADC_CALIBRATION_OFFSET_MISMATCH);
	/* ADC calibration number config */
	adc_calibration_number(ADC->adc_port, ADC_CALIBRATION_NUM32);
	adc_calibration_enable(ADC->adc_port);
}


/*!
* ˵��     �������ADCת������
* ����     ADC��ADC���ýṹ��ָ��
* ����ֵ   ��
*/
void driver_adc_software_trigger_enable(typdef_adc_ch_general *ADC)
{
    adc_software_trigger_enable(ADC->adc_port, ADC->adc_channel_group);
}


/*!
* ˵��     ��DMA���ܵ�ADC��ʼ��������ADCʱ��ʹ�ܺ͹�������
* ����[1]  ADC��   ADC���ýṹ��ָ��
* ����[2]  ADC_CH��ADCͨ�����ýṹ��ָ��
* ����[3]  buffer��ADC���ݻ���buffer
* ����ֵ   ��
*/
void driver_adc_regular_ch_dma_config(typdef_adc_ch_general *ADC, typdef_adc_ch_parameter *ADC_CH,void *buffer)
{	
	dma_single_data_parameter_struct dma_single_data_parameter;
	rcu_periph_clock_enable(ADC->dma_parameter.rcu_dma);													/*DMAʱ�ӿ���*/
	rcu_periph_clock_enable(RCU_DMAMUX);	
	dma_deinit(ADC->dma_parameter.dma_periph, ADC->dma_parameter.dma_channel);		/*DMAͨ��������λ*/
	/*DMAԴ��ַ��Ŀ���ַ��������ʽ������λ�����䷽�򡢴�����������ȼ�����*/
	dma_single_data_parameter.request = ADC->dma_parameter.request;
	dma_single_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA(ADC->adc_port));
	dma_single_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
	dma_single_data_parameter.memory0_addr  = (uint32_t)(buffer);
	dma_single_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
	if(ADC->adc_mode == ADC_DAUL_REGULAL_PARALLEL)
	{
		dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
	}
	else
	{
		dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
	}
	dma_single_data_parameter.direction    = DMA_PERIPH_TO_MEMORY;
	dma_single_data_parameter.number       = ADC->dma_parameter.dma_number;
	dma_single_data_parameter.priority     = ADC->dma_parameter.dma_priority;
	dma_single_data_mode_init(ADC->dma_parameter.dma_periph, ADC->dma_parameter.dma_channel, &dma_single_data_parameter);
	/*DMAѭ��ģʽ����*/
	if(ADC->dma_parameter.dma_circulation_mode == ENABLE)
	{
		dma_circulation_enable(ADC->dma_parameter.dma_periph, ADC->dma_parameter.dma_channel);
	}
	else
	{
		dma_circulation_disable(ADC->dma_parameter.dma_periph, ADC->dma_parameter.dma_channel);
	}
	dma_channel_enable(ADC->dma_parameter.dma_periph, ADC->dma_parameter.dma_channel);	/* ʹ��DMA */
	driver_adc_config(ADC,ADC_CH);																											/* ADC��ʼ�� */
}


/*!
* ˵��     ��ѵ��ʽ��ȡADCֵ
* ����[1]  ADC��   ADC���ýṹ��ָ��
* ����[2]  ADC_CH��ADCͨ�����ýṹ��ָ��
* ����ֵ   ��
*/
uint16_t driver_adc_transform_polling(typdef_adc_ch_general *ADC,typdef_adc_ch_parameter *ADC_CH)
{
	/*���������*/
	if(ADC->adc_channel_group == ADC_REGULAR_CHANNEL)
	{
		adc_regular_channel_config(ADC->adc_port, 0, ADC_CH->adc_channel, ADC_CH->sample_time);			/*���ù�������Ҫ������ͨ��*/
		adc_software_trigger_enable(ADC->adc_port, ADC_REGULAR_CHANNEL);														/*�������������ת��*/
		while(RESET == adc_flag_get(ADC->adc_port,ADC_FLAG_EOC));																		/*�ȴ�EOC����*/
		adc_flag_clear(ADC->adc_port,ADC_FLAG_EOC);																									/*���EOC��־λ*/
		return ADC_RDATA(ADC->adc_port);																														/*��������ת�������Ϊ����ֵ*/
		
	}
	/*ע�������*/
	else if(ADC->adc_channel_group == ADC_INSERTED_CHANNEL)
	{
		adc_inserted_channel_config(ADC->adc_port, 0, ADC_CH->adc_channel, ADC_CH->sample_time);		/*����ע������Ҫ������ͨ��*/
		adc_software_trigger_enable(ADC->adc_port, ADC_INSERTED_CHANNEL);														/*�������ע����ת��*/
		while(RESET == adc_flag_get(ADC->adc_port,ADC_FLAG_EOIC));																	/*�ȴ�EOIC���� */
		adc_flag_clear(ADC->adc_port,ADC_FLAG_EOIC);																								/*���EOIC��־λ*/
		return ADC_IDATA0(ADC->adc_port);																														/*��ע����ת�������Ϊ����ֵ*/
	}
	return 0;
}

/*!
* ˵��     ADC�жϴ�����
* ����[1]  ADC��   ADC���ýṹ��ָ��
* ����[2]  buffer��ADC���ݻ���buffer
* ����ֵ   ��
*/

void driver_adc_int_handler(typdef_adc_ch_general *ADC,void *buffer)
{
	uint8_t i;
  uint32_t buff_addres=(uint32_t)(buffer);
	if(ADC->adc_channel_group == ADC_REGULAR_CHANNEL)
	{
		if(SET == adc_interrupt_flag_get(ADC->adc_port,ADC_INT_FLAG_EOC))
		{
			adc_interrupt_flag_clear(ADC->adc_port,ADC_INT_FLAG_EOC);
			if(ADC->adc_mode == ADC_DAUL_REGULAL_PARALLEL)
			{
				REG32(buffer) = (uint32_t)(ADC_RDATA(ADC->adc_port));
			}
			else
			{
				REG16(buffer) = (uint16_t)(ADC_RDATA(ADC->adc_port));
			}
			
		}
	}
	else if(ADC->adc_channel_group == ADC_INSERTED_CHANNEL)
	{
		if(SET == adc_interrupt_flag_get(ADC->adc_port,ADC_INT_FLAG_EOIC))
		{
			adc_interrupt_flag_clear(ADC->adc_port,ADC_INT_FLAG_EOIC);
			if(ADC->adc_mode == ADC_DAUL_INSERTED_PARALLEL)
			{
				for(i = 0; i<ADC->ch_count ; i++)
				{
					REG32(buff_addres) = REG32((ADC->adc_port) + 0x3C+(i*4));
					buff_addres += 4;
				}
			}
			else
			{
				for(i = 0; i<ADC->ch_count ; i++)
				{
					REG16(buff_addres) = REG32((ADC->adc_port) + 0x3C+(i*4));
					buff_addres += 2;
				}
			}
		}
	}
}

/*!
* ˵��     ADC DMA�жϴ�����
* ����[1]  ADC��ADC���ýṹ��ָ��
* ����ֵ   ��
*/
void driver_adc_dma_int_handler(typdef_adc_ch_general *ADC)
{
	if(SET == dma_interrupt_flag_get(ADC->dma_parameter.dma_periph,ADC->dma_parameter.dma_channel,DMA_INT_FLAG_FTF))
	{
		dma_interrupt_flag_clear(ADC->dma_parameter.dma_periph,ADC->dma_parameter.dma_channel,DMA_INT_FLAG_FTF);
	}
}


/*!
* ˵��     ADC�ж�ʹ�ܺ���
* ����[1]  ADC��   ADC���ýṹ��ָ��
* ����ֵ   ��
*/
void ADC_int_enable(typdef_adc_ch_general *ADC)
{
	/*�������ж�ʹ��*/
	if(ADC->adc_channel_group == ADC_REGULAR_CHANNEL)
	{
		adc_interrupt_enable(ADC->adc_port,ADC_INT_EOC);
		adc_interrupt_flag_clear(ADC->adc_port,ADC_INT_EOC);
	}
	/*ע�����ж�ʹ��*/
	else if(ADC->adc_channel_group == ADC_INSERTED_CHANNEL)
	{
		adc_interrupt_enable(ADC->adc_port,ADC_INT_EOIC);
		adc_interrupt_flag_clear(ADC->adc_port,ADC_INT_EOIC);
	}	
}


/*!
* ˵��     ADC DMAʹ�ܺ���
* ����[1]  ADC��   ADC���ýṹ��ָ��
* ����ֵ   ��
*/
void ADC_dma_int_enable(typdef_adc_ch_general *ADC)
{
	dma_interrupt_enable(ADC->dma_parameter.dma_periph,ADC->dma_parameter.dma_channel,DMA_CHXCTL_FTFIE);	
}

