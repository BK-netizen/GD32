/*!
 * �ļ����ƣ� driver_wdog.h
 * ��    ���� wdog �����ļ�
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
#ifndef DRIVER_WDOG_H
#define DRIVER_WDOG_H
#include "driver_public.h"
#include "driver_gpio.h"

void drive_fwdgt_config(uint16_t reload_value, uint8_t prescaler_div);
void driver_feed_fwdgt(void);

#endif /* DRIVER_WDOG_H*/
