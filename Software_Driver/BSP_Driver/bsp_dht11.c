/*!
 * �ļ����ƣ� bsp_dht11.c
 * ��    ���� dhtll�����ļ�
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

#include "bsp_dht11.h"
#include "driver_gpio.h"

/* DHT11����ע�� */
GPIO_DEF(DHT11_DA,G,6,OUT_OD,SET,NULL); 

/* DHT11��������������� */
#define DHT11_DA_OUT(x)     do{ x ? \
                                driver_gpio_pin_write(&DHT11_DA, SET) : \
                                driver_gpio_pin_write(&DHT11_DA, RESET); \
                            }while(0)                       /* ���ݶ˿���� */

/* DHT11�������������ȡ���� */
#define DHT11_DA_IN         driver_gpio_pin_get(&DHT11_DA)  /* ���ݶ˿����� */

/*!
* ˵��       ��λDHT11
* ����[1]    ��
* ����ֵ     ��
*/
static void dht11_reset(void)
{
    DHT11_DA_OUT(0);    /* ����DQ */
    delay_ms(20);       /* ��������18ms */
    DHT11_DA_OUT(1);    /* DQ=1 */
    delay_us(30);       /* ��������10~35us */
}

/**
 * @brief       �ȴ�DHT11�Ļ�Ӧ
 * @param       ��
 * @retval      0, DHT11����
 *              1, DHT11�쳣/������
 */
/*!
* ˵��       �ȴ�DHT11�Ļ�Ӧ
* ����[1]    ��
* ����ֵ     0, DHT11���� / 1, DHT11�쳣/������
*/
uint8_t dht11_check(void)
{
    uint8_t retry = 0;
    uint8_t rval = 0;
    while (DHT11_DA_IN && retry < 100) 				 /* DHT11������83us */
    {
        retry++;
        delay_us(1);
    }

    if (retry >= 100)
    {
        rval = 1;
    }
    else
    {
        retry = 0;
        while (!DHT11_DA_IN && retry < 100) 	/* DHT11���ͺ���ٴ�����87us */
        {
            retry++;
            delay_us(1);
        }
        if (retry >= 100) rval = 1;
    }
    
    return rval;
}

/*!
* ˵��       ��DHT11��ȡһ��λ
* ����[1]    ��
* ����ֵ     ��ȡ����λֵ: 0 / 1
*/
uint8_t dht11_read_bit(void)
{
    uint8_t retry = 0;

    while (DHT11_DA_IN && retry < 100) 				 /* �ȴ���Ϊ�͵�ƽ */
    {
        retry++;
        delay_us(1);
    }
    retry = 0;
    while (!DHT11_DA_IN && retry < 100)				 /* �ȴ���ߵ�ƽ */
    {
        retry++;
        delay_us(1);
    }
    delay_us(40);      												 /* �ȴ�40us */
    if (DHT11_DA_IN)    										 	/* ��������״̬���� bit */
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}

/*!
* ˵��       ��DHT11��ȡһ���ֽ�
* ����[1]    ��
* ����ֵ     ����������
*/
static uint8_t dht11_read_byte(void)
{
    uint8_t i, data = 0;

    for (i = 0; i < 8; i++)         /* ѭ����ȡ8λ���� */
    {
        data <<= 1;                 /* ��λ���������, ������һλ */
        data |= dht11_read_bit();   /* ��ȡ1bit���� */
    }
    return data;
}

/*!
* ˵��       ��DHT11��ȡһ������
* ����[1]    temp: �¶�ֵ(��Χ:-20~50��)
* ����[2]    humi: ʪ��ֵ(��Χ:5%~95%)
* ����ֵ     0, ����./ 1, ʧ��
*/
uint8_t dht11_read_data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;
    dht11_reset();

    if (dht11_check() == 0)
    {
        for (i = 0; i < 5; i++)     /* ��ȡ40λ���� */
        {
            buf[i] = dht11_read_byte();
        }
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];
        }
    }
    else
    {
        return 1;
    }
    
    return 0;
}

/*!
* ˵��       ��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���
* ����[1]    ��
* ����ֵ     0, ���� / 1, ������/������
*/
uint8_t dht11_init(void)
{
	  driver_gpio_general_init(&DHT11_DA);
    /* DHT11_DQ����ģʽ����,��©���,����, �����Ͳ���������IO������, ��©�����ʱ��(=1), Ҳ���Զ�ȡ�ⲿ�źŵĸߵ͵�ƽ */
    dht11_reset();
    return dht11_check();
}
