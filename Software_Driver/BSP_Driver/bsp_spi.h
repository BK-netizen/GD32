/*!
 * 文件名称： bsp_spi.h
 * 描    述： bsp spi驱动文件
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
                               
#ifndef BSP_SPI_H
#define BSP_SPI_H
#include "driver_public.h"
#include "driver_spi.h"
//spi结构体定义
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
