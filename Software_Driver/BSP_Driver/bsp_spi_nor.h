/*!
 * 文件名称： bsp_spi_nor.h
 * 描    述： bsp nor驱动文件
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

#ifndef BSP_SPI_NOR_H
#define BSP_SPI_NOR_H

#include "driver_public.h"
#include "bsp_spi.h"
#define  SFLASH_4B_ID       0xC84016
#define  SFLASH_16B_ID       0xC84018
#define  SPI_FLASH_PAGE_SIZE       0x100
#define WRITE            0x02     /* write to memory instruction */
#define WRSR             0x01     /* write status register instruction */
#define WREN             0x06     /* write enable instruction */
#define READ             0x03     /* read from memory instruction */
#define RDSR             0x05     /* read status register instruction  */
#define RDID             0x9F     /* read identification */
#define SE               0x20     /* sector erase instruction */
#define BE               0xC7     /* bulk erase instruction */
#define WIP_FLAG         0x01     /* write in progress(wip)flag */
#define NOR_DUMMY_BYTE       0xA5

/* initialize SPI0 GPIO and parameter */
void bsp_spi_nor_init(void);
/* erase the specified flash sector */
void bsp_spi_nor_sector_erase(uint32_t sector_addr);
/* erase the entire flash */
void bsp_spi_nor_bulk_erase(void);
/* write more than one byte to the flash */
void bsp_spi_nor_page_write(uint8_t* pbuffer,uint32_t write_addr,uint16_t num_byte_to_write);
/* write block of data to the flash */
void bsp_spi_nor_buffer_write(uint8_t* pbuffer,uint32_t write_addr,uint16_t num_byte_to_write);
/* read a block of data from the flash */
void bsp_spi_nor_buffer_read(uint8_t* pbuffer,uint32_t read_addr,uint16_t num_byte_to_read);
/* read flash identification */
uint32_t bsp_spi_nor_read_id(void);
/* initiate a read data byte (read) sequence from the flash */
void bsp_spi_nor_start_read_sequence(uint32_t read_addr);
/* read a byte from the SPI flash */
uint8_t bsp_spi_nor_read_byte(void);
/* send a half word through the SPI interface and return the half word received from the SPI bus */
uint16_t bsp_spi_nor_send_halfword(uint16_t half_word);
/* enable the write access to the flash */
void bsp_spi_nor_write_enable(void);
/* poll the status of the write in progress (wip) flag in the flash's status register */
void bsp_spi_nor_wait_for_write_end(void);

#endif /* BSP_SPI_NOR_H*/
