/*!
 * �ļ����ƣ� bsp_led.c
 * ��    ���� LED interface
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

#include "bsp_led.h"
#include "driver_gpio.h"


/* LED����ע�� */
LED_DEF(LED1,E,3,RESET);      /* PE3����ΪLED1 */
LED_DEF(LED2,C,13,RESET);      /* PC13����ΪLED2 */
LED_DEF(LED3,G,3,RESET);      /* PG3����ΪLED3 */
LED_DEF(LED4,E,2,RESET);      /* PE2����ΪLED4 */

const void* LED_INIT_GROUP[]={&LED1,&LED2,&LED3,&LED4};  

const uint8_t LED_SIZE=sizeof(LED_INIT_GROUP)/sizeof(void*);


/*!
* ˵��     LED��ʼ������
* ����[1]  LEDx��LED�ṹ��ָ�� @LED0/LED1
* ����ֵ   ��
*/
void bsp_led_init(typdef_gpio_general *LEDx)
{
    driver_gpio_general_init(LEDx);
}
/*!
* ˵��     ��ʼ������ע���LED
* ����     ��
* ����ֵ   ��
*/
void bsp_led_group_init(void)
{
    uint8_t i;
    for(i=0;i<LED_SIZE;i++)
    {
        bsp_led_init(((typdef_gpio_general *)LED_INIT_GROUP[i]));    
    }
}


/*!
* ˵��     ����LED����
* ����[1]  LEDx��LED�ṹ��ָ�� @LED0/LED1
* ����ֵ   ��
*/
void bsp_led_on(typdef_gpio_general *LEDx)
{
    driver_gpio_pin_write(LEDx,(bit_status)!(LEDx->default_state));
}

/*!
* ˵��     Ϩ��LED����
* ����[1]  LEDx��LED�ṹ��ָ�� @LED0/LED1
* ����ֵ   ��
*/
void bsp_led_off(typdef_gpio_general *LEDx)
{
    driver_gpio_pin_write(LEDx,LEDx->default_state);
}

/*!
* ˵��     ��תLED����
* ����[1]  LEDx��LED�ṹ��ָ�� @LED0/LED1
* ����ֵ   ��
*/
void bsp_led_toggle(typdef_gpio_general *LEDx)
{
    driver_gpio_pin_toggle(LEDx);
}
