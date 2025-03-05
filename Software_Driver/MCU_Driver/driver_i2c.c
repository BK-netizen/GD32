/*!
 * �ļ����ƣ� driver_i2c.c
 * ��    ���� i2c �����ļ�
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

#include "driver_i2c.h"


#define MAX_RELOAD_SIZE 255

//I2C_SCL_GPIO_DEF(BOARD_I2C,B,10,GPIO_AF_4);
//I2C_SDA_GPIO_DEF(BOARD_I2C,B,11,GPIO_AF_4);
//I2C_DEF(BOARD_I2C,I2C1,100000,0x60,MODE_POLL); 

/*!
* ˵��     i2c stop���ͣ��ɻָ��ӻ��������ߣ�
* ����[1]  i2cx��i2cx�ṹ��ָ�� 
* ����ֵ   ��
*/
void driver_i2c_stop(typdef_i2c_struct *i2cx)
{
    __IO uint8_t i=0,delay=0;    
    __IO uint64_t timeout = driver_tick;  
    Drv_Err i2c_state=DRV_SUCCESS;    

    if(i2c_flag_get(i2cx->i2c_x,I2C_FLAG_I2CBSY)){
        
        i2c_stop_on_bus(i2cx->i2c_x);         
        
        /* wait until I2C bus is idle */  
        i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_STPDET,SET);
        i2c_flag_clear(i2cx->i2c_x, I2C_FLAG_STPDET);  
        if(i2c_state!=DRV_SUCCESS){
            
            driver_gpio_pin_set(i2cx->i2c_scl_gpio);
            driver_gpio_pin_set(i2cx->i2c_sda_gpio);
            
            i2cx->i2c_sda_gpio->gpio_mode=OUT_OD;
            i2cx->i2c_scl_gpio->gpio_mode=OUT_OD;

            driver_gpio_general_init(i2cx->i2c_scl_gpio);                
            driver_gpio_general_init(i2cx->i2c_sda_gpio);

            delay_ms(1);
            
            for(i=0;i<9;i++)
            {
                driver_gpio_pin_reset(i2cx->i2c_scl_gpio); 
                delay_ms(1);
                driver_gpio_pin_set(i2cx->i2c_scl_gpio);
                delay_ms(1);
            }
            
            i2cx->i2c_sda_gpio->gpio_mode=AF_OD;
            i2cx->i2c_scl_gpio->gpio_mode=AF_OD;                
            driver_i2c_init(i2cx);                           
        }                
    }
}

/*!
* ˵��     ��ʱ�ȴ�flag״̬
* ����[1]  i2cx��i2cx�ṹ��ָ��
* ����[2]  flag���ȴ���flag
* ����[3]  wait_state���ȴ���flag״̬ 
* ����ֵ   ��
*/
Drv_Err driver_i2c_flag_wait_timeout(typdef_i2c_struct *i2cx, uint32_t flag ,FlagStatus wait_state)
{
    __IO uint64_t timeout = driver_tick;    
    while(wait_state!=i2c_flag_get(i2cx->i2c_x, flag)){
        if((timeout+I2C_TIMEOUT_MS) <= driver_tick) {              
            return DRV_ERROR;
        } 
    }
    return DRV_SUCCESS;
}

/*
*˵�� :��I2C���I2C��ַ����DMAģʽдN�ֽ�����
*����[1]              i2cx��i2cx�ṹ��ָ��
*����[2]               i2c_slave_addr --- I2C��ַ7λ
 ����[3]               slave_reg_addr --- Ŀ��Ĵ�����ַ
 ����[4]               reg_addr_type  --- Ŀ��Ĵ�����ַ��� MEM_ADDRESS_8BIT/MEM_ADDRESS_16BIT 
 ����[5]               *pbuff --- д���ݻ���   
 ����[6]               Length--- д���ݳ���   
 ����[7]     
*����ֵ                i2c_state --- I2Cͨ���Ƿ�ɹ�
*/
Drv_Err driver_i2c_mem_dma_write(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr,uint16_t slave_reg_addr,i2c_mem_type_enum reg_addr_type,uint8_t *pbuff,uint8_t Length)
{

    Drv_Err i2c_state=DRV_SUCCESS;
    
    i2cx->i2c_control.Com_Flag.Bits.SendSuccess=0;
    i2cx->i2c_control.Com_Flag.Bits.SendState=1;    
    i2cx->i2c_control.p_Send=pbuff;
    i2cx->i2c_control.SendSize=Length; 
    i2cx->i2c_control.SendCount=0;  
    
    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_I2CBSY,RESET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    
    /* clear I2C_TDATA register */
    I2C_STAT(i2cx->i2c_x) |= I2C_STAT_TBE;
    /* configure slave address */
    i2c_master_addressing(i2cx->i2c_x, i2c_slave_addr, I2C_MASTER_TRANSMIT);
    
    if(reg_addr_type==MEM_ADDRESS_16BIT){    
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, 2);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_disable(i2cx->i2c_x);  
    
        i2c_start_on_bus(i2cx->i2c_x);    

        /* wait until I2C bus is idle */  
        i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TBE,SET);
        if(i2c_state!=DRV_SUCCESS){
            driver_i2c_stop(i2cx);
            return DRV_ERROR;
        }
        
        i2c_data_transmit(i2cx->i2c_x, (uint8_t)((slave_reg_addr>>8)&0x00ff));
    }else{
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, 1);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_disable(i2cx->i2c_x);  
    
        i2c_start_on_bus(i2cx->i2c_x);            
    }
    

    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TBE,SET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    /* send the EEPROM's internal address to write to : only one byte address */
    i2c_data_transmit(i2cx->i2c_x, (uint8_t)(slave_reg_addr&0x00ff));
   
    /* wait until TC bit is set */
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TC,SET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }   
    
    /* enable I2C reload mode */
    i2c_reload_disable(i2cx->i2c_x);
    
    /* configure number of bytes to be transferred */
    i2c_transfer_byte_number_config(i2cx->i2c_x, Length);
    /* clear I2C_TDATA register */
    I2C_STAT(i2cx->i2c_x) |= I2C_STAT_TBE;
    /* enable I2C automatic end mode in master mode */
    i2c_automatic_end_enable(i2cx->i2c_x);

    i2c_dma_disable(i2cx->i2c_x, I2C_DMA_TRANSMIT);
    driver_dma_start(i2cx->i2c_tx_dma,pbuff,Length);
    i2c_dma_enable(i2cx->i2c_x, I2C_DMA_TRANSMIT);  
    
//    i2c_start_on_bus(i2cx->i2c_x);
    
    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_STPDET,SET);
        
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    
    i2c_flag_clear(i2cx->i2c_x, I2C_FLAG_STPDET);
    
    i2cx->i2c_control.Com_Flag.Bits.SendSuccess=1;
    i2cx->i2c_control.Com_Flag.Bits.SendState=0; 
    i2cx->i2c_control.SendCount=Length;    

	return DRV_SUCCESS;
}

/*
*˵�� :��I2C���I2C��ַ����DMAģʽ��N�ֽ�����
*����[1]               i2cx��i2cx�ṹ��ָ��
*����[2]               i2c_slave_addr --- I2C��ַ7λ
 ����[3]               slave_reg_addr --- Ŀ��Ĵ�����ַ
 ����[4]               reg_addr_type  --- Ŀ��Ĵ�����ַ��� MEM_ADDRESS_8BIT/MEM_ADDRESS_16BIT 
 ����[5]               *pbuff --- д���ݻ���   
 ����[6]               Length--- д���ݳ���   
 ����[7]     
*����ֵ                i2c_state --- I2Cͨ���Ƿ�ɹ�
*/
Drv_Err driver_i2c_mem_dma_read(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr,uint16_t slave_reg_addr,i2c_mem_type_enum reg_addr_type,uint8_t *pbuff,uint8_t Length)
{
    Drv_Err i2c_state=DRV_SUCCESS;
    
    i2cx->i2c_control.Com_Flag.Bits.RecSuccess=0;
    i2cx->i2c_control.Com_Flag.Bits.RecState=1;    
    i2cx->i2c_control.p_Rec=pbuff;
    i2cx->i2c_control.RecSize=Length; 
    i2cx->i2c_control.RecCount=0;    
    
    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_I2CBSY,RESET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    
    /* clear I2C_TDATA register */
    I2C_STAT(i2cx->i2c_x) |= I2C_STAT_TBE;
    /* configure slave address */
    i2c_master_addressing(i2cx->i2c_x, i2c_slave_addr, I2C_MASTER_TRANSMIT);
    
    if(reg_addr_type==MEM_ADDRESS_16BIT){    
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, 2);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_disable(i2cx->i2c_x);  
    
        i2c_start_on_bus(i2cx->i2c_x);    

        /* wait until I2C bus is idle */  
        i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TBE,SET);
        if(i2c_state!=DRV_SUCCESS){
            driver_i2c_stop(i2cx);
            return DRV_ERROR;
        }
        
        i2c_data_transmit(i2cx->i2c_x, (uint8_t)((slave_reg_addr>>8)&0x00ff));
    }else{
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, 1);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_disable(i2cx->i2c_x);  
    
        i2c_start_on_bus(i2cx->i2c_x);            
    }
    

    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TBE,SET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    /* send the EEPROM's internal address to write to : only one byte address */
    i2c_data_transmit(i2cx->i2c_x, (uint8_t)(slave_reg_addr&0x00ff));
   
    /* wait until TC bit is set */
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TC,SET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }  

    i2c_master_addressing(i2cx->i2c_x, i2c_slave_addr, I2C_MASTER_RECEIVE);
    /* enable I2C reload mode */
    i2c_reload_disable(i2cx->i2c_x);
    /* disable I2C automatic end mode in master mode */
    i2c_automatic_end_enable(i2cx->i2c_x);          


    /* configure number of bytes to be transferred */
    i2c_transfer_byte_number_config(i2cx->i2c_x, Length);
    
    i2c_dma_disable(i2cx->i2c_x, I2C_DMA_RECEIVE);
    driver_dma_start(i2cx->i2c_rx_dma,pbuff,Length);
    i2c_dma_enable(i2cx->i2c_x, I2C_DMA_RECEIVE);    
    
    /* restart */
    i2c_start_on_bus(i2cx->i2c_x);        
    
    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_STPDET,SET);
        
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    
    i2c_flag_clear(i2cx->i2c_x, I2C_FLAG_STPDET);
    
    i2cx->i2c_control.RecCount=Length;
    i2cx->i2c_control.Com_Flag.Bits.RecSuccess=1;
    i2cx->i2c_control.Com_Flag.Bits.RecState=0;    

    return DRV_SUCCESS ;    
}


/*
*˵�� :��I2C���I2C��ַ����ģʽдN�ֽ�����
*����[1]              i2cx��i2cx�ṹ��ָ��
*����[2]               i2c_slave_addr --- I2C��ַ7λ
 ����[3]               slave_reg_addr --- Ŀ��Ĵ�����ַ
 ����[4]               reg_addr_type  --- Ŀ��Ĵ�����ַ��� MEM_ADDRESS_8BIT/MEM_ADDRESS_16BIT 
 ����[5]               *pbuff --- д���ݻ���   
 ����[6]               Length--- д���ݳ���   
 ����[7]     
*����ֵ                i2c_state --- I2Cͨ���Ƿ�ɹ�
*/
Drv_Err driver_i2c_mem_poll_write(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr,uint16_t slave_reg_addr,i2c_mem_type_enum reg_addr_type,uint8_t *pbuff,uint16_t Length)
{
    Drv_Err i2c_state=DRV_SUCCESS;

    uint8_t size_temp=0;
    i2cx->i2c_control.Com_Flag.Bits.SendSuccess=0;
    i2cx->i2c_control.Com_Flag.Bits.SendState=1;    
    i2cx->i2c_control.p_Send=pbuff;
    i2cx->i2c_control.SendSize=Length; 
    i2cx->i2c_control.SendCount=0;   
    
    
    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_I2CBSY,RESET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    
    /* clear I2C_TDATA register */
    I2C_STAT(i2cx->i2c_x) |= I2C_STAT_TBE;
    /* configure slave address */
    i2c_master_addressing(i2cx->i2c_x, i2c_slave_addr, I2C_MASTER_TRANSMIT);
    
    i2c_reload_enable(i2cx->i2c_x);
    
    if(reg_addr_type==MEM_ADDRESS_16BIT){    
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, 2);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_disable(i2cx->i2c_x);  
    
        i2c_start_on_bus(i2cx->i2c_x);    

        /* wait until I2C bus is idle */  
        i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TBE,SET);
        if(i2c_state!=DRV_SUCCESS){
            driver_i2c_stop(i2cx);
            return DRV_ERROR;
        }
        
        i2c_data_transmit(i2cx->i2c_x, (uint8_t)((slave_reg_addr>>8)&0x00ff));
    }else{
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, 1);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_disable(i2cx->i2c_x);  
    
        i2c_start_on_bus(i2cx->i2c_x);            
    }
    

    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TBE,SET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    /* send the EEPROM's internal address to write to : only one byte address */
    i2c_data_transmit(i2cx->i2c_x, (uint8_t)(slave_reg_addr&0x00ff));
   
    /* wait until TC bit is set */
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TBE,SET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }  
 
    if(i2cx->i2c_control.SendSize > MAX_RELOAD_SIZE) {
        size_temp = MAX_RELOAD_SIZE;               
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, size_temp);        
        /* enable I2C reload mode */
        i2c_reload_enable(i2cx->i2c_x);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_disable(i2cx->i2c_x);               
    }else{
        size_temp=i2cx->i2c_control.SendSize;
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, size_temp);         
        /* enable I2C reload mode */
        i2c_reload_disable(i2cx->i2c_x);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_enable(i2cx->i2c_x);  
    }    

//    i2c_start_on_bus(i2cx->i2c_x); 
//    i2c_dma_disable(i2cx->i2c_x, I2C_DMA_SendEIVE);
//    driver_dma_start(i2cx->i2c_rx_dma,pbuff,length);
//    i2c_dma_enable(i2cx->i2c_x, I2C_DMA_SendEIVE);    
    do
    {
        /* wait until I2C bus is idle */  
        i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TBE,SET);
        if(i2c_state!=DRV_SUCCESS){
            driver_i2c_stop(i2cx);
            return DRV_ERROR;
        }

        i2c_data_transmit(i2cx->i2c_x, *pbuff);

        pbuff++;
        i2cx->i2c_control.SendCount++;
        size_temp--;
//      /* Increment Buffer pointer */
//      hi2c->pBuffPtr++;

//      hi2c->XferSize--;
//      hi2c->XferCount--;

        if ((i2cx->i2c_control.SendCount < i2cx->i2c_control.SendSize) && (size_temp == 0U))
        {
            /* wait until I2C TC */  
            i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TCR,SET);
            if(i2c_state!=DRV_SUCCESS){
                driver_i2c_stop(i2cx);
                return DRV_ERROR;
            }
            
            if( (i2cx->i2c_control.SendSize - i2cx->i2c_control.SendCount) > MAX_RELOAD_SIZE) {

                size_temp = MAX_RELOAD_SIZE;
                /* enable I2C reload mode */
                i2c_reload_enable(i2cx->i2c_x);
                /* disable I2C automatic end mode in master mode */
                i2c_automatic_end_disable(i2cx->i2c_x);               
            }else{
                size_temp=i2cx->i2c_control.SendSize - i2cx->i2c_control.SendCount;
                /* enable I2C reload mode */
                i2c_reload_disable(i2cx->i2c_x);
                /* disable I2C automatic end mode in master mode */
                i2c_automatic_end_enable(i2cx->i2c_x);  
            }
            /* configure number of bytes to be transferred */
            i2c_transfer_byte_number_config(i2cx->i2c_x, size_temp);            
        } 
    }while (i2cx->i2c_control.SendCount < i2cx->i2c_control.SendSize);       
    
    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_STPDET,SET);
        
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    
    i2c_flag_clear(i2cx->i2c_x, I2C_FLAG_STPDET);
    
    i2cx->i2c_control.SendCount=Length;
    i2cx->i2c_control.Com_Flag.Bits.SendSuccess=1;
    i2cx->i2c_control.Com_Flag.Bits.SendState=0;    

    return DRV_SUCCESS ;    
}

/*
*˵�� :��I2C���I2C��ַ����ģʽ��N�ֽ�����
*����[1]               i2cx��i2cx�ṹ��ָ��
*����[2]               i2c_slave_addr --- I2C��ַ7λ
 ����[3]               slave_reg_addr --- Ŀ��Ĵ�����ַ
 ����[4]               reg_addr_type  --- Ŀ��Ĵ�����ַ��� MEM_ADDRESS_8BIT/MEM_ADDRESS_16BIT 
 ����[5]               *pbuff --- д���ݻ���   
 ����[6]               Length--- д���ݳ���   
 ����[7]     
*����ֵ                i2c_state --- I2Cͨ���Ƿ�ɹ�
*/
Drv_Err driver_i2c_mem_poll_read(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr,uint16_t slave_reg_addr,i2c_mem_type_enum reg_addr_type,uint8_t *pbuff,uint16_t Length)
{
    Drv_Err i2c_state=DRV_SUCCESS;

    uint8_t size_temp=0;
    i2cx->i2c_control.Com_Flag.Bits.RecSuccess=0;
    i2cx->i2c_control.Com_Flag.Bits.RecState=1;    
    i2cx->i2c_control.p_Rec=pbuff;
    i2cx->i2c_control.RecSize=Length; 
    i2cx->i2c_control.RecCount=0;   
    
    
    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_I2CBSY,RESET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    
    /* clear I2C_TDATA register */
    I2C_STAT(i2cx->i2c_x) |= I2C_STAT_TBE;
    /* configure slave address */
    i2c_master_addressing(i2cx->i2c_x, i2c_slave_addr, I2C_MASTER_TRANSMIT);
    
    i2c_reload_disable(i2cx->i2c_x);
    /* disable I2C automatic end mode in master mode */
    i2c_automatic_end_disable(i2cx->i2c_x);      
    
    if(reg_addr_type==MEM_ADDRESS_16BIT){    
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, 2);
    
        i2c_start_on_bus(i2cx->i2c_x);    

        /* wait until I2C bus is idle */  
        i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TBE,SET);
        if(i2c_state!=DRV_SUCCESS){
            driver_i2c_stop(i2cx);
            return DRV_ERROR;
        }
        
        i2c_data_transmit(i2cx->i2c_x, (uint8_t)((slave_reg_addr>>8)&0x00ff));
    }else{
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, 1);
    
        i2c_start_on_bus(i2cx->i2c_x);            
    }
    

    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TBE,SET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    /* send the EEPROM's internal address to write to : only one byte address */
    i2c_data_transmit(i2cx->i2c_x, (uint8_t)(slave_reg_addr&0x00ff));
   
    /* wait until TC bit is set */
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TC,SET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }  

    i2c_master_addressing(i2cx->i2c_x, i2c_slave_addr, I2C_MASTER_RECEIVE);
       
    if(i2cx->i2c_control.RecSize > MAX_RELOAD_SIZE) {
        size_temp = MAX_RELOAD_SIZE;
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, size_temp);        
        /* enable I2C reload mode */
        i2c_reload_enable(i2cx->i2c_x);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_disable(i2cx->i2c_x);               
    }else{
        size_temp=i2cx->i2c_control.RecSize;
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, size_temp);       
        /* enable I2C reload mode */
        i2c_reload_disable(i2cx->i2c_x);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_enable(i2cx->i2c_x);  
    }    
                
    
    i2c_start_on_bus(i2cx->i2c_x); 
//    i2c_dma_disable(i2cx->i2c_x, I2C_DMA_RECEIVE);
//    driver_dma_start(i2cx->i2c_rx_dma,pbuff,length);
//    i2c_dma_enable(i2cx->i2c_x, I2C_DMA_RECEIVE);    
    do
    {
        /* wait until I2C bus is idle */  
        i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_RBNE,SET);
        if(i2c_state!=DRV_SUCCESS){
            driver_i2c_stop(i2cx);
            return DRV_ERROR;
        }

        /* Read data from RXDR */
        *pbuff = i2c_data_receive(i2cx->i2c_x);

        pbuff++;
        i2cx->i2c_control.RecCount++;
        size_temp--;
//      /* Increment Buffer pointer */
//      hi2c->pBuffPtr++;

//      hi2c->XferSize--;
//      hi2c->XferCount--;

        if ((i2cx->i2c_control.RecCount < i2cx->i2c_control.RecSize) && (size_temp == 0U))
        {
            /* wait until I2C TC */  
            i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TCR,SET);
            if(i2c_state!=DRV_SUCCESS){
                driver_i2c_stop(i2cx);
                return DRV_ERROR;
            }
            
            if( (i2cx->i2c_control.RecSize - i2cx->i2c_control.RecCount) > MAX_RELOAD_SIZE) {

                size_temp = MAX_RELOAD_SIZE;
                /* enable I2C reload mode */
                i2c_reload_enable(i2cx->i2c_x);
                /* disable I2C automatic end mode in master mode */
                i2c_automatic_end_disable(i2cx->i2c_x);               
            }else{
                size_temp=i2cx->i2c_control.RecSize - i2cx->i2c_control.RecCount;
                /* enable I2C reload mode */
                i2c_reload_disable(i2cx->i2c_x);
                /* disable I2C automatic end mode in master mode */
                i2c_automatic_end_enable(i2cx->i2c_x);  
            }
            /* configure number of bytes to be transferred */
            i2c_transfer_byte_number_config(i2cx->i2c_x, size_temp);            
        } 
    }while (i2cx->i2c_control.RecCount < i2cx->i2c_control.RecSize);       
    
    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_STPDET,SET);
        
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    
    i2c_flag_clear(i2cx->i2c_x, I2C_FLAG_STPDET);
    
    i2cx->i2c_control.RecCount=Length;
    i2cx->i2c_control.Com_Flag.Bits.RecSuccess=1;
    i2cx->i2c_control.Com_Flag.Bits.RecState=0;    

    return DRV_SUCCESS ;    
}

/*
*˵�� :��I2C��N�ֽ�����
*����[1]              i2cx��i2cx�ṹ��ָ��
*����[2]               i2c_slave_addr --- I2C��ַ7λ
 ����[3]               reg_addr_type  --- Ŀ��Ĵ�����ַ��� MEM_ADDRESS_8BIT/MEM_ADDRESS_16BIT 
 ����[4]               *pbuff --- ���ݻ���   
 ����[5]               Length--- ���ݳ���   
 ����[6]     
*����ֵ                i2c_state --- I2Cͨ���Ƿ�ɹ�
*/
Drv_Err driver_i2c_poll_read(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr,uint8_t *pbuff,uint16_t Length)
{
    Drv_Err i2c_state=DRV_SUCCESS;

    uint8_t size_temp=0;
    i2cx->i2c_control.Com_Flag.Bits.RecSuccess=0;
    i2cx->i2c_control.Com_Flag.Bits.RecState=1;    
    i2cx->i2c_control.p_Rec=pbuff;
    i2cx->i2c_control.RecSize=Length; 
    i2cx->i2c_control.RecCount=0;   
    
    
    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_I2CBSY,RESET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    
    i2c_master_addressing(i2cx->i2c_x, i2c_slave_addr, I2C_MASTER_RECEIVE);
       
    if(i2cx->i2c_control.RecSize > MAX_RELOAD_SIZE) {
        size_temp = MAX_RELOAD_SIZE;
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, size_temp);        
        /* enable I2C reload mode */
        i2c_reload_enable(i2cx->i2c_x);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_disable(i2cx->i2c_x);               
    }else{
        size_temp=i2cx->i2c_control.RecSize;
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, size_temp);       
        /* enable I2C reload mode */
        i2c_reload_disable(i2cx->i2c_x);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_enable(i2cx->i2c_x);  
    }    
                
    
    i2c_start_on_bus(i2cx->i2c_x); 
//    i2c_dma_disable(i2cx->i2c_x, I2C_DMA_RECEIVE);
//    driver_dma_start(i2cx->i2c_rx_dma,pbuff,length);
//    i2c_dma_enable(i2cx->i2c_x, I2C_DMA_RECEIVE);    
    do
    {
        /* wait until I2C bus is idle */  
        i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_RBNE,SET);
        if(i2c_state!=DRV_SUCCESS){
            driver_i2c_stop(i2cx);
            return DRV_ERROR;
        }

        /* Read data from RXDR */
        *pbuff = i2c_data_receive(i2cx->i2c_x);

        pbuff++;
        i2cx->i2c_control.RecCount++;
        size_temp--;
//      /* Increment Buffer pointer */
//      hi2c->pBuffPtr++;

//      hi2c->XferSize--;
//      hi2c->XferCount--;

        if ((i2cx->i2c_control.RecCount < i2cx->i2c_control.RecSize) && (size_temp == 0U))
        {
            /* wait until I2C TC */  
            i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TCR,SET);
            if(i2c_state!=DRV_SUCCESS){
                driver_i2c_stop(i2cx);
                return DRV_ERROR;
            }
            
            if( (i2cx->i2c_control.RecSize - i2cx->i2c_control.RecCount) > MAX_RELOAD_SIZE) {

                size_temp = MAX_RELOAD_SIZE;
                /* enable I2C reload mode */
                i2c_reload_enable(i2cx->i2c_x);
                /* disable I2C automatic end mode in master mode */
                i2c_automatic_end_disable(i2cx->i2c_x);               
            }else{
                size_temp=i2cx->i2c_control.RecSize - i2cx->i2c_control.RecCount;
                /* enable I2C reload mode */
                i2c_reload_disable(i2cx->i2c_x);
                /* disable I2C automatic end mode in master mode */
                i2c_automatic_end_enable(i2cx->i2c_x);  
            }
            /* configure number of bytes to be transferred */
            i2c_transfer_byte_number_config(i2cx->i2c_x, size_temp);            
        } 
    }while (i2cx->i2c_control.RecCount < i2cx->i2c_control.RecSize);       
    
    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_STPDET,SET);
        
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    
    i2c_flag_clear(i2cx->i2c_x, I2C_FLAG_STPDET);
    
    i2cx->i2c_control.RecCount=Length;
    i2cx->i2c_control.Com_Flag.Bits.RecSuccess=1;
    i2cx->i2c_control.Com_Flag.Bits.RecState=0;    

    return DRV_SUCCESS ;    
}

/*
*˵�� :��I2CдN�ֽ�����
*����[1]              i2cx��i2cx�ṹ��ָ��
*����[2]               i2c_slave_addr --- I2C��ַ7λ
 ����[3]               reg_addr_type  --- Ŀ��Ĵ�����ַ��� MEM_ADDRESS_8BIT/MEM_ADDRESS_16BIT 
 ����[4]               *pbuff --- ���ݻ���   
 ����[5]               Length--- ���ݳ���   
 ����[6]     
*����ֵ                i2c_state --- I2Cͨ���Ƿ�ɹ�
*/
Drv_Err driver_i2c_poll_write(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr,uint8_t *pbuff,uint16_t Length)
{
    Drv_Err i2c_state=DRV_SUCCESS;

    uint8_t size_temp=0;
    i2cx->i2c_control.Com_Flag.Bits.SendSuccess=0;
    i2cx->i2c_control.Com_Flag.Bits.SendState=1;    
    i2cx->i2c_control.p_Send=pbuff;
    i2cx->i2c_control.SendSize=Length; 
    i2cx->i2c_control.SendCount=0;   
    
    
    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_I2CBSY,RESET);
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    
    /* clear I2C_TDATA register */
    I2C_STAT(i2cx->i2c_x) |= I2C_STAT_TBE;
    /* configure slave address */
    i2c_master_addressing(i2cx->i2c_x, i2c_slave_addr, I2C_MASTER_TRANSMIT);    
 
    if(i2cx->i2c_control.SendSize > MAX_RELOAD_SIZE) {
        size_temp = MAX_RELOAD_SIZE;               
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, size_temp);        
        /* enable I2C reload mode */
        i2c_reload_enable(i2cx->i2c_x);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_disable(i2cx->i2c_x);               
    }else{
        size_temp=i2cx->i2c_control.SendSize;
        /* configure number of bytes to be transferred */
        i2c_transfer_byte_number_config(i2cx->i2c_x, size_temp);         
        /* enable I2C reload mode */
        i2c_reload_disable(i2cx->i2c_x);
        /* disable I2C automatic end mode in master mode */
        i2c_automatic_end_enable(i2cx->i2c_x);  
    }    

    i2c_start_on_bus(i2cx->i2c_x); 
//    i2c_dma_disable(i2cx->i2c_x, I2C_DMA_SendEIVE);
//    driver_dma_start(i2cx->i2c_rx_dma,pbuff,length);
//    i2c_dma_enable(i2cx->i2c_x, I2C_DMA_SendEIVE);    
    do
    {
        /* wait until I2C bus is idle */  
        i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TBE,SET);
        if(i2c_state!=DRV_SUCCESS){
            driver_i2c_stop(i2cx);
            return DRV_ERROR;
        }

        i2c_data_transmit(i2cx->i2c_x, *pbuff);

        pbuff++;
        i2cx->i2c_control.SendCount++;
        size_temp--;
//      /* Increment Buffer pointer */
//      hi2c->pBuffPtr++;

//      hi2c->XferSize--;
//      hi2c->XferCount--;

        if ((i2cx->i2c_control.SendCount < i2cx->i2c_control.SendSize) && (size_temp == 0U))
        {
            /* wait until I2C TC */  
            i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_TCR,SET);
            if(i2c_state!=DRV_SUCCESS){
                driver_i2c_stop(i2cx);
                return DRV_ERROR;
            }
            
            if( (i2cx->i2c_control.SendSize - i2cx->i2c_control.SendCount) > MAX_RELOAD_SIZE) {

                size_temp = MAX_RELOAD_SIZE;
                /* enable I2C reload mode */
                i2c_reload_enable(i2cx->i2c_x);
                /* disable I2C automatic end mode in master mode */
                i2c_automatic_end_disable(i2cx->i2c_x);               
            }else{
                size_temp=i2cx->i2c_control.SendSize - i2cx->i2c_control.SendCount;
                /* enable I2C reload mode */
                i2c_reload_disable(i2cx->i2c_x);
                /* disable I2C automatic end mode in master mode */
                i2c_automatic_end_enable(i2cx->i2c_x);  
            }
            /* configure number of bytes to be transferred */
            i2c_transfer_byte_number_config(i2cx->i2c_x, size_temp);            
        } 
    }while (i2cx->i2c_control.SendCount < i2cx->i2c_control.SendSize);       
    
    /* wait until I2C bus is idle */  
    i2c_state=driver_i2c_flag_wait_timeout(i2cx, I2C_FLAG_STPDET,SET);
        
    if(i2c_state!=DRV_SUCCESS){
        driver_i2c_stop(i2cx);
        return DRV_ERROR;
    }
    
    i2c_flag_clear(i2cx->i2c_x, I2C_FLAG_STPDET);
    
    i2cx->i2c_control.SendCount=Length;
    i2cx->i2c_control.Com_Flag.Bits.SendSuccess=1;
    i2cx->i2c_control.Com_Flag.Bits.SendState=0;    

    return DRV_SUCCESS ;    
}

/*
*˵�� :��I2C��һ���ֽ�����
*����[1]              i2cx��i2cx�ṹ��ָ��
*����[2]               i2c_slave_addr --- I2C��ַ7λ    
*����ֵ                ����
*/
uint8_t driver_i2c_master_read_byte(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr)
{
    uint8_t data=0;
    
    driver_i2c_poll_read(i2cx,i2c_slave_addr,&data,1);    

    return data ;
}

/*
*˵�� :��I2Cдһ���ֽ�����
*����[1]              i2cx��i2cx�ṹ��ָ��
*����[2]               i2c_slave_addr --- I2C��ַ7λ
*����[2]               data --- ����     
*����ֵ                i2c_state --- I2Cͨ���Ƿ�ɹ�
*/
Drv_Err driver_i2c_master_write_byte(typdef_i2c_struct *i2cx,uint8_t i2c_slave_addr,uint8_t Data)
{
    return driver_i2c_poll_write(i2cx,i2c_slave_addr,&Data,1);
}


/*
*˵�� :i2c��ʼ��
*����[1]              i2cx��i2cx�ṹ��ָ��    
*����ֵ               ��
*/
void driver_i2c_init(typdef_i2c_struct *i2cx)
{
    __IO uint32_t apb1_freq=0;
    i2c_idx_enum idx_i2cx=IDX_I2C0;
    __IO uint32_t freq_tick=0;
    __IO uint32_t scl_dely=0,sclh=0,scll=0;
//    rcu_periph_clock_enable(i2cx->rcu_i2c_x);

//    i2c_deinit(i2cx->i2c_x);
//    
//    driver_gpio_general_init(i2cx->i2c_scl_gpio);
//    driver_gpio_general_init(i2cx->i2c_sda_gpio);
//    
//    /* I2C clock configure */
//    i2c_clock_config(i2cx->i2c_x, i2cx->frequency, I2C_DTCY_2);
//    /* I2C address configure */
//    i2c_mode_addr_config(i2cx->i2c_x, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, i2cx->slave_addr);
//    /* enable i2cx->i2c_x */
//    i2c_enable(i2cx->i2c_x);
//    /* enable acknowledge */
//    i2c_ack_config(i2cx->i2c_x, I2C_ACK_ENABLE); 
    
    rcu_periph_clock_enable(i2cx->rcu_i2c_x);
    
    if(i2cx->i2c_x==I2C0){
        idx_i2cx=IDX_I2C0;    
    }else if(i2cx->i2c_x==I2C1){
        idx_i2cx=IDX_I2C1;    
    }else if(i2cx->i2c_x==I2C2){
        idx_i2cx=IDX_I2C2;    
    }else if(i2cx->i2c_x==I2C3){
        idx_i2cx=IDX_I2C3;    
    }      
        
    rcu_i2c_clock_config(idx_i2cx,RCU_I2CSRC_APB1);
    
    apb1_freq = rcu_clock_freq_get(CK_APB1);
    
    freq_tick = apb1_freq/i2cx->frequency/6;
    
    i2c_deinit(i2cx->i2c_x);

    driver_gpio_general_init(i2cx->i2c_scl_gpio);
    driver_gpio_general_init(i2cx->i2c_sda_gpio);
    
    if(i2cx->i2c_mode==MODE_DMA)
    {
        if(i2cx->i2c_rx_dma!=NULL)
        {
            driver_dma_com_init(i2cx->i2c_rx_dma,(uint32_t)&I2C_RDATA(i2cx->i2c_x),NULL,DMA_Width_8BIT,DMA_PERIPH_TO_MEMORY);
            
        }
        if(i2cx->i2c_tx_dma!=NULL)
        {
            driver_dma_com_init(i2cx->i2c_tx_dma,(uint32_t)&I2C_TDATA(i2cx->i2c_x),NULL,DMA_Width_8BIT,DMA_MEMORY_TO_PERIPH);           
        }            
    }    

    scl_dely=freq_tick*3/10/2;
    if(scl_dely>0xff)
    {
        scl_dely=0xff;
    }
    
    sclh=freq_tick*3/10;
    if(sclh>0xf)
    {
        sclh=0xf;
    }

    scll=freq_tick*7/10;
    if(scll>0xf)
    {
        scll=0xf;
    }
    
    /* configure I2C timing */
    i2c_timing_config(i2cx->i2c_x, 5, scl_dely, 0);
    i2c_master_clock_config(i2cx->i2c_x, sclh, scll);
    /* enable I2C */
    i2c_enable(i2cx->i2c_x);    
}


//I2C����ж�
/*
*˵�� :i2c�¼���buff�жϴ������̺�������i2c�ж������
*����[1]              i2cx��i2cx�ṹ��ָ��    
*����ֵ               ��
*/
void driver_i2c_slave_int_handler(typdef_i2c_struct *i2cx)
{
    uint8_t temp=0;
    if(i2c_interrupt_flag_get(i2cx->i2c_x, I2C_INT_FLAG_ADDSEND)) 
    {
        i2cx->i2c_control.Com_Flag.Bits.Dir=i2c_flag_get(i2cx->i2c_x,I2C_FLAG_TR);
        if(i2cx->i2c_control.Com_Flag.Bits.Dir==RESET)
        {
            i2cx->i2c_control.Com_Flag.Bits.RecState=1;
            i2cx->i2c_control.Com_Flag.Bits.RecSuccess=0;
            i2cx->i2c_control.Com_Flag.Bits.SendState=0;
            i2cx->i2c_control.Com_Flag.Bits.SendSuccess=0;
            i2cx->i2c_control.RecCount=0;           
        }
        else
        {
            i2cx->i2c_control.Com_Flag.Bits.RecState=0;
            i2cx->i2c_control.Com_Flag.Bits.RecSuccess=0;
            i2cx->i2c_control.Com_Flag.Bits.SendState=1;
            i2cx->i2c_control.Com_Flag.Bits.SendSuccess=0;           
            i2cx->i2c_control.SendCount=0;
        }
        if(i2cx->i2c_slave_addr_callback!=NULL){
            i2cx->i2c_slave_addr_callback(i2cx);        
        }
        /* clear the ADDSEND bit */
        i2c_interrupt_flag_clear(i2cx->i2c_x, I2C_INT_FLAG_ADDSEND);
    } 
   
    
    if(i2cx->i2c_control.Com_Flag.Bits.Dir==RESET)//����
    {
        if(i2c_interrupt_flag_get(i2cx->i2c_x, I2C_INT_FLAG_RBNE)!=RESET){
            
            temp = i2c_data_receive(i2cx->i2c_x);            
            if(i2cx->i2c_control.RecCount < i2cx->i2c_control.RecSize)
            {              
                /* if reception data register is not empty, I2C1 will read a data from I2C_RDATA */
                i2cx->i2c_control.p_Rec[i2cx->i2c_control.RecCount] = temp;                
                i2cx->i2c_control.RecCount++;
            }
        }
    }
    else  if(i2cx->i2c_control.Com_Flag.Bits.Dir==SET)//����
    {
        if(i2c_interrupt_flag_get(i2cx->i2c_x, I2C_INT_FLAG_TI)!=RESET){
            /* send a data byte */
            if(i2cx->i2c_control.SendCount < i2cx->i2c_control.SendSize)
            {
                /* if reception data register is not empty, I2C1 will read a data from I2C_RDATA */
                i2c_data_transmit(i2cx->i2c_x,i2cx->i2c_control.p_Send[i2cx->i2c_control.SendCount]);
                i2cx->i2c_control.SendCount++;
            }
            else
            {
                i2c_data_transmit(i2cx->i2c_x,0xff);        
            }
        }   
    }    
    else if(i2c_interrupt_flag_get(i2cx->i2c_x, I2C_INT_FLAG_STPDET)) 
    {
        /* clear STPDET interrupt flag */
        i2c_interrupt_flag_clear(i2cx->i2c_x, I2C_INT_FLAG_STPDET);
        
        if(i2cx->i2c_control.Com_Flag.Bits.Dir==RESET)
        {
            i2cx->i2c_control.Com_Flag.Bits.SendSuccess = DRV_SUCCESS;
            i2cx->i2c_control.Com_Flag.Bits.RecState = 0;
        }
        else
        {
            i2cx->i2c_control.Com_Flag.Bits.SendSuccess = DRV_SUCCESS;
            i2cx->i2c_control.Com_Flag.Bits.SendState = 0;  
        }
        
        if(i2cx->i2c_slave_end_callback!=NULL){
            i2cx->i2c_slave_end_callback(i2cx);        
        }                
    }    
}

/*
*˵�� :i2c�����жϴ������̺�������i2c�ж������
*����[1]              i2cx��i2cx�ṹ��ָ��    
*����ֵ               ��
*/
void driver_i2c_err_int_handler(typdef_i2c_struct *i2cx)
{
    /* SMBus alert */
    if(i2c_interrupt_flag_get(i2cx->i2c_x, I2C_INT_FLAG_SMBALT)){
        i2c_interrupt_flag_clear(i2cx->i2c_x, I2C_INT_FLAG_SMBALT);
        if(i2cx->i2c_err_callback!=NULL)
        {
            i2cx->i2c_err_callback(i2cx);        
        }        
    }

    /* bus timeout in SMBus mode */
    if(i2c_interrupt_flag_get(i2cx->i2c_x, I2C_INT_FLAG_TIMEOUT)){
        i2c_interrupt_flag_clear(i2cx->i2c_x, I2C_INT_FLAG_TIMEOUT);
        if(i2cx->i2c_err_callback!=NULL)
        {
            i2cx->i2c_err_callback(i2cx);        
        }        
    }

    /* over-run or under-run when SCL stretch is disabled */
    if(i2c_interrupt_flag_get(i2cx->i2c_x, I2C_INT_FLAG_OUERR)){
       i2c_interrupt_flag_clear(i2cx->i2c_x, I2C_INT_FLAG_OUERR);
        if(i2cx->i2c_err_callback!=NULL)
        {
            i2cx->i2c_err_callback(i2cx);        
        }            
    }

    /* arbitration lost */
    if(i2c_interrupt_flag_get(i2cx->i2c_x, I2C_INT_FLAG_LOSTARB)){
        i2c_interrupt_flag_clear(i2cx->i2c_x, I2C_INT_FLAG_LOSTARB);
        if(i2cx->i2c_err_callback!=NULL)
        {
            i2cx->i2c_err_callback(i2cx);        
        }        
    }

    /* bus DRV_ERROR */
    if(i2c_interrupt_flag_get(i2cx->i2c_x, I2C_INT_FLAG_BERR)){
        i2c_interrupt_flag_clear(i2cx->i2c_x, I2C_INT_FLAG_BERR);
        if(i2cx->i2c_err_callback!=NULL)
        {
            i2cx->i2c_err_callback(i2cx);        
        }        
    }

    /* CRC value doesn't match */
    if(i2c_interrupt_flag_get(i2cx->i2c_x, I2C_INT_FLAG_PECERR)){
        i2c_interrupt_flag_clear(i2cx->i2c_x, I2C_INT_FLAG_PECERR);
        if(i2cx->i2c_err_callback!=NULL)
        {
            i2cx->i2c_err_callback(i2cx);        
        }        
    } 
      
}
