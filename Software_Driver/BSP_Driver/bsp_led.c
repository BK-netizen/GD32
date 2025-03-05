/*!
 * 文件名称： bsp_led.c
 * 描    述： LED interface
 * 版本：     2023-12-03, V1.0
*/

/*
* GD32H757海棠派开发板V1.0
* 聚沃科技官网：https://www.gd32bbs.com/
* B站视频教程：https://space.bilibili.com/475462605
* CSDN文档教程：https://gd32mcu.blog.csdn.net/
* 聚沃开发板购买：https://juwo.taobao.com
* GD技术交流QQ群：859440462
* GD官方开发板、样品购买，咨询Q：87205168  咨询V：13905102619
* GD32 MCU方案移植、方案开发，咨询Q：87205168  咨询V：13905102619
* Copyright@苏州聚沃电子科技有限公司，盗版必究。
*/

#include "bsp_led.h"
#include "driver_gpio.h"


/* LED定义注册 */
LED_DEF(LED1,E,3,RESET);      /* PE3定义为LED1 */
LED_DEF(LED2,C,13,RESET);      /* PC13定义为LED2 */
LED_DEF(LED3,G,3,RESET);      /* PG3定义为LED3 */
LED_DEF(LED4,E,2,RESET);      /* PE2定义为LED4 */

const void* LED_INIT_GROUP[]={&LED1,&LED2,&LED3,&LED4};  

const uint8_t LED_SIZE=sizeof(LED_INIT_GROUP)/sizeof(void*);


/*!
* 说明     LED初始化函数
* 输入[1]  LEDx：LED结构体指针 @LED0/LED1
* 返回值   无
*/
void bsp_led_init(typdef_gpio_general *LEDx)
{
    driver_gpio_general_init(LEDx);
}
/*!
* 说明     初始化所有注册的LED
* 输入     无
* 返回值   无
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
* 说明     点亮LED函数
* 输入[1]  LEDx：LED结构体指针 @LED0/LED1
* 返回值   无
*/
void bsp_led_on(typdef_gpio_general *LEDx)
{
    driver_gpio_pin_write(LEDx,(bit_status)!(LEDx->default_state));
}

/*!
* 说明     熄灭LED函数
* 输入[1]  LEDx：LED结构体指针 @LED0/LED1
* 返回值   无
*/
void bsp_led_off(typdef_gpio_general *LEDx)
{
    driver_gpio_pin_write(LEDx,LEDx->default_state);
}

/*!
* 说明     反转LED函数
* 输入[1]  LEDx：LED结构体指针 @LED0/LED1
* 返回值   无
*/
void bsp_led_toggle(typdef_gpio_general *LEDx)
{
    driver_gpio_pin_toggle(LEDx);
}
