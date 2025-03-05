/*!
 * 文件名称： bsp_key.h
 * 描    述： 按键驱动文件
 * 版本：     2023-12-03, V1.0
*/

/*
* GD32H757海棠派开发板V1.0
* 淘宝商城：   https://juwo.taobao.com
* 技术交流：   https://www.gd32bbs.com/ask/
* 视频学习：   https://space.bilibili.com/475462605
* 微信公众号： gd32bbs
* Copyright    苏州聚沃电子科技有限公司
* 版权所有，盗版必究。
*/

#ifndef BSP_KEY_H
#define BSP_KEY_H

#include "driver_public.h"
#include "driver_gpio.h"
          
#define KEY_FILTER_DEFAULT 10

extern __IO FlagStatus all_key_up;
     
typedef enum 
{
    PRESS_NONE   =0,
    PRESS_DOWN   =1,
    PRESS_50MS   = 50,
    PRESS_200MS  = 200,    
    PRESS_500MS  = 500,    
    PRESS_1000MS = 1000,    
    PRESS_2000MS = 2000,    
    PRESS_3000MS = 3000,    
    PRESS_4000MS = 4000,    
    PRESS_5000MS = 5000,      
}key_PRESS_timerms_enum;

typedef struct __typdef_bsp_key
{
    typdef_gpio_general *key_gpio;
    uint16_t key_filter_num;
    uint16_t press_cycle_count;
    key_PRESS_timerms_enum  press_timerms;        
}typdef_bsp_key;

#define KEY_DEF(name,port,pin,mode,default_state,int_callback)    \
    GPIO_DEF(name##_GPIO,port,pin,mode,default_state,int_callback);\
    typdef_bsp_key name =                           \
    {                                               \
        .key_gpio=&name##_GPIO,                     \
        .key_filter_num=KEY_FILTER_DEFAULT,                       \
        .press_cycle_count=0,                       \
        .press_timerms=PRESS_NONE,                  \
    }
    
#define KEY_DEF_EXTERN(name) \
    extern  typdef_bsp_key name    

//KEY_DEF_EXTERN(USER_KEY);
KEY_DEF_EXTERN(USER_KEY);
//KEY_DEF_EXTERN(KEY2);
KEY_DEF_EXTERN(WKUP_KEY);
    
void bsp_key_init(  typdef_bsp_key *KEYx);
void bsp_key_group_init(void);
bit_status bsp_key_state_get(  typdef_bsp_key *KEYx);
bit_status key_scan(uint16_t scan_ms_cycle); 
void key_scan_1ms_callhandle(void);
void key_scan_10ms_callhandle(void);    
#endif /* BSP_KEY_H */
