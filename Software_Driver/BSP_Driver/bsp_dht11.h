/*!
 * �ļ����ƣ� bsp_dht11.h
 * ��    ���� dhtll�����ļ�
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

#ifndef BSP_DHT11_H
#define BSP_DHT11_H

#include "driver_gpio.h"

uint8_t dht11_init(void);   /* ��ʼ��DHT11 */
uint8_t dht11_check(void);  /* ����Ƿ����DHT11 */
uint8_t dht11_read_data(uint8_t *temp,uint8_t *humi);   /* ��ȡ��ʪ�� */
#endif /* DHT11_H*/
