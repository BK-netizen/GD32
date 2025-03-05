/*!
 * �ļ����ƣ� bsp_uart.h
 * ��    ���� bsp uart interface
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
#ifndef BSP_UART_H
#define BSP_UART_H

#include "driver_public.h"
#include "driver_uart.h"

UART_DEF_EXTERN(BOARD_UART);
UART_DEF_EXTERN(MODULE_UART);

void bsp_uart_init(typdef_uart_struct *uartx);
void bsp_uart_group_init(void);
void bsp_rs485_uart_init(void);
void bsp_rs485_uart_transmit(uint8_t *pbuff,uint16_t length);
void bsp_rs485_uart_receive(uint8_t *pbuff,uint16_t length);
#endif /* BSP_UART_H */
