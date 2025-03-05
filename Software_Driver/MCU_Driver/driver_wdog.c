/*!
 * �ļ����ƣ� driver_wdog.c
 * ��    ���� wdog �����ļ�
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
#include "driver_wdog.h"
#include "driver_gpio.h"

/*!
* ˵��     ���Ź����ú���
* ����[1]  reload_value�� ���Ź�����ֵ
* ����[2]  prescaler_div�����Ź�Ԥ��Ƶֵ
* ����ֵ   ��
*/
void drive_fwdgt_config(uint16_t reload_value, uint8_t prescaler_div)
{
    rcu_osci_on(RCU_IRC32K);      											/* �����ڲ�40Kʱ�� */ 
    /* �ȴ�ʱ��ready */
    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC32K)){
    }
		fwdgt_config(reload_value,prescaler_div);						/*���ÿ��Ź�����*/
    fwdgt_enable();																			/*�������Ź�*/
}

/*!
* ˵��     ���Ź�ι������
* ����     ��
* ����ֵ   ��
*/
void driver_feed_fwdgt(void)
{
	fwdgt_counter_reload(); 															/* ��װ�ؼ����� */
}
