/*!
 * �ļ����ƣ� bsp_eeprom.c
 * ��    ���� eeprom �����ļ�
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

#include "bsp_eeprom.h"
#include "driver_gpio.h"
#include "driver_i2c.h"

/* EEPROM I2C������ע�� */
I2C_SCL_GPIO_DEF(EEPROM_I2C,B,10,GPIO_AF_4);
I2C_SDA_GPIO_DEF(EEPROM_I2C,B,11,GPIO_AF_4);
I2C_DEF(EEPROM_I2C,I2C1,100000,0x60,MODE_POLL); 

/*!
* ˵��     EEPROM��ʼ������
* ����     ��
* ����ֵ   ��
*/
void bsp_eeprom_init_AT24C16(void)
{
	driver_i2c_init(&EEPROM_I2C);
}

/*!
* ˵��       ��AT24C16 EEPROMд������
* ����[1]    p_buffer: д��buf����  @
* ����[2]		 write_address:д���ַ 
* ����[3]		 number_of_byte��д�볤��
* ����ֵ     EEPROM����״̬
*/
EEPROM_STATE eeprom_buffer_write_AT24C16(uint8_t* p_buffer, uint16_t write_address, uint16_t number_of_byte)
{
	  uint8_t number_of_page = 0, number_of_single = 0, address = 0, count = 0;
    uint8_t deviceId;
    address = write_address % I2C_PAGE_SIZE;
    count = I2C_PAGE_SIZE - address;
    number_of_page =  number_of_byte / I2C_PAGE_SIZE;
    number_of_single = number_of_byte % I2C_PAGE_SIZE;
    
	  if(write_address+write_address>EEPROM_SIZE)
		{
			return EEPROM_ERROR;
		}
    /* �ж�д��ĵ�ַ�Ƿ� I2C_PAGE_SIZE ����  */
    if(0 == address){
        while(number_of_page--){
					
					  deviceId=(write_address>>8)>0 ? (EEPROM_ADDR | (uint8_t)((write_address>>7)&0x0E)):EEPROM_ADDR ;
            if(driver_i2c_mem_poll_write(&EEPROM_I2C,deviceId,write_address,MEM_ADDRESS_8BIT,p_buffer,I2C_PAGE_SIZE) == DRV_ERROR)
						{
							return EEPROM_ERROR;
						}							
            if(eeprom_wait_standby_state(&EEPROM_I2C) == EEPROM_ERROR)
						{
							return EEPROM_ERROR;
						}
            write_address +=  I2C_PAGE_SIZE;
            p_buffer += I2C_PAGE_SIZE;
        }
        if(0 != number_of_single){
					 deviceId=(write_address>>8)>0 ? (EEPROM_ADDR | (uint8_t)((write_address>>7)&0x0E)):EEPROM_ADDR ;
           if(driver_i2c_mem_poll_write(&EEPROM_I2C,deviceId,write_address,MEM_ADDRESS_8BIT,p_buffer, number_of_single)==DRV_ERROR)
					 {
						 return EEPROM_ERROR;
					 }
           if(eeprom_wait_standby_state(&EEPROM_I2C) == EEPROM_ERROR)
					 {
						 return EEPROM_ERROR;
					 }
        }
        return 	EEPROM_SUCCESS;			
    }else{
        if(number_of_byte < count){ 
					deviceId=(write_address>>8)>0 ? (EEPROM_ADDR | (uint8_t)((write_address>>7)&0x0E)):EEPROM_ADDR ;
					if(driver_i2c_mem_poll_write(&EEPROM_I2C,deviceId,write_address,MEM_ADDRESS_8BIT,p_buffer, number_of_byte)==DRV_ERROR)
					{
						return EEPROM_ERROR;
					}
					if(eeprom_wait_standby_state(&EEPROM_I2C)==EEPROM_ERROR)
					{
						return EEPROM_ERROR;
					}
					
        }else{
            number_of_byte -= count;
            number_of_page =  number_of_byte / I2C_PAGE_SIZE;
            number_of_single = number_of_byte % I2C_PAGE_SIZE;
            
            if(0 != count){
						  	deviceId=(write_address>>8)>0 ? (EEPROM_ADDR | (uint8_t)((write_address>>7)&0x0E)):EEPROM_ADDR ;
					      if(driver_i2c_mem_poll_write(&EEPROM_I2C,deviceId,write_address,MEM_ADDRESS_8BIT,p_buffer, count)==DRV_ERROR)
								{
									return EEPROM_ERROR;
								}
						  	if(eeprom_wait_standby_state(&EEPROM_I2C)==EEPROM_ERROR)
								{
									return EEPROM_ERROR;
								}
                write_address += count;
                p_buffer += count;
            } 
            while(number_of_page--){
						  	deviceId=(write_address>>8)>0 ? (EEPROM_ADDR | (uint8_t)((write_address>>7)&0x0E)):EEPROM_ADDR ;
					      if(driver_i2c_mem_poll_write(&EEPROM_I2C,deviceId,write_address,MEM_ADDRESS_8BIT,p_buffer, I2C_PAGE_SIZE)==DRV_ERROR)
								{
									return EEPROM_ERROR;
								}
							  if(eeprom_wait_standby_state(&EEPROM_I2C)==EEPROM_ERROR)
								{
									return EEPROM_ERROR;
								}
                write_address +=  I2C_PAGE_SIZE;
                p_buffer += I2C_PAGE_SIZE;
            }
            if(0 != number_of_single){
						  	deviceId=(write_address>>8)>0 ? (EEPROM_ADDR | (uint8_t)((write_address>>7)&0x0E)):EEPROM_ADDR ;
					      if(driver_i2c_mem_poll_write(&EEPROM_I2C,deviceId,write_address,MEM_ADDRESS_8BIT,p_buffer, number_of_single)==DRV_ERROR)
								{
									return EEPROM_ERROR;
								}
							  if(eeprom_wait_standby_state(&EEPROM_I2C)==EEPROM_ERROR)
								{
									return EEPROM_ERROR;
								}
            }
        }
				return 	EEPROM_SUCCESS;	
    }  
}

/*!
* ˵��       ��ȡAT24C16 EEPROM����
* ����[1]    p_buffer: ��ȡbuf  @
* ����[2]		 write_address:��ȡ��ַ 
* ����[3]		 number_of_byte����ȡ����
* ����ֵ     EEPROM����״̬
*/
EEPROM_STATE eeprom_buffer_read_AT24C16(uint8_t* p_buffer, uint16_t read_address, uint16_t number_of_byte)
{
	uint8_t rNum=0;																	/* ��ȡ�����ݳ��� */
	uint16_t lenLeft=number_of_byte;								/* ʣ������ݳ��� */
  uint8_t deviceId;																/* ��ȡ��������ַ */
	if(read_address+number_of_byte>EEPROM_SIZE)			/* �����ȡ�ĳ��ȼ��϶�ȡ��ַ������EEPROM�Ŀռ��С���򱨴��� */
	{
		return EEPROM_ERROR;
	}
		/*calculate the current read position to know how many word can read continully*/
	rNum=16-read_address & 0x0F;
	if(rNum == 0)  rNum=16;
	rNum = lenLeft>=rNum ? rNum : lenLeft;					/* ʣ��δ���ֽ����������rNum, ���rNum�������С��rNum����һ�ζ����� */
	/*read the data from e2prom*/
	while(lenLeft)
	{
		//�������ҳ��ַ������ַС��256ʱ������8λ��С��0������������ַΪ����ַA1
		//�����ȡ�ĵ�ַ����256ʱ������8λ�򲻻�С��0������������ַΪ ����ַA1 | 3λҳ��ַ
    deviceId=(read_address>>8)>0 ? (EEPROM_ADDR | (uint8_t)((read_address>>7)&0x0E)):EEPROM_ADDR ;

		if(driver_i2c_mem_poll_read(&EEPROM_I2C,deviceId,read_address,MEM_ADDRESS_8BIT,p_buffer,rNum)==DRV_ERROR)
		{
//			printf("i2c read error\r\n");
			  return EEPROM_ERROR;
		}
		read_address+=rNum;													/* �Ѿ�����rNum���ˣ����Ե�ַ����rNum�� */
		lenLeft-=rNum;															/* ʣ��δ�����ݼ���rNum�� */
		p_buffer+=rNum;
		rNum=lenLeft>16? 16 : lenLeft;							/* ���ʣ�����16�������´��ٶ�16�������С�ڣ���һ�ζ��� */
	}
  return EEPROM_SUCCESS;
}

/*!
* ˵��       �ȴ�EEPROM�������
* ����[1]    i2cx��I2C�ṹ��  @EEPROM_I2C
* ����ֵ     EEPROM����״̬
*/
EEPROM_STATE eeprom_wait_standby_state(typdef_i2c_struct *i2cx)
{
    uint8_t val = 0;
//    Drv_Err i2c_state=DRV_SUCCESS;
    
//    while(1){
//			i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_I2CBSY,RESET);
//			if(i2c_state!=DRV_SUCCESS){
//					driver_i2c_stop(i2cx);
//					return EEPROM_ERROR;
//			}
//			i2c_start_on_bus(i2cx->i2c_x);
//			i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_SBSEND,SET);
//			if(i2c_state!=DRV_SUCCESS){
//					driver_i2c_stop(i2cx);
//					return EEPROM_ERROR;
//			}
//			i2c_master_addressing(i2cx->i2c_x, EEPROM_ADDR, I2C_TRANSMITTER);
//			do{
//					val = I2C_STAT0(i2cx->i2c_x);
//			}while(0 == (val & (I2C_STAT0_ADDSEND | I2C_STAT0_AERR)));
//					 
//			if(val & I2C_STAT0_ADDSEND){
//				i2c_flag_clear(i2cx->i2c_x,I2C_FLAG_ADDSEND);
//				driver_i2c_stop(i2cx);
//				return EEPROM_SUCCESS;
//			}else{
//				i2c_flag_clear(i2cx->i2c_x,I2C_FLAG_AERR);
//			}
//					
//			driver_i2c_stop(i2cx);
//        }
    while(1){
        if(DRV_SUCCESS==driver_i2c_mem_poll_read(i2cx,EEPROM_ADDR,0,MEM_ADDRESS_8BIT,&val,1))
            return EEPROM_SUCCESS;
    }
}
