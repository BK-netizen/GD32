/*!
 * �ļ����ƣ� driver_exmc.h
 * ��    ���� exmc �����ļ�
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

#ifndef DRIVER_EXMC_H
#define DRIVER_EXMC_H
#include "driver_public.h"

#define EXMC_BANK0_NORSRAM_REGIONx_ADDR(NEx)       ((uint32_t)0x60000000+64*1024*1024*NEx)

#define EXMC_BANK0_NORSRAM_REGION0_ADDR            EXMC_BANK0_NORSRAM_REGIONx_ADDR(0)
#define EXMC_BANK0_NORSRAM_REGION1_ADDR            EXMC_BANK0_NORSRAM_REGIONx_ADDR(1)
#define EXMC_BANK0_NORSRAM_REGION2_ADDR            EXMC_BANK0_NORSRAM_REGIONx_ADDR(2)
#define EXMC_BANK0_NORSRAM_REGION3_ADDR            EXMC_BANK0_NORSRAM_REGIONx_ADDR(3)

void driver_exmc_norsram_init(uint32_t norsram_region);
void driver_exmc_lcd_init(uint32_t norsram_region);
Drv_Err driver_exmc_sdram_init(uint32_t sdram_device);

    

#endif /* GPIO_H*/
