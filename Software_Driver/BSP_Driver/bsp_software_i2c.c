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
#include <stdio.h>
#include "bsp_software_i2c.h"
#include "driver_gpio.h"

/************************������ֲ�޸���************************************/
SOFT_I2C_SCL_DEF(CAMERA_IIC, D, 6); 
SOFT_I2C_SDA_DEF(CAMERA_IIC, G,14); 
SOFT_I2C_DEF(CAMERA_IIC,100000);


SOFT_I2C_SCL_DEF(TOUCH_CAP_IIC, A, 8); 
SOFT_I2C_SDA_DEF(TOUCH_CAP_IIC, B, 4); 
SOFT_I2C_DEF(TOUCH_CAP_IIC,400000);

/**************************************************************************/

static void SDA_IN(typdef_software_i2c* soft_i2c)  
{
    soft_i2c->iic_sda->gpio_mode=IN_PU; 
    driver_gpio_general_init(soft_i2c->iic_sda);
}

static void SDA_OUT_PP(typdef_software_i2c* soft_i2c,bit_status bit_value)  
{
    soft_i2c->iic_sda->gpio_mode=OUT_PP; 
    soft_i2c->iic_sda->default_state=bit_value;    
    driver_gpio_general_init(soft_i2c->iic_sda);
    soft_i2c->iic_sda->default_state=SET; 
}

static void SDA_OUT_OD(typdef_software_i2c* soft_i2c,bit_status bit_value)  
{
    soft_i2c->iic_sda->gpio_mode=OUT_OD; 
    soft_i2c->iic_sda->default_state=bit_value;    
    driver_gpio_general_init(soft_i2c->iic_sda);
    soft_i2c->iic_sda->default_state=SET;  
}

//IO��������	 
static void IIC_SCL_H(typdef_software_i2c* soft_i2c)    
{
    driver_gpio_pin_set(soft_i2c->iic_scl); //SCL
}

//IO��������	 
static void IIC_SDA_H(typdef_software_i2c* soft_i2c)    
{
    driver_gpio_pin_set(soft_i2c->iic_sda); //SCL
}

//IO��������	 
static void IIC_SDA_L(typdef_software_i2c* soft_i2c)    
{
    driver_gpio_pin_reset(soft_i2c->iic_sda); //SCL
}

//IO��������	 
static void IIC_SCL_L(typdef_software_i2c* soft_i2c)    
{
    driver_gpio_pin_reset(soft_i2c->iic_scl); //SCL
}

//IO��������	 
static bit_status READ_SDA(typdef_software_i2c* soft_i2c)    
{
    return driver_gpio_pin_get(soft_i2c->iic_sda); //SCL
}


/******************************************************************************
*��  ����void bsp_software_i2c_delay(soft_i2c,void)
*�����ܣ�IIC��ʱ
*��  ������
*����ֵ����
*��  ע: ��ֲʱֻ��Ҫ��bsp_software_i2c_delay(soft_i2c,)�����Լ�����ʱ����
*******************************************************************************/	

void bsp_software_i2c_delay(typdef_software_i2c* soft_i2c,uint8_t delay)
{   
    while(delay--)
    {
        delay_us(soft_i2c->delay_us);
    }
}

/******************************************************************************
*��  ����void bsp_software_i2c_init(void)
*�����ܣ�IIC��ʼ��
*��  ������
*����ֵ����
*��  ע����
*******************************************************************************/	
void bsp_software_i2c_init(typdef_software_i2c* soft_i2c)
{			
    driver_gpio_general_init(soft_i2c->iic_sda);
    driver_gpio_general_init(soft_i2c->iic_scl);    
}
/******************************************************************************
*��  ����void bsp_software_i2c_start(void)
*�����ܣ�����IIC��ʼ�ź�
*��  ������
*����ֵ����
*��  ע����
*******************************************************************************/	
void bsp_software_i2c_start(typdef_software_i2c* soft_i2c)
{
	SDA_OUT_PP(soft_i2c,SET); //sda����� 
	IIC_SDA_H(soft_i2c);	
	IIC_SCL_H(soft_i2c);
	bsp_software_i2c_delay(soft_i2c,1);
 	IIC_SDA_L(soft_i2c); //START:when CLK is high,DATA change form high to low 
	bsp_software_i2c_delay(soft_i2c,1);
	IIC_SCL_L(soft_i2c); //ǯסI2C���ߣ�׼�����ͻ�������� 
	bsp_software_i2c_delay(soft_i2c,1);    
}

/******************************************************************************
*��  ����void bsp_software_i2c_stop(void)
*�����ܣ�����IICֹͣ�ź�
*��  ������
*����ֵ����
*��  ע����
*******************************************************************************/	  
void bsp_software_i2c_stop(typdef_software_i2c* soft_i2c)
{
	IIC_SCL_L(soft_i2c);
	SDA_OUT_PP(soft_i2c,RESET); //sda�����    
	IIC_SDA_L(soft_i2c); //STOP:when CLK is high DATA change form low to high
  	bsp_software_i2c_delay(soft_i2c,1);
	IIC_SCL_H(soft_i2c); 
  	bsp_software_i2c_delay(soft_i2c,1);    
	IIC_SDA_H(soft_i2c); //����I2C���߽����ź�
    bsp_software_i2c_delay(soft_i2c,1);							   	
}

/******************************************************************************
*��  ��: uint8_t bsp_software_i2c_wait_ack(void)
*������: �ȴ�Ӧ���źŵ��� ����ЧӦ�𣺴ӻ���9�� SCL=0 ʱ SDA ���ӻ�����,
                            ���� SCL = 1ʱ SDA��ȻΪ�ͣ�
*��  ������
*����ֵ��1������Ӧ��ʧ��
         0������Ӧ��ɹ�
*��  ע���ӻ���������Ӧ��
*******************************************************************************/
uint8_t bsp_software_i2c_wait_ack(typdef_software_i2c* soft_i2c)
{
	uint8_t ucErrTime=0;
//	IIC_SCL_L(soft_i2c); //ʱ�����0 	     
//	SDA_IN(soft_i2c); //SDA����Ϊ����  ���ӻ���һ���͵�ƽ��ΪӦ��     
	IIC_SCL_L(soft_i2c); //ʱ�����0 	         
    bsp_software_i2c_delay(soft_i2c,1);	    
	IIC_SDA_H(soft_i2c);    
	while(READ_SDA(soft_i2c))
	{        
		ucErrTime++;
		if(ucErrTime>5)
		{
            IIC_SCL_H(soft_i2c);
            bsp_software_i2c_delay(soft_i2c,1);	
			bsp_software_i2c_stop(soft_i2c);
			return 1;
		}
        bsp_software_i2c_delay(soft_i2c,1);
	}
    IIC_SCL_H(soft_i2c);
    bsp_software_i2c_delay(soft_i2c,1);    
	IIC_SCL_L(soft_i2c); //ʱ�����0 	   
	return 0;  
} 

/******************************************************************************
*��  ��: void bsp_software_i2c_ack(void)
*������: ����ACKӦ�� ������������һ���ֽ����ݺ�����������ACK֪ͨ�ӻ�һ��
                       �ֽ���������ȷ���գ�
*��  ������
*����ֵ����
*��  ע���������ӻ���Ӧ��
*******************************************************************************/

void bsp_software_i2c_ack(typdef_software_i2c* soft_i2c)
{
	IIC_SCL_L(soft_i2c);
	SDA_OUT_PP(soft_i2c,RESET);
	IIC_SDA_L(soft_i2c);
	bsp_software_i2c_delay(soft_i2c,1);
	IIC_SCL_H(soft_i2c);
	bsp_software_i2c_delay(soft_i2c,1);
	IIC_SCL_L(soft_i2c);
}

/******************************************************************************
*��  ��: void bsp_software_i2c_nack(void)
*������: ����NACKӦ�� ���������������һ���ֽ����ݺ�����������NACK֪ͨ�ӻ�
                        ���ͽ������ͷ�SDA,�Ա���������ֹͣ�źţ�
*��  ������
*����ֵ����
*��  ע���������ӻ���Ӧ��
*******************************************************************************/
void bsp_software_i2c_nack(typdef_software_i2c* soft_i2c)
{
	IIC_SCL_L(soft_i2c);
	SDA_OUT_PP(soft_i2c,RESET);
	IIC_SDA_H(soft_i2c);
	bsp_software_i2c_delay(soft_i2c,1);
	IIC_SCL_H(soft_i2c);
	bsp_software_i2c_delay(soft_i2c,1);
	IIC_SCL_L(soft_i2c);
}					 				     

/******************************************************************************
*��  ����void bsp_software_i2c_send_byte(soft_i2c,uint8_t txd)
*��  �ܣ�IIC����һ���ֽ�
*��  ����data Ҫд������
*����ֵ����
*��  ע���������ӻ���
*******************************************************************************/		  
void bsp_software_i2c_send_byte(typdef_software_i2c* soft_i2c,uint8_t data)
{                        
    uint8_t t;   
    
    SDA_OUT_PP(soft_i2c,(bit_status)((data&0x80)>>7)); 
    
    for(t=0;t<8;t++)
    {   
        IIC_SCL_L(soft_i2c);
        if(t==7)
        {
            SDA_OUT_OD(soft_i2c,(bit_status)((data&0x80)>>7));         
        }
        if((data&0x80)>>7)
            IIC_SDA_H(soft_i2c);
        else
            IIC_SDA_L(soft_i2c);
        data<<=1;	
        bsp_software_i2c_delay(soft_i2c,1);        
        IIC_SCL_H(soft_i2c);
        bsp_software_i2c_delay(soft_i2c,1);	   
    }   
} 	 
   
/******************************************************************************
*��  ����uint8_t bsp_software_i2c_read_byte(soft_i2c,uint8_t ack)
*��  �ܣ�IIC��ȡһ���ֽ�
*��  ����ack=1 ʱ���������ݻ�û������ ack=0 ʱ����������ȫ���������
*����ֵ����
*��  ע���ӻ���������
*******************************************************************************/	
uint8_t bsp_software_i2c_read_byte(typdef_software_i2c* soft_i2c,uint8_t ack)
{
	uint8_t i,receive=0;
	SDA_IN(soft_i2c); //SDA����Ϊ����ģʽ �ȴ����մӻ���������
    for(i=0;i<8;i++ )
    {
        IIC_SCL_L(soft_i2c); 
        bsp_software_i2c_delay(soft_i2c,1);
        IIC_SCL_H(soft_i2c);
        receive<<=1;
        if(READ_SDA(soft_i2c)) receive++; //�ӻ����͵ĵ�ƽ
        bsp_software_i2c_delay(soft_i2c,1); 
    }					 
    if(ack)
        bsp_software_i2c_ack(soft_i2c); //����ACK 
    else
        bsp_software_i2c_nack(soft_i2c); //����nACK  
    return receive;
}

/******************************************************************************
*��  ����uint8_t bsp_software_i2c_read_byteFromSlave(uint8_t I2C_Addr,uint8_t addr)
*�����ܣ���ȡָ���豸 ָ���Ĵ�����һ��ֵ
*��  ����I2C_Addr  Ŀ���豸��ַ
		     reg	     �Ĵ�����ַ
         *buf      ��ȡ����Ҫ�洢�ĵ�ַ    
*����ֵ������ 1ʧ�� 0�ɹ�
*��  ע����
*******************************************************************************/ 
uint8_t bsp_software_i2c_mem_read_byte(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr,uint8_t reg,uint8_t *buf)
{
	bsp_software_i2c_start(soft_i2c);	
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0);	 //���ʹӻ���ַ
	if(bsp_software_i2c_wait_ack(soft_i2c)) //����ӻ�δӦ�������ݷ���ʧ��
	{
		bsp_software_i2c_stop(soft_i2c);
		return 1;
	}
	bsp_software_i2c_send_byte(soft_i2c,reg); //���ͼĴ�����ַ
	bsp_software_i2c_wait_ack(soft_i2c);	  
	
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|1); //�������ģʽ			   
	bsp_software_i2c_wait_ack(soft_i2c);
	*buf=bsp_software_i2c_read_byte(soft_i2c,0);	   
  bsp_software_i2c_stop(soft_i2c); //����һ��ֹͣ����
	return 0;
}


uint8_t bsp_software_i2c_read_data(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr,uint8_t *buf)
{
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|1); //�������ģʽ			   
	bsp_software_i2c_wait_ack(soft_i2c);
	*buf=bsp_software_i2c_read_byte(soft_i2c,0);	   
    bsp_software_i2c_stop(soft_i2c); //����һ��ֹͣ����
	return 0;
}

/******************************************************************************
*��  ����uint8_t IIC_WriteByteFromSlave(uint8_t I2C_Addr,uint8_t addr��uint8_t buf))
*�����ܣ�д��ָ���豸 ָ���Ĵ�����һ��ֵ
*��  ����I2C_Addr  Ŀ���豸��ַ
		     reg	     �Ĵ�����ַ
         buf       Ҫд�������
*����ֵ��1 ʧ�� 0�ɹ�
*��  ע����
*******************************************************************************/ 
uint8_t bsp_software_i2c_mem_write_byte(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr,uint8_t reg,uint8_t data)
{
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0); //���ʹӻ���ַ
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c);
		return 1; //�ӻ���ַд��ʧ��
	}
	bsp_software_i2c_send_byte(soft_i2c,reg); //���ͼĴ�����ַ
	bsp_software_i2c_wait_ack(soft_i2c);	  
	bsp_software_i2c_send_byte(soft_i2c,data); 
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c); 
		return 1; //����д��ʧ��
	}
	bsp_software_i2c_stop(soft_i2c); //����һ��ֹͣ����

  //return 1; //status == 0;
	return 0;
}

uint8_t bsp_software_i2c_write_byte(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr,uint8_t data)
{
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0); //���ʹӻ���ַ
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c);
		return 1; //�ӻ���ַд��ʧ��
	}  
	bsp_software_i2c_send_byte(soft_i2c,data); 
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c); 
		return 1; //����д��ʧ��
	}
	bsp_software_i2c_stop(soft_i2c); //����һ��ֹͣ����
	return 0;
}

/******************************************************************************
*��  ����uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
*�����ܣ���ȡָ���豸 ָ���Ĵ����� length��ֵ
*��  ����dev     Ŀ���豸��ַ
		     reg	   �Ĵ�����ַ
         length  Ҫ�����ֽ���
		     *data   ���������ݽ�Ҫ��ŵ�ָ��
*����ֵ��1�ɹ� 0ʧ��
*��  ע����
*******************************************************************************/ 
uint8_t bsp_software_i2c_mem_read(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr, uint8_t reg, uint8_t length, uint8_t *data)
{
  uint8_t count = 0;
	uint8_t temp;
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0); //���ʹӻ���ַ
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c); 
		return 1; //�ӻ���ַд��ʧ��
	}
	bsp_software_i2c_send_byte(soft_i2c,reg); //���ͼĴ�����ַ
	bsp_software_i2c_wait_ack(soft_i2c);	  
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|1); //�������ģʽ	
	bsp_software_i2c_wait_ack(soft_i2c);
  	for(count=0;count<length;count++)
	{
		if(count!=(length-1))
		temp = bsp_software_i2c_read_byte(soft_i2c,1); //��ACK�Ķ�����
		else  
		temp = bsp_software_i2c_read_byte(soft_i2c,0); //���һ���ֽ�NACK

		data[count] = temp;
	}
    bsp_software_i2c_stop(soft_i2c); //����һ��ֹͣ����
    //return count;
	 return 0;
}

/******************************************************************************
*��  ����uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
*�����ܣ�������ֽ�д��ָ���豸 ָ���Ĵ���
*��  ����dev     Ŀ���豸��ַ
         reg    �Ĵ�����ַ
         length  Ҫд���ֽ���
        *data   Ҫд������ݽ�Ҫ��ŵ�ָ��
*����ֵ��1�ɹ� 0ʧ��
*��  ע����
*******************************************************************************/ 
uint8_t bsp_software_i2c_mem_write(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr, uint8_t reg, uint8_t length, uint8_t* data)
{
  
 	uint8_t count = 0;
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0); //���ʹӻ���ַ
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c);
		return 1; //�ӻ���ַд��ʧ��
	}
	bsp_software_i2c_send_byte(soft_i2c,reg); //���ͼĴ�����ַ
  	bsp_software_i2c_wait_ack(soft_i2c);	  
	for(count=0;count<length;count++)
	{
		bsp_software_i2c_send_byte(soft_i2c,data[count]); 
		if(bsp_software_i2c_wait_ack(soft_i2c)) //ÿһ���ֽڶ�Ҫ�ȴӻ�Ӧ��
		{
			bsp_software_i2c_stop(soft_i2c);
			return 1; //����д��ʧ��
		}
	}
	bsp_software_i2c_stop(soft_i2c); //����һ��ֹͣ����

	return 0;
}

/******************************************************************************
*��  ����uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
*�����ܣ���ȡָ���豸 ָ���Ĵ����� length��ֵ
*��  ����dev     Ŀ���豸��ַ
		     reg	   �Ĵ�����ַ
         length  Ҫ�����ֽ���
		     *data   ���������ݽ�Ҫ��ŵ�ָ��
*����ֵ��1�ɹ� 0ʧ��
*��  ע����
*******************************************************************************/ 
uint8_t bsp_software_i2c_mem16_read(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr, uint16_t reg, uint8_t length, uint8_t *data)
{
  uint8_t count = 0;
	uint8_t temp;
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0); //���ʹӻ���ַ
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c); 
		return 1; //�ӻ���ַд��ʧ��
	}
	bsp_software_i2c_send_byte(soft_i2c,reg>>8); //���ͼĴ�����ַ
  	bsp_software_i2c_wait_ack(soft_i2c);	
	bsp_software_i2c_send_byte(soft_i2c,reg&0XFF); //���ͼĴ�����ַ
  	bsp_software_i2c_wait_ack(soft_i2c);
    
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|1); //�������ģʽ	
	bsp_software_i2c_wait_ack(soft_i2c);
  	for(count=0;count<length;count++)
	{
		if(count!=(length-1))
		temp = bsp_software_i2c_read_byte(soft_i2c,1); //��ACK�Ķ�����
		else  
		temp = bsp_software_i2c_read_byte(soft_i2c,0); //���һ���ֽ�NACK

		data[count] = temp;
	}
    bsp_software_i2c_stop(soft_i2c); //����һ��ֹͣ����
    //return count;
	 return 0;
}

/******************************************************************************
*��  ����uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
*�����ܣ�������ֽ�д��ָ���豸 ָ���Ĵ���
*��  ����dev     Ŀ���豸��ַ
         reg    �Ĵ�����ַ
         length  Ҫд���ֽ���
        *data   Ҫд������ݽ�Ҫ��ŵ�ָ��
*����ֵ��1�ɹ� 0ʧ��
*��  ע����
*******************************************************************************/ 
uint8_t bsp_software_i2c_mem16_write(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr, uint16_t reg, uint8_t length, uint8_t* data)
{
  
 	uint8_t count = 0;
	bsp_software_i2c_start(soft_i2c);
	bsp_software_i2c_send_byte(soft_i2c,I2C_Addr<<1|0); //���ʹӻ���ַ
	if(bsp_software_i2c_wait_ack(soft_i2c))
	{
		bsp_software_i2c_stop(soft_i2c);
		return 1; //�ӻ���ַд��ʧ��
	}
	bsp_software_i2c_send_byte(soft_i2c,reg>>8); //���ͼĴ�����ַ
  	bsp_software_i2c_wait_ack(soft_i2c);	

	bsp_software_i2c_send_byte(soft_i2c,reg&0XFF); //���ͼĴ�����ַ
  	bsp_software_i2c_wait_ack(soft_i2c);
    
	for(count=0;count<length;count++)
	{
		bsp_software_i2c_send_byte(soft_i2c,data[count]); 
		if(bsp_software_i2c_wait_ack(soft_i2c)) //ÿһ���ֽڶ�Ҫ�ȴӻ�Ӧ��
		{
			bsp_software_i2c_stop(soft_i2c);
			return 1; //����д��ʧ��
		}
	}
	bsp_software_i2c_stop(soft_i2c); //����һ��ֹͣ����

	return 0;
}



