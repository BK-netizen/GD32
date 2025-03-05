/*!
 * �ļ����ƣ� bsp_spi.h
 * ��    ���� bsp spi�����ļ�
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
                               
#ifndef BSP_SPI_H
#define BSP_SPI_H
#include "driver_public.h"
#include "driver_spi.h"
//spi�ṹ�嶨��
typedef union  _spi_device_union {
    uint8_t spi_device_state ;
    struct {
        uint8_t device0:1 ;
        uint8_t device1:1 ;
        uint8_t device2:1 ;
        uint8_t device3:1 ;
        uint8_t device4:1 ;
        uint8_t device5:1 ;
        uint8_t device6:1 ;
        uint8_t device7:1 ;
    }spi_device_state_bits ;
}spi_device_union;

extern spi_device_union board_spi_device;
extern spi_device_union touch_spi_device;

SPI_DEF_EXTERN(BOARD_SPI);
SPI_DEF_EXTERN(TOUCH_SPI);

void bsp_spi_init(typdef_spi_struct *spix);
void bsp_spi_group_init(void);
void bsp_spi_device_busy_wait(spi_device_union* spi_device);
#endif /* BSP_SPI_H*/
