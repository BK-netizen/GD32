/*!
 * 文件名称： bsp_spi.c
 * 描    述： bsp spi驱动文件
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

#include "bsp_spi.h"


//定义BOARD_SPI
SPI_MASTER_SCK_GPIO_DEF(BOARD_SPI,F,7,GPIO_AF_5);
SPI_MASTER_MOSI_GPIO_DEF(BOARD_SPI,F,9,GPIO_AF_5);
SPI_MASTER_MISO_GPIO_DEF(BOARD_SPI,F,8,GPIO_AF_5);
SPI_MASTER_CS_GPIO_DEF(BOARD_SPI,F,6);

SPI_TX_DMA_DEF(BOARD_SPI,SPI4,DMA1,DMA_CH4);
SPI_RX_DMA_DEF(BOARD_SPI,SPI4,DMA1,DMA_CH3);
SPI_DEF(BOARD_SPI,SPI4,SPI_MASTER,SPI_DATASIZE_8BIT,SPI_CK_PL_LOW_PH_1EDGE,SPI_PSC_8,SPI_ENDIAN_MSB,MODE_POLL);

//定义TOUCH_SPI
SPI_MASTER_SCK_GPIO_DEF(TOUCH_SPI,B,3,GPIO_AF_5);
SPI_MASTER_MOSI_GPIO_DEF(TOUCH_SPI,B,5,GPIO_AF_5);
SPI_MASTER_MISO_GPIO_DEF(TOUCH_SPI,B,4,GPIO_AF_5);
SPI_MASTER_CS_GPIO_DEF(TOUCH_SPI,A,8);

SPI_TX_DMA_DEF(TOUCH_SPI,SPI0,DMA1,DMA_CH1);
SPI_RX_DMA_DEF(TOUCH_SPI,SPI0,DMA1,DMA_CH0);
SPI_DEF(TOUCH_SPI,SPI0,SPI_MASTER,SPI_DATASIZE_8BIT,SPI_CK_PL_LOW_PH_1EDGE,SPI_PSC_64,SPI_ENDIAN_MSB,MODE_POLL);

const void* SPI_INIT_GROUP[]={&BOARD_SPI,&TOUCH_SPI}; 

#define SPI_INIT_SIZE  sizeof(SPI_INIT_GROUP)/sizeof(void*)
    
//多从机管理
spi_device_union board_spi_device=
{
    .spi_device_state=0,
};

//多从机管理
spi_device_union touch_spi_device=
{
    .spi_device_state=0,
};
    

/*!
* 说明     SPI初始化函数
* 输入[1]  spix：spix结构体指针 @BOARD_SPI/TOUCH_SPI
* 返回值   无
*/
void bsp_spi_init(typdef_spi_struct *spix)
{
    driver_spi_init(spix);
}

/*!
* 说明     初始化所有注册的spi
* 输入[1]  无
* 返回值   无
*/
void bsp_spi_group_init(void)
{
    uint8_t i;
    for(i=0;i<SPI_INIT_SIZE;i++)
    {
        bsp_spi_init(((typdef_spi_struct *)SPI_INIT_GROUP[i]));    
    }
}

/*!
* 说明     等待SPI空闲
* 输入[1]  无
* 返回值   无
*/
void bsp_spi_device_busy_wait(spi_device_union* spi_device)
{
    while(spi_device->spi_device_state!=0);
}



