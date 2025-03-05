/*!
 * �ļ����ƣ� bsp_spi.c
 * ��    ���� bsp spi�����ļ�
 * �汾��     2023-12-03, V1.0
*/

/*
* GD32H757�����ɿ�����V1.0
* ���ֿƼ�������https://www.gd32bbs.com/
* Bվ��Ƶ�̳̣�https://space.bilibili.com/475462605
* CSDN�ĵ��̳̣�https://gd32mcu.blog.csdn.net/
* ���ֿ����幺��https://juwo.taobao.com
* GD��������QQȺ��859440462
* GD�ٷ������塢��Ʒ������ѯQ��87205168  ��ѯV��13905102619
* GD32 MCU������ֲ��������������ѯQ��87205168  ��ѯV��13905102619
* Copyright@���ݾ��ֵ��ӿƼ����޹�˾������ؾ���
*/

#include "bsp_spi.h"


//����BOARD_SPI
SPI_MASTER_SCK_GPIO_DEF(BOARD_SPI,F,7,GPIO_AF_5);
SPI_MASTER_MOSI_GPIO_DEF(BOARD_SPI,F,9,GPIO_AF_5);
SPI_MASTER_MISO_GPIO_DEF(BOARD_SPI,F,8,GPIO_AF_5);
SPI_MASTER_CS_GPIO_DEF(BOARD_SPI,F,6);

SPI_TX_DMA_DEF(BOARD_SPI,SPI4,DMA1,DMA_CH4);
SPI_RX_DMA_DEF(BOARD_SPI,SPI4,DMA1,DMA_CH3);
SPI_DEF(BOARD_SPI,SPI4,SPI_MASTER,SPI_DATASIZE_8BIT,SPI_CK_PL_LOW_PH_1EDGE,SPI_PSC_8,SPI_ENDIAN_MSB,MODE_POLL);

//����TOUCH_SPI
SPI_MASTER_SCK_GPIO_DEF(TOUCH_SPI,B,3,GPIO_AF_5);
SPI_MASTER_MOSI_GPIO_DEF(TOUCH_SPI,B,5,GPIO_AF_5);
SPI_MASTER_MISO_GPIO_DEF(TOUCH_SPI,B,4,GPIO_AF_5);
SPI_MASTER_CS_GPIO_DEF(TOUCH_SPI,A,8);

SPI_TX_DMA_DEF(TOUCH_SPI,SPI0,DMA1,DMA_CH1);
SPI_RX_DMA_DEF(TOUCH_SPI,SPI0,DMA1,DMA_CH0);
SPI_DEF(TOUCH_SPI,SPI0,SPI_MASTER,SPI_DATASIZE_8BIT,SPI_CK_PL_LOW_PH_1EDGE,SPI_PSC_64,SPI_ENDIAN_MSB,MODE_POLL);

const void* SPI_INIT_GROUP[]={&BOARD_SPI,&TOUCH_SPI}; 

#define SPI_INIT_SIZE  sizeof(SPI_INIT_GROUP)/sizeof(void*)
    
//��ӻ�����
spi_device_union board_spi_device=
{
    .spi_device_state=0,
};

//��ӻ�����
spi_device_union touch_spi_device=
{
    .spi_device_state=0,
};
    

/*!
* ˵��     SPI��ʼ������
* ����[1]  spix��spix�ṹ��ָ�� @BOARD_SPI/TOUCH_SPI
* ����ֵ   ��
*/
void bsp_spi_init(typdef_spi_struct *spix)
{
    driver_spi_init(spix);
}

/*!
* ˵��     ��ʼ������ע���spi
* ����[1]  ��
* ����ֵ   ��
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
* ˵��     �ȴ�SPI����
* ����[1]  ��
* ����ֵ   ��
*/
void bsp_spi_device_busy_wait(spi_device_union* spi_device)
{
    while(spi_device->spi_device_state!=0);
}



