/*!
 * �ļ����ƣ� bsp_uart.c
 * ��    ���� bsp uart interface
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

#include "bsp_uart.h"
#include "driver_uart.h"


//UART����ע��
UART_TX_GPIO_DEF(BOARD_UART,A,9,GPIO_AF_7);
UART_RX_GPIO_DEF(BOARD_UART,A,10,GPIO_AF_7);
UART_TX_DMA_DEF(BOARD_UART,USART0,DMA1,DMA_CH7);
UART_RX_DMA_DEF(BOARD_UART,USART0,DMA1,DMA_CH5);
UART_DEF(BOARD_UART,USART0,115200U,USART_PM_NONE,USART_WL_8BIT,MODE_POLL,MODE_POLL);

UART_TX_GPIO_DEF(MODULE_USART,B,10,GPIO_AF_7);
UART_RX_GPIO_DEF(MODULE_USART,B,11,GPIO_AF_7);
UART_TX_DMA_DEF(MODULE_USART,USART2,DMA0,DMA_CH3);
UART_RX_DMA_DEF(MODULE_USART,USART2,DMA0,DMA_CH1);
UART_DEF(MODULE_USART,USART2,115200U,USART_PM_NONE,USART_WL_8BIT,MODE_DMA,MODE_DMA);   

GPIO_DEF(RS485_DIR,G,15,OUT_PP,RESET,NULL);
    

const void* UART_INIT_GROUP[]={&BOARD_UART,&MODULE_USART};

#define UART_INIT_SIZE  sizeof(UART_INIT_GROUP)/sizeof(void*)

void rs485_transmit_complete_callback(typdef_uart_struct *uartx);


/*!
* ˵��     ����UART��ʼ������
* ����[1]  uartx��uart�ṹ��ָ�� @BOARD_UART/MODULE_USART
* ����ֵ   ��
*/
void bsp_uart_init(typdef_uart_struct *uartx)
{
    driver_uart_init(uartx);
}

/*!
* ˵��     ��ʼ��ȫ��ע��UART
* ����[1]  ��
* ����ֵ   ��
*/
void bsp_uart_group_init(void)
{
    uint8_t i;
    for(i=0;i<UART_INIT_SIZE;i++)
    {
        bsp_uart_init(((typdef_uart_struct *)UART_INIT_GROUP[i]));    
    }
}

/*!
* ˵��     ��ʼ��485
* ����[1]  ��
* ����ֵ   ��
*/
void bsp_rs485_uart_init(void)
{
    driver_gpio_general_init(&RS485_DIR);   
    driver_gpio_pin_reset(&RS485_DIR);     
    driver_uart_init(&BOARD_UART);
    usart_halfduplex_enable(BOARD_UART.uart_x);
    usart_receive_config(BOARD_UART.uart_x, USART_RECEIVE_ENABLE);
    usart_transmit_config(BOARD_UART.uart_x, USART_TRANSMIT_DISABLE);
    BOARD_UART.uart_tc_callback=rs485_transmit_complete_callback;
}

/*!
* ˵��     485 ����
* ����[1]  pbuff    ��������ָ��
* ����[2]  length   �������ݳ���
* ����ֵ   ��
*/
void bsp_rs485_uart_transmit(uint8_t *pbuff,uint16_t length)
{
    __IO uint64_t timeout = driver_tick;    
    while(BOARD_UART.uart_control.Com_Flag.Bits.RecState==1 && BOARD_UART.uart_control.RecCount!=0){
        if((timeout+UART_TIMEOUT_MS) <= driver_tick) {              
            BOARD_UART.uart_control.Com_Flag.Bits.RecState=0;
        } 
    }
    
    driver_gpio_pin_set(&RS485_DIR);  
    usart_receive_config(BOARD_UART.uart_x, USART_RECEIVE_DISABLE);
    usart_transmit_config(BOARD_UART.uart_x, USART_TRANSMIT_ENABLE);        

    
    if(BOARD_UART.uart_mode_tx==MODE_DMA)      
    {
        driver_uart_dma_transmit(&BOARD_UART,pbuff,length);
    }
    else if(BOARD_UART.uart_mode_tx==MODE_INT)
    {
        driver_uart_int_transmit(&BOARD_UART,pbuff,length);        
    }
    else if(BOARD_UART.uart_mode_tx==MODE_POLL)
    {
        driver_uart_poll_transmit(&BOARD_UART,pbuff,length); 
        
        usart_receive_config(BOARD_UART.uart_x, USART_RECEIVE_ENABLE);
        usart_transmit_config(BOARD_UART.uart_x, USART_TRANSMIT_DISABLE);          
        driver_gpio_pin_reset(&RS485_DIR);      
    }
}


/*!
* ˵��     485 ����
* ����[1]  pbuff    ��������ָ��
* ����[2]  length   �����������������
* ����ֵ   ��
*/
void bsp_rs485_uart_receive(uint8_t *pbuff,uint16_t length)
{
    __IO uint64_t timeout = driver_tick;    
    while(BOARD_UART.uart_control.Com_Flag.Bits.SendState==1){
        if((timeout+UART_TIMEOUT_MS) <= driver_tick) {              
            BOARD_UART.uart_control.Com_Flag.Bits.SendState=0;
        } 
    }
    
    usart_receive_config(BOARD_UART.uart_x, USART_RECEIVE_ENABLE);
    usart_transmit_config(BOARD_UART.uart_x, USART_TRANSMIT_DISABLE);  
    
    driver_gpio_pin_reset(&RS485_DIR);
  
    
    if(BOARD_UART.uart_mode_rx==MODE_DMA)
    {
        driver_uart_dma_receive(&BOARD_UART,pbuff,length);
    }
    else if(BOARD_UART.uart_mode_rx==MODE_INT)
    {
        driver_uart_int_receive(&BOARD_UART,pbuff,length);        
    }
    else if(BOARD_UART.uart_mode_rx==MODE_POLL)
    {
        driver_uart_poll_receive(&BOARD_UART,pbuff,length); 
    }
}

/*!
* ˵��     485������ɻص�
* ����[1]  uartx��uart�ṹ��ָ�� @BOARD_UART/MODULE_USART
* ����ֵ   ��
*/
void rs485_transmit_complete_callback(typdef_uart_struct *uartx)
{
    usart_transmit_config(BOARD_UART.uart_x, USART_TRANSMIT_DISABLE);    
    usart_receive_config(BOARD_UART.uart_x, USART_RECEIVE_ENABLE);    
    driver_gpio_pin_reset(&RS485_DIR);    
}

//void rs485_receive_complete_callback(void)
//{
//    bsp_rs485_uart_transmit(&BOARD_UART,BOARD_UART.uart_control.p_Rec,BOARD_UART.uart_control.RecCount);    
//}

/*!
* ˵��     UASRT0�жϷ�����
* ����[1]  uartx��uart�ṹ��ָ�� @BOARD_UART/MODULE_USART
* ����ֵ   ��
*/
void USART0_IRQHandler(void)
{
    driver_uart_int_handler(&BOARD_UART);
}

/*!
* ˵��     UASRT2�жϷ�����
* ����[1]  uartx��uart�ṹ��ָ�� @BOARD_UART/MODULE_USART
* ����ֵ   ��
*/
void USART2_IRQHandler(void)
{
    driver_uart_int_handler(&MODULE_USART);
}
           

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{   
    driver_uart_transmit_byte(&BOARD_UART,(uint8_t)ch);    
    return ch;
}
