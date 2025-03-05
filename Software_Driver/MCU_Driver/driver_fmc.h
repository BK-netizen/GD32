/*!
 * �ļ����ƣ� driver_fmc.h
 * ��    ���� fmc �����ļ�
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
#ifndef DRIVER_FMC_H
#define DRIVER_FMC_H
#include "driver_public.h"

#define FLAG_PAGE_SIZE 0x1000

void fmc_write_data(uint32_t write_start_addr, uint8_t *data_buf, uint16_t data_lengh);
uint8_t fmc_read_data(uint32_t write_read_addr);

#endif /* DRIVER_FMC_H*/
