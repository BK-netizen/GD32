/*!
 * �ļ����ƣ� driver_fmc.c
 * ��    ���� fmc �����ļ�
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

#include "driver_fmc.h"

/*!
* ˵��       ���ڲ�Flashд������
* ����[1]    write_start_addr��д�����ݵ�Flash��ַ  
* ����[2]    data_buf��        д�����ݵ�bufָ��
* ����[3]    data_lengh��      д�����ݵĳ���
* ����ֵ     ��
*/
void fmc_write_data(uint32_t write_start_addr, uint8_t *data_buf, uint16_t data_lengh)
{
		uint32_t write_addr,erase_addr;
		uint16_t data_write_num=0,data_lengh_word=data_lengh/4;
		int32_t data_earse_num;
        uint32_t* pbuff=(uint32_t*)data_buf;
    
        if(data_lengh%4 !=0)
        {
            data_lengh_word++;        
        }
        
        
		fmc_unlock();													/* ����FMC */
		/* ��������־ */
		fmc_flag_clear(FMC_FLAG_WPERR);
		fmc_flag_clear(FMC_FLAG_PGSERR);		
		fmc_flag_clear(FMC_FLAG_RPERR);
		fmc_flag_clear(FMC_FLAG_RSERR);
		fmc_flag_clear(FMC_FLAG_ECCCOR);
        fmc_flag_clear(FMC_FLAG_ECCDET);
        fmc_flag_clear(FMC_FLAG_OBMERR);
        
		erase_addr = write_start_addr;
		data_earse_num = data_lengh;

        if(write_start_addr%FLAG_PAGE_SIZE == 0)  /* ��д���ַΪҳ��ʼ��ַ */
        {
            for(;data_earse_num>0;)
            {
                fmc_sector_erase(erase_addr);
                /* ��������־ */
                fmc_flag_clear(FMC_FLAG_WPERR);
                fmc_flag_clear(FMC_FLAG_PGSERR);		
                fmc_flag_clear(FMC_FLAG_RPERR);
                fmc_flag_clear(FMC_FLAG_RSERR);
                fmc_flag_clear(FMC_FLAG_ECCCOR);
                fmc_flag_clear(FMC_FLAG_ECCDET);
                fmc_flag_clear(FMC_FLAG_OBMERR);
                erase_addr+=FLAG_PAGE_SIZE;
                data_earse_num-=FLAG_PAGE_SIZE;
            }
        }else{
                /*��д���ַ����ҳ��ʼ��ַ*/
                for(;(data_earse_num>0||erase_addr>=write_start_addr+data_lengh);)
                {
                    fmc_sector_erase(erase_addr);
                    /* ��������־ */
                    fmc_flag_clear(FMC_FLAG_WPERR);
                    fmc_flag_clear(FMC_FLAG_PGSERR);		
                    fmc_flag_clear(FMC_FLAG_RPERR);
                    fmc_flag_clear(FMC_FLAG_RSERR);
                    fmc_flag_clear(FMC_FLAG_ECCCOR);
                    fmc_flag_clear(FMC_FLAG_ECCDET);
                    fmc_flag_clear(FMC_FLAG_OBMERR);
                    erase_addr+=FLAG_PAGE_SIZE;
                    data_earse_num-=FLAG_PAGE_SIZE;
                }
        }
		
		/* д������ */
		write_addr = write_start_addr;

		for(data_write_num = 0; data_write_num<data_lengh_word;data_write_num++)
		{
			fmc_word_program(write_addr, pbuff[data_write_num]);
            /* ��������־ */
            fmc_flag_clear(FMC_FLAG_WPERR);
            fmc_flag_clear(FMC_FLAG_PGSERR);		
            fmc_flag_clear(FMC_FLAG_RPERR);
            fmc_flag_clear(FMC_FLAG_RSERR);
            fmc_flag_clear(FMC_FLAG_ECCCOR);
            fmc_flag_clear(FMC_FLAG_ECCDET);
            fmc_flag_clear(FMC_FLAG_OBMERR);
                       
			write_addr+=4;
		}
		fmc_lock();
}

/*!
* ˵��       ��ȡFlash��ַ����
* ����[1]    write_read_addr����Ҫ��ȡ��Flash��ַ
* ����ֵ     ��ȡ������
*/
uint8_t fmc_read_data(uint32_t write_read_addr)
{
		return *(uint8_t *)write_read_addr;
}

