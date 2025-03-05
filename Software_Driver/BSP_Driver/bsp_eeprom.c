/*!
 * 文件名称： bsp_eeprom.c
 * 描    述： eeprom 驱动文件
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

#include "bsp_eeprom.h"
#include "driver_gpio.h"
#include "driver_i2c.h"

/* EEPROM I2C及引脚注册 */
I2C_SCL_GPIO_DEF(EEPROM_I2C,B,10,GPIO_AF_4);
I2C_SDA_GPIO_DEF(EEPROM_I2C,B,11,GPIO_AF_4);
I2C_DEF(EEPROM_I2C,I2C1,100000,0x60,MODE_POLL); 

/*!
* 说明     EEPROM初始化函数
* 输入     无
* 返回值   无
*/
void bsp_eeprom_init_AT24C16(void)
{
	driver_i2c_init(&EEPROM_I2C);
}

/*!
* 说明       向AT24C16 EEPROM写入数据
* 输入[1]    p_buffer: 写入buf数组  @
* 输入[2]		 write_address:写入地址 
* 输入[3]		 number_of_byte：写入长度
* 返回值     EEPROM操作状态
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
    /* 判断写入的地址是否 I2C_PAGE_SIZE 对齐  */
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
* 说明       读取AT24C16 EEPROM数据
* 输入[1]    p_buffer: 读取buf  @
* 输入[2]		 write_address:读取地址 
* 输入[3]		 number_of_byte：读取长度
* 返回值     EEPROM操作状态
*/
EEPROM_STATE eeprom_buffer_read_AT24C16(uint8_t* p_buffer, uint16_t read_address, uint16_t number_of_byte)
{
	uint8_t rNum=0;																	/* 读取的数据长度 */
	uint16_t lenLeft=number_of_byte;								/* 剩余的数据长度 */
  uint8_t deviceId;																/* 读取的器件地址 */
	if(read_address+number_of_byte>EEPROM_SIZE)			/* 如果读取的长度加上读取地址超过了EEPROM的空间大小，则报错误 */
	{
		return EEPROM_ERROR;
	}
		/*calculate the current read position to know how many word can read continully*/
	rNum=16-read_address & 0x0F;
	if(rNum == 0)  rNum=16;
	rNum = lenLeft>=rNum ? rNum : lenLeft;					/* 剩余未读字节数如果大于rNum, 则读rNum个，如果小于rNum，则一次读完了 */
	/*read the data from e2prom*/
	while(lenLeft)
	{
		//这里计算页地址，当地址小于256时，右移8位会小于0，所以器件地址为基地址A1
		//如果读取的地址大于256时，右移8位则不会小于0，所以器件地址为 基地址A1 | 3位页地址
    deviceId=(read_address>>8)>0 ? (EEPROM_ADDR | (uint8_t)((read_address>>7)&0x0E)):EEPROM_ADDR ;

		if(driver_i2c_mem_poll_read(&EEPROM_I2C,deviceId,read_address,MEM_ADDRESS_8BIT,p_buffer,rNum)==DRV_ERROR)
		{
//			printf("i2c read error\r\n");
			  return EEPROM_ERROR;
		}
		read_address+=rNum;													/* 已经读了rNum个了，所以地址后移rNum个 */
		lenLeft-=rNum;															/* 剩余未读数据减少rNum个 */
		p_buffer+=rNum;
		rNum=lenLeft>16? 16 : lenLeft;							/* 如果剩余大于16个，则下次再读16个，如果小于，则一次读完 */
	}
  return EEPROM_SUCCESS;
}

/*!
* 说明       等待EEPROM操作完成
* 输入[1]    i2cx：I2C结构体  @EEPROM_I2C
* 返回值     EEPROM操作状态
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
