/*!
 * �ļ����ƣ� bsp_key.c
 * ��    ���� ���������ļ�
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

#include "bsp_key.h"
#include "driver_public.h"

////�û��жϻص���������
//__weak void KEY0_IRQHandler(typdef_gpio_general *KEYx_IO);

//��������ע��
//���������ٿ������˲�ʱ�䣬������ά�ṹ�����Ӱ�������������簴��ʱ����ɨ�����ڡ��˲�ʱ���
KEY_DEF(USER_KEY,A,1,IN_PU,SET,NULL);              // PA1����ΪUSER_KEY���ж�ģʽ,Ĭ��״̬��
KEY_DEF(WKUP_KEY,A,0,IN_PD,RESET,NULL);              // PA0����ΪWKUP_KEY���ж�ģʽ,Ĭ��״̬��

/* �����鶨�� */
const void* KEY_INIT_GROUP[]={&USER_KEY,&WKUP_KEY}; 
/* �����鳤�ȼ��� */
#define KEY_INIT_SIZE  sizeof(KEY_INIT_GROUP)/sizeof(void*)    
/* ��ʼ������ע���KEY */


__IO FlagStatus all_key_up=RESET;
/*!
* ˵��     KEYע�����ʼ������
* ����     ��
* ����ֵ   ��
*/
void bsp_key_group_init(void)
{
    uint8_t i;
    for(i=0;i<KEY_INIT_SIZE;i++)
    {
        bsp_key_init(((typdef_bsp_key *)KEY_INIT_GROUP[i]));    
    }
}

/*!
* ˵��     KEY��ʼ������
* ����[1]  KEYx��KEY�ṹ��ָ�� @USER_KEY/USER_KEY/KEY2/WKUP
* ����ֵ   ��
*/
void bsp_key_init(typdef_bsp_key *KEYx)
{
    driver_gpio_general_init(KEYx->key_gpio);
}

/*!
* ˵��     key״̬��ȡ
* ����[1]  KEYx��KEY�ṹ��ָ�� @USER_KEY/USER_KEY/KEY2/WKUP
* ����ֵ   ����״̬����������£�����SET���򷵻�RESET.
*/
bit_status bsp_key_state_get(typdef_bsp_key *KEYx)
{  
    if(dvire_gpio_pin_filter_get(KEYx->key_gpio,10)!=KEYx->key_gpio->default_state)
        return SET;
    else
        return RESET;
        
}

/*!
* ˵��     keyɨ��
* ����[1]  scan_ms_cycle������ɨ�������ʱ�䣨��λms��
* ����ֵ   ��������״̬������������º��𷵻�SET.
*/
bit_status key_scan(uint16_t scan_ms_cycle)
{   
    uint8_t i;
    bit_status press_flag=RESET;
    for(i=0;i<KEY_INIT_SIZE;i++)
    {
//        ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_timerms=press_none;        
        if(bsp_key_state_get((typdef_bsp_key *)KEY_INIT_GROUP[i])==SET && ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count<0xffff){
                
            ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_timerms =PRESS_DOWN;
            ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count+=scan_ms_cycle;
        }               
    }    
    
    for(i=0;i<KEY_INIT_SIZE;i++)
    {
       
        if(bsp_key_state_get((typdef_bsp_key *)KEY_INIT_GROUP[i])==SET  && ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count < 15000) //����15s�����±��Ϊ����������        
        {
            return press_flag;
        }
    }

    for(i=0;i<KEY_INIT_SIZE;i++)
    {   
        if( (((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count > PRESS_50MS) && (((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count < 15000) )
        {
            press_flag=SET;
            all_key_up=SET;
            if(((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count > PRESS_5000MS){
                ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_timerms=PRESS_5000MS;
            }         
            else if(((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count>PRESS_4000MS){
                ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_timerms=PRESS_4000MS;
            }        
            else if(((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count>PRESS_3000MS){
                ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_timerms=PRESS_3000MS;
            }
            else if(((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count>PRESS_2000MS){
                ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_timerms=PRESS_2000MS;
            }
            else if(((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count>PRESS_1000MS){
                ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_timerms=PRESS_1000MS;
            }
            else if(((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count>PRESS_500MS){
                ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_timerms=PRESS_500MS;
            }
            else if(((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count>PRESS_200MS){
                ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_timerms=PRESS_200MS;
            }   
            else{
                ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_timerms=PRESS_50MS;
            }
        }        
        if(bsp_key_state_get((typdef_bsp_key *)KEY_INIT_GROUP[i])==RESET){         
            ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count=0;   
        }
    }    
    return press_flag;        
}

void key_scan_1ms_callhandle(void)
{
    key_scan(1);
}

void key_scan_10ms_callhandle(void)
{
    key_scan(10);
}

void EXTI1_IRQHandler(void)
{
    dvire_gpio_exti_handle(USER_KEY.key_gpio);
}

//void EXTI4_IRQHandler(void)
//{
//    dvire_gpio_exti_handle(KEY2.key_gpio);
//}

void EXTI0_IRQHandler(void)
{
    dvire_gpio_exti_handle(WKUP_KEY.key_gpio);
}
