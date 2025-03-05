/*!
 * 文件名称：  bsp_ov7670.c
 * 描    述：  bsp ov7670 interface
 * 版本：      2023-12-03, V1.0
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

#include "bsp_ov7670.h"
#include "bsp_ov7670_init_table.h"

#include "driver_timer.h"

#include "driver_gpio.h"

#include "bsp_software_i2c.h"


//GPIO_DEF(DCI_RESET,G,15,IN_NONE,RESET,NULL); 
GPIO_DEF(DCI_PWDN, G,13,OUT_PP,SET,  NULL); 

DMA_DEF(DCI_DMA,DMA1,DMA_CH7,Circulation_Disable,DMA_REQUEST_DCI);


#define DCI_WIDTH   320
#define DCI_HEIGHT  240

#define DMA_NUM  (DCI_WIDTH*DCI_HEIGHT/2)

SET_DMA_AREA_SDRAM uint16_t camera_buff[DCI_HEIGHT][DCI_WIDTH];

TIMER_CH_DEF(XLK_PWM,TIMER1,2,TIMER_CH_PWM_HIGH,A,2,AF_PP,GPIO_AF_1);


/*!
    \brief      configure the DCI to interface with the camera module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_config(void)
{
    dci_parameter_struct dci_struct;
    
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOE);

    rcu_periph_clock_enable(RCU_DCI);

    /* DCI GPIO AF configuration */
    /* configure DCI_PIXCLK(PE3), DCI_VSYNC(PB7), DCI_HSYNC(PA4) */
    gpio_af_set(GPIOE, GPIO_AF_13, GPIO_PIN_5|GPIO_PIN_6);
    gpio_af_set(GPIOB, GPIO_AF_13, GPIO_PIN_6|GPIO_PIN_7);
    gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_4|GPIO_PIN_6);
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11);    

    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5|GPIO_PIN_6);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_5|GPIO_PIN_6);
    
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6|GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_6|GPIO_PIN_7);

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4|GPIO_PIN_6);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_4|GPIO_PIN_6);

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11);

  
    dci_deinit();
    /* DCI configuration */
    dci_struct.capture_mode = DCI_CAPTURE_MODE_SNAPSHOT;
    dci_struct.clock_polarity =  DCI_CK_POLARITY_RISING;
    dci_struct.hsync_polarity = DCI_HSYNC_POLARITY_LOW;
    dci_struct.vsync_polarity = DCI_VSYNC_POLARITY_HIGH;
    dci_struct.frame_rate = DCI_FRAME_RATE_ALL;
    dci_struct.interface_format = DCI_INTERFACE_FORMAT_8BITS;
    dci_init(&dci_struct);
}

uint8_t dci_check_ready(void)
{
    uint8_t camera_state=0;
    if(dma_flag_get(DCI_DMA.dmax,DCI_DMA.dma_chx,DMA_FLAG_FTF)!=RESET)
    {
        dma_flag_clear(DCI_DMA.dmax,DCI_DMA.dma_chx,DMA_FLAG_FTF);
        camera_state=3;    
    }
    else if(dma_transfer_number_get(DCI_DMA.dmax,DCI_DMA.dma_chx)<DMA_NUM*1/10)
    {
        camera_state=2;    
    }     
    else if(dma_transfer_number_get(DCI_DMA.dmax,DCI_DMA.dma_chx)<DMA_NUM*5/10)
    {
        camera_state=1;    
    }
    return camera_state;    
}

void dci_restart(void)
{
    driver_dma_start(&DCI_DMA,(uint8_t *)camera_buff,DMA_NUM);    
    dci_capture_enable();
}


/*!
    \brief      mco1 initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_mco_init(uint8_t period)
{
    driver_timer_channel_init(&XLK_PWM,160000000,period);
    driver_timer_pwm_on(&XLK_PWM);    
    driver_timer_pwm_duty_set(&XLK_PWM,50);
    
}

/*!
    \brief      configure I2C gpio and I2C parameter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_sccb_config(void)
{
    bsp_software_i2c_init(&CAMERA_IIC);   
}

/*!
    \brief      write a byte at a specific camera register
    \param[in]  reg:  camera register address
    \param[in]  data: data to be written to the specific register
    \param[out] none
    \retval     0x00 if write operation is OK. 
                0xFF if timeout condition occured (device not connected or bus error).
*/
uint8_t bsp_dci_byte_write(uint8_t reg, uint8_t data)
{
    return bsp_software_i2c_mem_write_byte(&CAMERA_IIC,OV7670_DEVICE_WRITE_ADDRESS>>1,reg,data);
}

/*!
    \brief      read a byte from a specific camera register 
    \param[in]  reg:  camera register address
    \param[out] data: read from the specific register
    \retval     0x00 if write operation is ok. 
                0xFF if timeout condition occured (device not connected or bus error).
*/
uint8_t bsp_dci_byte_read(uint8_t reg, uint8_t *data)
{
    bsp_software_i2c_write_byte(&CAMERA_IIC,OV7670_DEVICE_WRITE_ADDRESS>>1,reg);
    return bsp_software_i2c_read_data(&CAMERA_IIC,OV7670_DEVICE_READ_ADDRESS>>1,data);      
}

/*!
    \brief      DCI camera initialization
    \param[in]  none
    \param[out] none
    \retval     0x00 or 0xff
*/
uint8_t bsp_ov7670_init(void)
{
    uint8_t i;
    
//    driver_gpio_general_init(&DCI_RESET);
    driver_gpio_general_init(&DCI_PWDN);

    driver_gpio_pin_reset(&DCI_PWDN);
    delay_ms(10);
//    driver_gpio_pin_set(&DCI_RESET);
//    delay_ms(10);
    
    bsp_sccb_config();

    dci_config();    
    
    bsp_mco_init(12);//配置OV PLL前先输出XLK高频
    
    delay_ms(10); 
    if(bsp_dci_byte_write(OV7670_COM7, SCCB_REG_RESET)!=0)
        return 0xff;
    delay_ms(10);
    
    for(i=0;i<CHANGE_REG_NUM;i++)
    {
        if(bsp_dci_byte_write(change_reg[i][0],change_reg[i][1])!=0)
        {
            return 0xff;
        }
    }
    
//    bsp_mco_init(15);//配置OV PLL后可降低XLK频率 

    delay_ms(10);
    
    driver_dma_com_init(&DCI_DMA,((uint32_t)&DCI_DATA),(uint32_t)camera_buff,DMA_Width_32BIT,DMA_PERIPH_TO_MEMORY);
    
    driver_dma_start(&DCI_DMA,(uint8_t *)camera_buff,DMA_NUM);
           
    dci_enable();
    dci_capture_enable();
    
    return 0;
}

/*!
    \brief      read the ov7670 manufacturer identifier
    \param[in]  ov7670id: pointer to the ov7670 manufacturer struct
    \param[out] none
    \retval     0x00 or 0xff
*/
uint8_t bsp_ov7670_id_read(ov7670_id_struct* ov7670id)
{
    uint8_t temp;
    if(bsp_dci_byte_read(OV7670_MIDH,&temp)!=0)
        return 0xff;
    ov7670id->Manufacturer_ID1 = temp;
    if(bsp_dci_byte_read(OV7670_MIDL,&temp)!=0)
        return 0xff;
    ov7670id->Manufacturer_ID2 = temp;
    if(bsp_dci_byte_read(OV7670_VER,&temp)!=0)
        return 0xff;
    ov7670id->Version = temp;
    if(bsp_dci_byte_read(OV7670_PID,&temp)!=0)
        return 0xff;
    ov7670id->PID = temp;

    return 0;
}


