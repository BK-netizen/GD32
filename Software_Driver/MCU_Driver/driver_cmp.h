/*!
 * �ļ����ƣ� driver_cmp.h
 * ��    ���� cmp �����ļ�
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

#ifndef DRIVER_CMP_H
#define DRIVER_CMP_H
#include "driver_public.h"

typedef struct __typdef_cmp_struct
{
	rcu_periph_enum rcu_cmpx;						//CMPʱ�ӿ�
	uint32_t cmp_x; 										//�Ƚ�������
	uint32_t operating_mode;						//�Ƚ�������ģʽ
	uint32_t inverting_input;						//��������ѡ��
	uint32_t output_hysteresis;					//�����������
	uint32_t VREF_scaler_en;						//�ο���ѹ�궨��ʹ�ܿ��ƣ�ENABLE/DISABLE
	uint32_t noninverting_input;				//������������
	uint32_t output_polarity;						//�����������
	uint32_t int_en;                    //�ж�ʹ�ܿ���
	
  typdef_gpio_general* cmp_forword_input_gpio;  //�����������Žṹ��
  typdef_gpio_general* cmp_reverse_input_gpio;  //�����������Žṹ��
  typdef_gpio_general* cmp_output_gpio;					//������Žṹ��
  typdef_gpio_general* cmp_MUX_output_gpio;			//MUX������Žṹ��
	
  void (*cmp_int_callback)(struct __typdef_cmp_struct *cmpx);  //�Ƚ����жϻص�����
	
}typdef_cmp_struct;
   				
void driver_cmp_init(typdef_cmp_struct *cmpx);
void driver_cmp_int_handler(typdef_cmp_struct *cmpx);
#endif /* DRIVER_CMP_H*/
