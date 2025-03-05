/*!
 * 文件名称： bsp_touch_panel.c
 * 描    述： 电阻屏触摸驱动文件
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
#include "driver_public.h"
#include "bsp_touch_panel.h"
#include "bsp_spi.h"
#include "driver_gpio.h"

GPIO_DEF(TFT_LCD_TOUCH_CS,A,8,OUT_PP,SET,NULL);
GPIO_DEF(LCD_TOUCH_BUSY,B,15,IN_PD, RESET,NULL);

GPIO_DEF(LCD_PEN_INT,D,11,IN_PU,SET,NULL);


#define TFT_LCD_TOUCH_UNLOCK()  touch_spi_device.spi_device_state_bits.device0=SET
#define TFT_LCD_TOUCH_LOCK()    touch_spi_device.spi_device_state_bits.device0=RESET

#define RGB_LCD_TOUCH_UNLOCK()  touch_spi_device.spi_device_state_bits.device1=SET
#define RGB_LCD_TOUCH_LOCK()    touch_spi_device.spi_device_state_bits.device1=RESET


typde_lcd_touch_panel_struct lcd_touch_panel_struct;


static void bsp_spi_touch_cs_low(void)
{
    bsp_spi_device_busy_wait(&touch_spi_device); 
    TFT_LCD_TOUCH_UNLOCK();
    /* select the flash: chip select low */
    driver_gpio_pin_reset(&TFT_LCD_TOUCH_CS);   
}  

static void bsp_spi_touch_cs_high(void)
{
    /* select the flash: chip select high */
    driver_gpio_pin_set(&TFT_LCD_TOUCH_CS);  
    TFT_LCD_TOUCH_LOCK();   
}  


/*!
    \brief      touch panel gpio configure
    \param[in]  none 
    \param[out] none
    \retval     none
*/
void bsp_touch_panel_init(uint16_t lcd_width,uint16_t lcd_height)
{    
    driver_gpio_general_init(&TFT_LCD_TOUCH_CS);
    driver_gpio_general_init(&LCD_PEN_INT);
//    bsp_spi_init(&TOUCH_SPI);
    
    /* Set chip select pin high */
    bsp_spi_touch_cs_high();
    
    lcd_touch_panel_struct.AD_Bottom=AD_Bottom_Default;
    lcd_touch_panel_struct.AD_Top   =AD_Top_Default;
    lcd_touch_panel_struct.AD_Right =AD_Right_Default;
    lcd_touch_panel_struct.AD_Left  =AD_Left_Default;
    
    lcd_touch_panel_struct.LCD_X =lcd_width;    
    lcd_touch_panel_struct.LCD_Y =lcd_height; 

    lcd_touch_panel_struct.TOUCH_State=0;  

    lcd_touch_panel_struct.Touch_y_Last=lcd_width/2;
    lcd_touch_panel_struct.Touch_y_Last=lcd_height/2;
}


/*!
    \brief      read the touch pen interrupt request signal
    \param[in]  none
    \param[out] none
    \retval     the status of touch pen: SET or RESET
      \arg        SET: touch pen is inactive
      \arg        RESET: touch pen is active
*/
FlagStatus bsp_touch_pen_irq(void)
{
    if(dvire_gpio_pin_filter_get(&LCD_PEN_INT,10)!=LCD_PEN_INT.default_state)
        return SET;
    else
        return RESET;    
}

/*!
    \brief      get the AD sample value of touch location at X coordinate
    \param[in]  none
    \param[out] none
    \retval     channel X+ AD sample value
*/
uint16_t bsp_touch_ad_get(uint8_t cmd)
{
    __IO uint16_t temp;
    if (RESET == bsp_touch_pen_irq()){
        /* touch pen is inactive */
        return 0;
    }

    bsp_spi_touch_cs_low();

    driver_spi_master_transmit_receive_byte(&TOUCH_SPI,cmd);
    
//    while(dvire_gpio_pin_filter_get(&LCD_TOUCH_BUSY,10)==SET);

    delay_us(1);

    temp=(uint16_t)driver_spi_master_transmit_receive_byte(&TOUCH_SPI,0)<<8;  
    temp|=(uint16_t)driver_spi_master_transmit_receive_byte(&TOUCH_SPI,0);

    temp&=(~BIT(15));
    temp>>=3;    
    bsp_spi_touch_cs_high();
    return temp;
}


/*!
    \brief      get channel X+ AD average sample value
    \param[in]  none
    \param[out] none
    \retval     channel X+ AD average sample value
*/
uint16_t bsp_touch_average_ad_get(uint8_t cmd)
{
    uint8_t i;
    uint16_t temp=0;
    for (i=0;i<8;i++){
        temp+=bsp_touch_ad_get(cmd);
        delay_us(10);
    }
    temp>>=3;
    
    return temp;
}

/*!
    \brief      get X coordinate value of touch point on LCD screen
    \param[in]  adx : channel X+ AD average sample value
    \param[out] none
    \retval     X coordinate value of touch point
*/
uint16_t bsp_touch_coordinate_x_get(uint16_t adx)
{
    uint16_t sx = 0;
    uint32_t
    r = adx - lcd_touch_panel_struct.AD_Left;
    r *= lcd_touch_panel_struct.LCD_X - 1;
    sx =  r / (lcd_touch_panel_struct.AD_Right - lcd_touch_panel_struct.AD_Left);
    if (sx <= 0 || sx > lcd_touch_panel_struct.LCD_X)
        return 0;
    return sx;
}

/*!
    \brief      get Y coordinate value of touch point on LCD screen
    \param[in]  ady : channel Y+ AD average sample value
    \param[out] none
    \retval     Y coordinate value of touch point
*/
uint16_t bsp_touch_coordinate_y_get(uint16_t ady)
{
    uint16_t sy = 0;
    uint32_t
    r = ady - lcd_touch_panel_struct.AD_Top;
    r *= lcd_touch_panel_struct.LCD_Y - 1;
    sy =  r / (lcd_touch_panel_struct.AD_Bottom - lcd_touch_panel_struct.AD_Top);  
    if (sy <= 0 || sy > lcd_touch_panel_struct.LCD_Y)
        return 0;
    return sy;  
}

/*!
    \brief      get a value (X or Y) for several times. Order these values, 
                remove the lowest and highest and obtain the average value
    \param[in]  channel_select: select channel X or Y
      \arg        CH_X: channel X
      \arg        CH_Y: channel Y
    \param[out] none
    \retval     a value(X or Y) of touch point
*/
uint16_t bsp_touch_data_filter(uint8_t channel_select)
{
    uint16_t i=0, j=0; 
    uint16_t buf[FILTER_READ_TIMES]; 
    uint16_t sum=0; 
    uint16_t temp=0;
    /* Read data in FILTER_READ_TIMES times */
    for(i=0; i < FILTER_READ_TIMES; i++){
        buf[i] = bsp_touch_ad_get(channel_select);
    }
    /* Sort in ascending sequence */
    for(i = 0; i < FILTER_READ_TIMES - 1; i++){
        for(j = i + 1; j < FILTER_READ_TIMES; j++){
            if(buf[i] > buf[j]){
                temp = buf[i]; 
                buf[i] = buf[j]; 
                buf[j] = temp;
            }
        }
    }
    sum = 0;
    for(i = FILTER_LOST_VAL; i < FILTER_READ_TIMES - FILTER_LOST_VAL; i++){
        sum += buf[i];
    }
    temp = sum / (FILTER_READ_TIMES - 2 * FILTER_LOST_VAL);
    
    return temp;
}

/*!
    \brief      get the AD sample value of touch location. 
                get the sample value for several times,order these values,remove the lowest and highest and obtain the average value
    \param[in]  channel_select: select channel X or Y
    \param[out] none
      \arg        ad_x: channel X AD sample value
      \arg        ad_y: channel Y AD sample value
    \retval     ErrStatus: SUCCESS or ERROR

*/
Drv_Err bsp_touch_ad_xy_get(uint16_t *ad_x, uint16_t *ad_y)
{
    uint16_t ad_x1=0, ad_y1=0, ad_x2=0, ad_y2=0; 

    ad_x1 = bsp_touch_data_filter(CH_X); 
    ad_y1 = bsp_touch_data_filter(CH_Y); 
    ad_x2 = bsp_touch_data_filter(CH_X); 
    ad_y2 = bsp_touch_data_filter(CH_Y);
    
    if((abs(ad_x1 - ad_x2) > AD_ERR_RANGE) || (abs(ad_y1 - ad_y2) > AD_ERR_RANGE)){
        return DRV_ERROR;
    }
    *ad_x = (ad_x1 + ad_x2) / 2; 
    *ad_y = (ad_y1 + ad_y2) / 2;
   
    return DRV_SUCCESS;
}

/*!
    \brief      detect the touch event
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR

*/
Drv_Err bsp_touch_scan(uint16_t *x, uint16_t *y)
{
    static uint8_t count=0;
    uint16_t touch_ad_x,touch_ad_y = 0;
    /* touch pen is active */
    if (SET == bsp_touch_pen_irq())
    {   
        if(++count>=20 || lcd_touch_panel_struct.TOUCH_State==1)
        {
            lcd_touch_panel_struct.TOUCH_State=1;
            if((SUCCESS != bsp_touch_ad_xy_get(&touch_ad_x, &touch_ad_y))){
                return DRV_ERROR; 
            }        
//            if(invalid_count >= 20){ 
//                return DRV_ERROR;
//            }           
        }else
        {
            return DRV_ERROR;
        }        
    }else{ 
        count=0;
        if(lcd_touch_panel_struct.TOUCH_State==1){
            lcd_touch_panel_struct.TOUCH_State=2;
            lcd_touch_panel_struct.Touch_x_Last=*x;
            lcd_touch_panel_struct.Touch_y_Last=*y;             
        }
        return DRV_ERROR;
    }
    

    *x=bsp_touch_coordinate_x_get(touch_ad_x); 
    *y=bsp_touch_coordinate_y_get(touch_ad_y);  
    if(count==20)
    {
        lcd_touch_panel_struct.TOUCH_State=1;       
        lcd_touch_panel_struct.Touch_x_First=*x;
        lcd_touch_panel_struct.Touch_y_First=*y;                 
    }
    lcd_touch_panel_struct.Touch_x_Now=*x;
    lcd_touch_panel_struct.Touch_y_Now=*y;       
    
    return DRV_SUCCESS;
}
