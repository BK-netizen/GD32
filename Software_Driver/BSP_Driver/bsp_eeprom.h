/*!
 * 文件名称： bsp_eeprom.h
 * 描    述： eeprom 驱动文件
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

#ifndef BSP_EEPROM_H
#define BSP_EEPROM_H

#include "driver_i2c.h"

#define EEPROM_ADDR 0xA0
#define I2C_PAGE_SIZE           16
typedef enum {EEPROM_ERROR = 0, EEPROM_SUCCESS = !EEPROM_ERROR} EEPROM_STATE;
#define EEPROM_SIZE 2048

void bsp_eeprom_init_AT24C16(void);
EEPROM_STATE eeprom_buffer_write_AT24C16(uint8_t* p_buffer, uint16_t write_address, uint16_t number_of_byte);
EEPROM_STATE eeprom_buffer_read_AT24C16(uint8_t* p_buffer, uint16_t read_address, uint16_t number_of_byte);
EEPROM_STATE eeprom_wait_standby_state(typdef_i2c_struct *i2cx);
#endif /* BSP_EEPROM_H*/
