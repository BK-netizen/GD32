/*!
 * 文件名称： bsp_dht11.c
 * 描    述： dhtll驱动文件
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

#include "bsp_dht11.h"
#include "driver_gpio.h"

/* DHT11引脚注册 */
GPIO_DEF(DHT11_DA,G,6,OUT_OD,SET,NULL); 

/* DHT11数据引脚输出操作 */
#define DHT11_DA_OUT(x)     do{ x ? \
                                driver_gpio_pin_write(&DHT11_DA, SET) : \
                                driver_gpio_pin_write(&DHT11_DA, RESET); \
                            }while(0)                       /* 数据端口输出 */

/* DHT11数据引脚输入读取操作 */
#define DHT11_DA_IN         driver_gpio_pin_get(&DHT11_DA)  /* 数据端口输入 */

/*!
* 说明       复位DHT11
* 输入[1]    无
* 返回值     无
*/
static void dht11_reset(void)
{
    DHT11_DA_OUT(0);    /* 拉低DQ */
    delay_ms(20);       /* 拉低至少18ms */
    DHT11_DA_OUT(1);    /* DQ=1 */
    delay_us(30);       /* 主机拉高10~35us */
}

/**
 * @brief       等待DHT11的回应
 * @param       无
 * @retval      0, DHT11正常
 *              1, DHT11异常/不存在
 */
/*!
* 说明       等待DHT11的回应
* 输入[1]    无
* 返回值     0, DHT11正常 / 1, DHT11异常/不存在
*/
uint8_t dht11_check(void)
{
    uint8_t retry = 0;
    uint8_t rval = 0;
    while (DHT11_DA_IN && retry < 100) 				 /* DHT11会拉低83us */
    {
        retry++;
        delay_us(1);
    }

    if (retry >= 100)
    {
        rval = 1;
    }
    else
    {
        retry = 0;
        while (!DHT11_DA_IN && retry < 100) 	/* DHT11拉低后会再次拉高87us */
        {
            retry++;
            delay_us(1);
        }
        if (retry >= 100) rval = 1;
    }
    
    return rval;
}

/*!
* 说明       从DHT11读取一个位
* 输入[1]    无
* 返回值     读取到的位值: 0 / 1
*/
uint8_t dht11_read_bit(void)
{
    uint8_t retry = 0;

    while (DHT11_DA_IN && retry < 100) 				 /* 等待变为低电平 */
    {
        retry++;
        delay_us(1);
    }
    retry = 0;
    while (!DHT11_DA_IN && retry < 100)				 /* 等待变高电平 */
    {
        retry++;
        delay_us(1);
    }
    delay_us(40);      												 /* 等待40us */
    if (DHT11_DA_IN)    										 	/* 根据引脚状态返回 bit */
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}

/*!
* 说明       从DHT11读取一个字节
* 输入[1]    无
* 返回值     读到的数据
*/
static uint8_t dht11_read_byte(void)
{
    uint8_t i, data = 0;

    for (i = 0; i < 8; i++)         /* 循环读取8位数据 */
    {
        data <<= 1;                 /* 高位数据先输出, 先左移一位 */
        data |= dht11_read_bit();   /* 读取1bit数据 */
    }
    return data;
}

/*!
* 说明       从DHT11读取一次数据
* 输入[1]    temp: 温度值(范围:-20~50°)
* 输入[2]    humi: 湿度值(范围:5%~95%)
* 返回值     0, 正常./ 1, 失败
*/
uint8_t dht11_read_data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;
    dht11_reset();

    if (dht11_check() == 0)
    {
        for (i = 0; i < 5; i++)     /* 读取40位数据 */
        {
            buf[i] = dht11_read_byte();
        }
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];
        }
    }
    else
    {
        return 1;
    }
    
    return 0;
}

/*!
* 说明       初始化DHT11的IO口 DQ 同时检测DHT11的存在
* 输入[1]    无
* 返回值     0, 正常 / 1, 不存在/不正常
*/
uint8_t dht11_init(void)
{
	  driver_gpio_general_init(&DHT11_DA);
    /* DHT11_DQ引脚模式设置,开漏输出,上拉, 这样就不用再设置IO方向了, 开漏输出的时候(=1), 也可以读取外部信号的高低电平 */
    dht11_reset();
    return dht11_check();
}
