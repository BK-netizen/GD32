#ifndef __BSP_SOFTWARE_I2C_H
#define __BSP_SOFTWARE_I2C_H

#include "driver_public.h"

typedef struct __typdef_software_i2c
{
    typdef_gpio_general* iic_scl;
    typdef_gpio_general* iic_sda; 
    uint8_t delay_us;
}typdef_software_i2c;


#define SOFT_I2C_SDA_DEF(name, port, pin)      \
        GPIO_DEF(name##_SDA, port, pin, OUT_PP, SET, NULL); 

#define SOFT_I2C_SCL_DEF(name, port, pin)      \
        GPIO_DEF(name##_SCL, port, pin, OUT_PP, SET, NULL); 


#define SOFT_I2C_DEF(name,freq)       \
        typdef_software_i2c name = \
        {&name##_SCL,&name##_SDA,1000000U/freq/2};
        
#define SOFT_I2C_DEF_EXTERN(name) \
    extern typdef_software_i2c name
        
    
        
SOFT_I2C_DEF_EXTERN(CAMERA_IIC);
SOFT_I2C_DEF_EXTERN(TOUCH_CAP_IIC);
        
        

//IIC所有操作函数
void bsp_software_i2c_init(typdef_software_i2c* soft_i2c);        //初始化IIC的IO口
void bsp_software_i2c_start(typdef_software_i2c* soft_i2c);			  //发送IIC开始信号
void bsp_software_i2c_stop(typdef_software_i2c* soft_i2c);	  	  //发送IIC停止信号
void bsp_software_i2c_ack(typdef_software_i2c* soft_i2c);					//IIC发送ACK信号
void bsp_software_i2c_nack(typdef_software_i2c* soft_i2c);				//IIC不发送ACK信号
uint8_t bsp_software_i2c_wait_ack(typdef_software_i2c* soft_i2c); 		 //IIC等待ACK信号

void bsp_software_i2c_send_byte(typdef_software_i2c* soft_i2c,uint8_t data);  //IIC发送一个字节
uint8_t bsp_software_i2c_read_byte(typdef_software_i2c* soft_i2c,uint8_t ack);//IIC读取一个字节

uint8_t bsp_software_i2c_mem_read_byte(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr,uint8_t reg,uint8_t *buf);
uint8_t bsp_software_i2c_mem_read(typdef_software_i2c* soft_i2c,uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data);
uint8_t bsp_software_i2c_mem_write_byte(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr,uint8_t reg,uint8_t buf);
uint8_t bsp_software_i2c_mem_write(typdef_software_i2c* soft_i2c,uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data);
uint8_t bsp_software_i2c_write_byte(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr,uint8_t data);
uint8_t bsp_software_i2c_read_data(typdef_software_i2c* soft_i2c,uint8_t I2C_Addr,uint8_t *buf);

uint8_t bsp_software_i2c_mem16_write(typdef_software_i2c* soft_i2c,uint8_t dev, uint16_t reg, uint8_t length, uint8_t* data);
uint8_t bsp_software_i2c_mem16_read(typdef_software_i2c* soft_i2c,uint8_t dev, uint16_t reg, uint8_t length, uint8_t *data);

#endif
