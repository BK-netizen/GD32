/*!
 * 文件名称： bsp_key.c
 * 描    述： 按键驱动文件
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

#include "bsp_key.h"
#include "driver_public.h"

////用户中断回调函数声明
//__weak void KEY0_IRQHandler(typdef_gpio_general *KEYx_IO);

//按键定义注册
//按键可以再考虑下滤波时间，做个二维结构体增加按键所需参数，如按下时长、扫描周期、滤波时间等
KEY_DEF(USER_KEY,A,1,IN_PU,SET,NULL);              // PA1定义为USER_KEY非中断模式,默认状态高
KEY_DEF(WKUP_KEY,A,0,IN_PD,RESET,NULL);              // PA0定义为WKUP_KEY非中断模式,默认状态高

/* 按键组定义 */
const void* KEY_INIT_GROUP[]={&USER_KEY,&WKUP_KEY}; 
/* 按键组长度计算 */
#define KEY_INIT_SIZE  sizeof(KEY_INIT_GROUP)/sizeof(void*)    
/* 初始化所有注册的KEY */


__IO FlagStatus all_key_up=RESET;
/*!
* 说明     KEY注册组初始化函数
* 输入     无
* 返回值   无
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
* 说明     KEY初始化函数
* 输入[1]  KEYx：KEY结构体指针 @USER_KEY/USER_KEY/KEY2/WKUP
* 返回值   无
*/
void bsp_key_init(typdef_bsp_key *KEYx)
{
    driver_gpio_general_init(KEYx->key_gpio);
}

/*!
* 说明     key状态读取
* 输入[1]  KEYx：KEY结构体指针 @USER_KEY/USER_KEY/KEY2/WKUP
* 返回值   按键状态，如果被按下，返回SET否则返回RESET.
*/
bit_status bsp_key_state_get(typdef_bsp_key *KEYx)
{  
    if(dvire_gpio_pin_filter_get(KEYx->key_gpio,10)!=KEYx->key_gpio->default_state)
        return SET;
    else
        return RESET;
        
}

/*!
* 说明     key扫描
* 输入[1]  scan_ms_cycle：调用扫描的周期时间（单位ms）
* 返回值   按键弹起状态，如果按键按下后弹起返回SET.
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
       
        if(bsp_key_state_get((typdef_bsp_key *)KEY_INIT_GROUP[i])==SET  && ((typdef_bsp_key *)KEY_INIT_GROUP[i])->press_cycle_count < 15000) //持续15s被按下标记为按键可能损坏        
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
