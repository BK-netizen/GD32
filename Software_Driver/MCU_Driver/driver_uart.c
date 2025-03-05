/*!
 * �ļ����ƣ� driver_uart.c
 * ��    ���� uart �����ļ�
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

#include "driver_uart.h"

/*!  UART����ע��ʾ��
UART_TX_GPIO_DEF(BOARD_UART,B,6,GPIO_USART0_REMAP);
UART_RX_GPIO_DEF(BOARD_UART,B,6,GPIO_USART0_REMAP);
UART_TX_DMA_DEF(BOARD_UART,DMA0,DMA_CH3);
UART_RX_DMA_DEF(BOARD_UART,DMA0,DMA_CH4);
UART_DEF(BOARD_UART,USART0,115200U,USART_PM_NONE,USART_WL_8BIT,MODE_POLL,MODE_INT);

UART_TX_GPIO_DEF(MODULE_USART,B,10,NULL);
UART_RX_GPIO_DEF(MODULE_USART,B,11,NULL);
UART_TX_DMA_DEF(MODULE_USART,DMA0,DMA_CH1);
UART_RX_DMA_DEF(MODULE_USART,DMA0,DMA_CH2);
UART_DEF(MODULE_USART,USART2,115200U,USART_PM_NONE,USART_WL_8BIT,MODE_DMA,MODE_DMA);   
    

const void* UART_INIT_GROUP[]={&BOARD_UART,&MODULE_USART}; 

#define UART_INIT_SIZE  sizeof(UART_INIT_GROUP)/sizeof(void*)

*/

/*!
* ˵��     UART��ʼ������
* ����[1]  uartx��uart�ṹ��ָ�� 
* ����ֵ   ��
*/
void driver_uart_init(typdef_uart_struct *uartx)
{
		rcu_periph_clock_enable(uartx->rcu_uart_x);
		/* USART configure */
		usart_deinit(uartx->uart_x);

		driver_gpio_general_init(uartx->uart_rx_gpio);
		driver_gpio_general_init(uartx->uart_tx_gpio);	

    if(uartx->uart_mode_rx==MODE_DMA)
    {
        if(uartx->uart_rx_dma!=NULL)
        {
            driver_dma_com_init(uartx->uart_rx_dma,(uint32_t)&USART_RDATA(uartx->uart_x),NULL,DMA_Width_8BIT,DMA_PERIPH_TO_MEMORY);
            usart_interrupt_enable(uartx->uart_x,USART_INT_IDLE);
        }
    }
    
    if(uartx->uart_mode_tx==MODE_DMA)
    {    
        if(uartx->uart_tx_dma!=NULL)
        {
            driver_dma_com_init(uartx->uart_tx_dma,(uint32_t)&USART_TDATA(uartx->uart_x),NULL,DMA_Width_8BIT,DMA_MEMORY_TO_PERIPH);
    //      usart_interrupt_enable(uartx->uart_x,USART_INT_TC);
        }
    }
	
    usart_baudrate_set(uartx->uart_x, uartx->baudrate);
    usart_receive_config(uartx->uart_x, USART_RECEIVE_ENABLE);
    usart_transmit_config(uartx->uart_x, USART_TRANSMIT_ENABLE);
    usart_word_length_set(uartx->uart_x, uartx->data_length);
    usart_parity_config(uartx->uart_x, uartx->parity);
    
    usart_enable(uartx->uart_x);	
}


/*!
* ˵��     UART��ѵ���ͺ���
* ����[1]   uartx��uart�ṹ��ָ�� 
* ����[2]   pbuff    ��������ָ��
* ����[3]   length   �������ݳ���
* ����ֵ   ��
*/
Drv_Err driver_uart_poll_transmit(typdef_uart_struct *uartx,uint8_t *pbuff,uint16_t length)
{
    __IO uint64_t timeout = driver_tick;    
    while(uartx->uart_control.Com_Flag.Bits.SendState==1){
        if((timeout+UART_TIMEOUT_MS) <= driver_tick) {              
            uartx->uart_control.Com_Flag.Bits.SendState=0;
            return DRV_ERROR;        
        } 
    }    
    uartx->uart_control.Com_Flag.Bits.SendSuccess=0;
    uartx->uart_control.Com_Flag.Bits.SendState=1;    
    uartx->uart_control.p_Send=pbuff;
    uartx->uart_control.SendSize=length;
    uartx->uart_control.SendCount=0;
    
		for(uint16_t i=0;i<length;i++){
			if(DRV_ERROR==driver_uart_flag_wait_timeout(uartx,USART_FLAG_TBE,SET))
			{
				return DRV_ERROR;
			}
			usart_data_transmit(uartx->uart_x,pbuff[i]);
      uartx->uart_control.SendCount++;
		}

		if(DRV_ERROR==driver_uart_flag_wait_timeout(uartx,USART_FLAG_TC,SET))
		{
			return DRV_ERROR;
		}
    
    uartx->uart_control.Com_Flag.Bits.SendSuccess=1;
    uartx->uart_control.Com_Flag.Bits.SendState=0;     

		return DRV_SUCCESS;	
}


/*!
* ˵��     UART��ѵ���ܺ���
* ����[1]   uartx��uart�ṹ��ָ�� 
* ����[2]   pbuff    ��������ָ��
* ����[3]   length   �������ݳ���
* ����ֵ   ��
*/
Drv_Err driver_uart_poll_receive(typdef_uart_struct *uartx,uint8_t *pbuff,uint16_t length)
{    
    uartx->uart_control.Com_Flag.Bits.RecSuccess=0;
    uartx->uart_control.Com_Flag.Bits.RecState=1;
    uartx->uart_control.p_Rec=pbuff;
    uartx->uart_control.RecSize=length;
    uartx->uart_control.RecCount=0;    
    
    if(usart_flag_get(uartx->uart_x,USART_FLAG_ORERR))
    {
        usart_flag_clear(uartx->uart_x,USART_FLAG_ORERR);
//        USART_STAT0(uartx->uart_x);
//        USART_DATA(uartx->uart_x);        
    }
    for(uint16_t i=0;i<length;i++){
        if(DRV_ERROR==driver_uart_flag_wait_timeout(uartx,USART_FLAG_RBNE,SET))
        {
            return DRV_ERROR;
        }
        pbuff[i]=usart_data_receive(uartx->uart_x);
        uartx->uart_control.RecCount++;
    }
    
    uartx->uart_control.Com_Flag.Bits.RecSuccess=1;
    uartx->uart_control.Com_Flag.Bits.RecState=0;
    
	return DRV_SUCCESS;	
}

/*!
* ˵��     UART dma���ͺ���
* ����[1]   uartx��uart�ṹ��ָ�� 
* ����[2]   pbuff    ��������ָ��
* ����[3]   length   �������ݳ���
* ����ֵ   ��
*/
Drv_Err driver_uart_dma_transmit(typdef_uart_struct *uartx,uint8_t *pbuff,uint16_t length)
{
    Drv_Err uart_state=DRV_ERROR;
    
    __IO uint64_t timeout = driver_tick;    
    while(uartx->uart_control.Com_Flag.Bits.SendState==1){
        if((timeout+UART_TIMEOUT_MS) <= driver_tick) {              
            uartx->uart_control.Com_Flag.Bits.SendState=0;
            return DRV_ERROR;        
        } 
    }    
    
    uartx->uart_control.Com_Flag.Bits.SendSuccess=0;
    uartx->uart_control.Com_Flag.Bits.SendState=1;    
    uartx->uart_control.p_Send=pbuff;
    uartx->uart_control.SendSize=length;
    uartx->uart_control.SendCount=0;
    
    uart_state=driver_dma_flag_wait_timeout(uartx->uart_tx_dma,DMA_FLAG_FTF,SET); 
    
    usart_dma_transmit_config(uartx->uart_x,USART_TRANSMIT_DMA_DISABLE);
    
    driver_dma_start(uartx->uart_tx_dma,pbuff,length);
    
    usart_flag_clear(uartx->uart_x,USART_FLAG_TC);
    
    usart_dma_transmit_config(uartx->uart_x,USART_TRANSMIT_DMA_ENABLE);
    
    usart_interrupt_enable(uartx->uart_x,USART_INT_TC);
    

    return uart_state;    
}

/*!
* ˵��     UART��ѵ����һ������
* ����[1]   uartx��uart�ṹ��ָ�� 
* ����[2]   data    ��������ֵ
* ����ֵ   ��
*/
Drv_Err driver_uart_transmit_byte(typdef_uart_struct *uartx,uint8_t data)
{
    __IO uint64_t timeout = driver_tick;    
    while(uartx->uart_control.Com_Flag.Bits.SendState==1){
        if((timeout+UART_TIMEOUT_MS) <= driver_tick) {              
            uartx->uart_control.Com_Flag.Bits.SendState=0;
            return DRV_ERROR;        
        } 
    }
    
    Drv_Err uart_state=DRV_SUCCESS;    
    uartx->uart_control.Com_Flag.Bits.SendSuccess=0;
    uartx->uart_control.Com_Flag.Bits.SendState=1;    
    
    uart_state=driver_uart_flag_wait_timeout(uartx,USART_FLAG_TBE,SET);

    usart_data_transmit(uartx->uart_x,data);

    
    uartx->uart_control.Com_Flag.Bits.SendSuccess=1;
    uartx->uart_control.Com_Flag.Bits.SendState=0;     

    return uart_state;	  
}


/*!
* ˵��     UART dma ���ܺ���
* ����[1]   uartx��uart�ṹ��ָ�� 
* ����[2]   pbuff    ��������ָ��
* ����[3]   length   �������ݳ���
* ����ֵ   ��
*/
Drv_Err driver_uart_dma_receive(typdef_uart_struct *uartx,uint8_t *pbuff,uint16_t length)
{
    Drv_Err uart_state=DRV_SUCCESS; 
    __IO uint64_t timeout = driver_tick; 
    while(uartx->uart_control.Com_Flag.Bits.RecState==1){
        if((timeout+UART_TIMEOUT_MS) <= driver_tick) {              
            uartx->uart_control.Com_Flag.Bits.RecState=0;
            return DRV_ERROR;        
        } 
    }    

    uartx->uart_control.Com_Flag.Bits.RecSuccess=0;
    uartx->uart_control.Com_Flag.Bits.RecState=1;
    uartx->uart_control.p_Rec=pbuff;
    uartx->uart_control.RecSize=length;
    uartx->uart_control.RecCount=0;
    
    if(usart_flag_get(uartx->uart_x,USART_FLAG_ORERR))
    {
        usart_flag_clear(uartx->uart_x,USART_FLAG_ORERR);
//        USART_STAT0(uartx->uart_x);
//        USART_DATA(uartx->uart_x);        
    }    
       
    usart_dma_receive_config(uartx->uart_x,USART_RECEIVE_DMA_DISABLE);
    
    driver_dma_start(uartx->uart_rx_dma,pbuff,length);

//    USART_STAT0(uartx->uart_x);
//    usart_data_receive(uartx->uart_x);

    usart_interrupt_flag_clear(uartx->uart_x,USART_INT_FLAG_IDLE);
    
    usart_interrupt_enable(uartx->uart_x,USART_INT_IDLE);    
    
    usart_dma_receive_config(uartx->uart_x,USART_RECEIVE_DMA_ENABLE);  

    return uart_state;     
}

/*!
* ˵��     UART �жϷ��ͺ���
* ����[1]   uartx��uart�ṹ��ָ�� 
* ����[2]   pbuff    ��������ָ��
* ����[3]   length   �������ݳ���
* ����ֵ   ��
*/
Drv_Err driver_uart_int_transmit(typdef_uart_struct *uartx,uint8_t *pbuff,uint16_t length)
{
    __IO uint64_t timeout = driver_tick;    
    while(uartx->uart_control.Com_Flag.Bits.SendState==1){
        if((timeout+UART_TIMEOUT_MS) <= driver_tick) {              
            uartx->uart_control.Com_Flag.Bits.SendState=0;
            return DRV_ERROR;        
        } 
    }
    
    uartx->uart_control.Com_Flag.Bits.SendSuccess=0;
    uartx->uart_control.Com_Flag.Bits.SendState=1;  
    uartx->uart_control.p_Send=pbuff;
    uartx->uart_control.SendSize=length;
    uartx->uart_control.SendCount=0;
    
    usart_flag_clear(uartx->uart_x,USART_FLAG_TC);
    usart_interrupt_enable(uartx->uart_x,USART_INT_TBE);
    
    return DRV_SUCCESS;    
}


/*!
* ˵��     UART�жϽ��ܺ���
* ����[1]  uartx��uart�ṹ��ָ�� 
* ����[2]  pbuff    ��������ָ��
* ����[3]  length   �������ݳ���
* ����ֵ   ��
*/
Drv_Err driver_uart_int_receive(typdef_uart_struct *uartx,uint8_t *pbuff,uint16_t length)
{    
    __IO uint64_t timeout = driver_tick;     
    while(uartx->uart_control.Com_Flag.Bits.RecState==1){
        if((timeout+UART_TIMEOUT_MS) <= driver_tick) {              
            uartx->uart_control.Com_Flag.Bits.RecState=0;
            return DRV_ERROR;        
        } 
    }

    if(usart_flag_get(uartx->uart_x,USART_FLAG_ORERR))
    {
        usart_flag_clear(uartx->uart_x,USART_FLAG_ORERR);
//        USART_STAT0(uartx->uart_x);
//        USART_DATA(uartx->uart_x);        
    }    
    
    uartx->uart_control.Com_Flag.Bits.RecSuccess=0;
    uartx->uart_control.Com_Flag.Bits.RecState=1;
    uartx->uart_control.p_Rec=pbuff;
    uartx->uart_control.RecSize=length;
    uartx->uart_control.RecCount=0;    

    usart_flag_clear(uartx->uart_x,USART_FLAG_IDLE);
//    USART_STAT0(uartx->uart_x);
//    USART_RDATA(uartx->uart_x);  
    
    usart_interrupt_enable(uartx->uart_x,USART_INT_RBNE);
    usart_interrupt_enable(uartx->uart_x,USART_INT_IDLE);
    
    return DRV_SUCCESS;    
}


/*!
* ˵��     UART��־��ʱ�ȴ�
* ����[1]  uartx��uart�ṹ��ָ�� 
* ����[2]  flag    Ҫ�ȴ��ı�־
* ����[3]  wait_state   Ҫ�ȴ���״̬
* ����ֵ   ��
*/
Drv_Err driver_uart_flag_wait_timeout(typdef_uart_struct *uartx, usart_flag_enum flag ,FlagStatus wait_state)
{
    __IO uint64_t timeout = driver_tick;    
    while(wait_state!=usart_flag_get(uartx->uart_x, flag)){
        if((timeout+UART_TIMEOUT_MS) <= driver_tick) {              
            return DRV_ERROR;
        } 
    }
    return DRV_SUCCESS;
}

/*!
* ˵��     UART�жϴ�����
* ����[1]  uartx��uart�ṹ��ָ�� 
* ����ֵ   ��
*/
Drv_Err driver_uart_int_handler(typdef_uart_struct *uartx)
{   
    Drv_Err uart_state=DRV_SUCCESS;    
    if(usart_interrupt_flag_get(uartx->uart_x,USART_INT_FLAG_RBNE)!=RESET)
    {
        if(uartx->uart_control.RecCount < uartx->uart_control.RecSize){
            uartx->uart_control.p_Rec[uartx->uart_control.RecCount]=usart_data_receive(uartx->uart_x); 
            uartx->uart_control.RecCount++;            
        }
        else{
            usart_data_receive(uartx->uart_x);
            uart_state=DRV_ERROR;
            //err ���
        }
        if(uartx->uart_rbne_callback!=NULL){
            uartx->uart_rbne_callback(uartx);
        }        
        //callback
        if(uartx->uart_control.RecCount == uartx->uart_control.RecSize){
            uartx->uart_control.Com_Flag.Bits.RecSuccess=1;            
            uartx->uart_control.Com_Flag.Bits.RecState=0; 
            uartx->uart_control.RecCount=0;            
        }        
    }       
    if(usart_interrupt_flag_get(uartx->uart_x,USART_INT_FLAG_IDLE)!=RESET)
    {
        usart_interrupt_flag_clear(uartx->uart_x,USART_INT_FLAG_IDLE);
//        USART_STAT0(uartx->uart_x);
//        USART_DATA(uartx->uart_x);
        
        if( (uartx->uart_mode_rx==MODE_INT && uartx->uart_control.RecCount>0) \
            ||(uartx->uart_mode_rx==MODE_DMA && dma_transfer_number_get(uartx->uart_rx_dma->dmax,uartx->uart_rx_dma->dma_chx)!=uartx->uart_control.RecSize))
        {
            uartx->uart_control.Com_Flag.Bits.RecSuccess=1;
            uartx->uart_control.Com_Flag.Bits.RecState=0;              
            
            if(uartx->uart_mode_rx==MODE_DMA){
                uartx->uart_control.RecCount=uartx->uart_control.RecSize-dma_transfer_number_get(uartx->uart_rx_dma->dmax,uartx->uart_rx_dma->dma_chx);            
            }
            //callback            
            if(uartx->uart_idle_callback!=NULL){
                uartx->uart_idle_callback(uartx);
            }                                
        }        
            
    } 
    
    if(usart_interrupt_flag_get(uartx->uart_x,USART_INT_FLAG_TBE)!=RESET)
    {
        usart_data_transmit(uartx->uart_x,uartx->uart_control.p_Send[uartx->uart_control.SendCount]);
        uartx->uart_control.SendCount++;
        
        if(uartx->uart_tbe_callback!=NULL){
            uartx->uart_tbe_callback(uartx);
        } 
        
        if(uartx->uart_control.SendCount >= uartx->uart_control.SendSize)
        {
            uartx->uart_control.SendCount=0;            
            usart_interrupt_disable(uartx->uart_x, USART_INT_TBE);
            usart_interrupt_enable(uartx->uart_x, USART_INT_TC);
        }
    } 

    if(usart_interrupt_flag_get(uartx->uart_x,USART_INT_FLAG_TC)!=RESET)
    {
        usart_interrupt_disable(uartx->uart_x, USART_INT_TC);          
        usart_flag_clear(uartx->uart_x,USART_FLAG_TC);  

        if( !(uartx->uart_mode_rx==MODE_DMA && dma_transfer_number_get(uartx->uart_tx_dma->dmax,uartx->uart_tx_dma->dma_chx)!=0) )
        {    
            uartx->uart_control.Com_Flag.Bits.SendSuccess=1;            
            uartx->uart_control.Com_Flag.Bits.SendState=0; 
            
            if(uartx->uart_tc_callback!=NULL){
                uartx->uart_tc_callback(uartx);
            }         
            
            uartx->uart_control.SendCount=0; 
        }
    }  

    if(usart_flag_get(uartx->uart_x,USART_FLAG_ORERR)==SET)
    {
        usart_flag_clear(uartx->uart_x,USART_FLAG_ORERR);        
//        USART_STAT0(uartx->uart_x);
//        USART_DATA(uartx->uart_x);
        uart_state=DRV_ERROR;
    }

    return uart_state;     

}

