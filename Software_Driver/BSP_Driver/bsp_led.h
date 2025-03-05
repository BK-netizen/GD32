/*!
 * 文件名称： bsp_led.h
 * 描    述： LED interface
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
#ifndef BSP_LED_H
#define BSP_LED_H

#include "driver_public.h"
#include "driver_gpio.h"

#define LED_DEF(name, port, pin,default_state)    \
    GPIO_DEF(name,port,pin,OUT_PP,default_state,NULL) 
    
extern const void* LED_INIT_GROUP[];  
extern const uint8_t LED_SIZE;
    
GPIO_DEF_EXTERN(LED1);
GPIO_DEF_EXTERN(LED2);
GPIO_DEF_EXTERN(LED3);
GPIO_DEF_EXTERN(LED4);

void bsp_led_init(  typdef_gpio_general *LEDx);
void bsp_led_group_init(void); 
void bsp_led_on(  typdef_gpio_general *LEDx);
void bsp_led_off(  typdef_gpio_general *LEDx);
void bsp_led_toggle(  typdef_gpio_general *LEDx);
#endif /* BSP_LED_H*/
