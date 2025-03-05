/*!
 * �ļ����ƣ� driver_i2c.h
 * ��    ���� i2c �����ļ�
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
#ifndef DRIVER_I2C_H
#define DRIVER_I2C_H
#include "driver_public.h"

#define I2C_TIMEOUT_MS  10

typedef enum 
{
    MEM_ADDRESS_8BIT=0,
    MEM_ADDRESS_16BIT,   
}i2c_mem_type_enum;

typedef struct __typdef_i2c_struct
{  
    rcu_periph_enum rcu_i2c_x; 
    uint32_t i2c_x; 
    uint32_t frequency; 
    uint32_t slave_addr;
    com_mode_enum i2c_mode; //MODE_POLL,MODE_DMA,MODE_INT 

    typdef_gpio_general* i2c_sda_gpio;
    typdef_gpio_general* i2c_scl_gpio;  
    typdef_dma_general*  i2c_tx_dma;
    typdef_dma_general*  i2c_rx_dma;  

    typdef_com_control_general  i2c_control;

    void (*i2c_slave_addr_callback)(struct  __typdef_i2c_struct *i2cx);
    void (*i2c_slave_end_callback)(struct  __typdef_i2c_struct *i2cx);
    void (*i2c_master_end_callback)(struct __typdef_i2c_struct *i2cx);
    void (*i2c_err_callback)(struct __typdef_i2c_struct *i2cx);
}typdef_i2c_struct;   


#define I2C_TX_DMA_DEF(name, dmax, dma_chx)       \
        typdef_dma_general name##_I2C_TX_DMA = \
        {RCU_##dmax,dmax,dma_chx,Circulation_Disable,NULL}

#define I2C_RX_DMA_DEF(name, dmax, dma_chx)       \
        typdef_dma_general name##_I2C_RX_DMA = \
        {RCU_##dmax,dmax,dma_chx,Circulation_Disable,NULL}
        

#define I2C_SDA_GPIO_DEF(name, port, pin, afio_mode)      \
        AFIO_DEF(name##_I2C_SDA_GPIO,port,pin,AF_OD,afio_mode) 

#define I2C_SCL_GPIO_DEF(name, port, pin, afio_mode)      \
        AFIO_DEF(name##_I2C_SCL_GPIO,port,pin,AF_OD,afio_mode)  

#define I2C_DEF(name, I2Cx, frequency,slave_addr,i2c_mode)      \
        typdef_i2c_struct name = \
        {RCU_##I2Cx,I2Cx,frequency,slave_addr,i2c_mode,&name##_I2C_SDA_GPIO,&name##_I2C_SCL_GPIO,NULL,NULL,NULL,NULL,NULL,NULL,NULL}        


#define I2C_DEF_EXTERN(name) \
    extern typdef_i2c_struct name


//�ⲿ��������
void driver_i2c_stop(typdef_i2c_struct *i2cx);

Drv_Err driver_i2c_flag_wait_timeout(typdef_i2c_struct *i2cx, uint32_t flag ,FlagStatus wait_state);
    
Drv_Err driver_i2c_poll_read(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr,uint8_t *pbuff,uint16_t Length);

Drv_Err driver_i2c_poll_write(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr,uint8_t *pbuff,uint16_t Length);
    

Drv_Err driver_i2c_mem_poll_write(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr,uint16_t slave_reg_addr,i2c_mem_type_enum reg_addr_type,uint8_t *pbuff,uint16_t Length);

Drv_Err driver_i2c_mem_poll_read(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr,uint16_t slave_reg_addr,i2c_mem_type_enum reg_addr_type,uint8_t *pbuff,uint16_t Length);

uint8_t driver_i2c_master_read_byte(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr);

Drv_Err driver_i2c_master_write_byte(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr,uint8_t Data);
    
void driver_i2c_init(typdef_i2c_struct *i2cx);

void driver_i2c_slave_int_handler(typdef_i2c_struct *i2cx);

void driver_i2c_slave_int_handler(typdef_i2c_struct *i2cx);


#endif /* DRIVER_I2C_H*/
