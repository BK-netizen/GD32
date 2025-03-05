/*!
 * 文件名称： bsp_uart.h
 * 描    述： bsp uart interface
 * 版本：     2023-12-03, V1.0
*/

/*
* GD32H757海棠派开发板V1.0
* 淘宝商城：   https://juwo.taobao.com
* 技术交流：   https://www.gd32bbs.com/ask/
* 视频学习：   https://space.bilibili.com/475462605
* 微信公众号： gd32bbs
* Copyright    苏州聚沃电子科技有限公司
* 版权所有，盗版必究。
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
