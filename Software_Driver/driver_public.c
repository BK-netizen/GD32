/*!
 * �ļ����ƣ� driver_public.c
 * ��    ���� public �����ļ�
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

#include "driver_public.h"
 

typdef_tick_handle driver_tick_handle[]=
{
    {
        .tick_value=1,
        .tick_task_callback=NULL
    },
    {
        .tick_value=10,
        .tick_task_callback=NULL
    },
    {
        .tick_value=100,
        .tick_task_callback=NULL,
    }
};

#define TICK_HANDLE_SIZE  sizeof(driver_tick_handle)/sizeof(typdef_tick_handle)
    
volatile uint64_t driver_tick=0;
static uint32_t delay_us_mul=0; 
    
SET_VECTOR_TABLE static const uint8_t vector_table[0x400];
extern const uint32_t __Vectors[];
static void default_mpu_config(void);
   

/*!
    \brief      enable the CPU cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
SET_FLASH_AREA void cache_enable(void)
{
    /* enable I-Cache and D-Cache */
		
    SCB_EnableICache();
    SCB_EnableDCache();
}

SET_FLASH_AREA void cache_disable(void)
{
    /* enable I-Cache and D-Cache */
		
    SCB_DisableICache();
    SCB_DisableDCache();
}

/*!
* ˵��     systic��ʼ����Ĭ��1ms��
* ����[1]  ��
* ����ֵ   ��
*/
SET_FLASH_AREA static void systick_config(void)
{          
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U)){
        /* capture error */
        while (1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0xFU);
}

/*!
* ˵��     delay��ʼ��
* ����[1]  ��
* ����ֵ   ��
*/
SET_FLASH_AREA Drv_Err delay_init(void)
{
    uint32_t temp;
    
    SystemCoreClockUpdate();
    
    temp=SystemCoreClock/1000;
    
    if ((temp - 1UL) > SysTick_LOAD_RELOAD_Msk)
    {
    return (DRV_ERROR);                                                   /* Reload value impossible */
    }

    SysTick->LOAD  = (uint32_t)(temp - 1UL);                         /* set reload register */

    SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
    
    delay_us_mul=SystemCoreClock/1000000;   
    
    return (DRV_SUCCESS);                                                     /* Function successful */
}

/*!
* ˵��     driver��ع���������ʼ��
* ����[1]  ��
* ����ֵ   ��
*/
SET_FLASH_AREA void driver_init(void)
{         
    __IO uint32_t p_vector_table=(uint32_t)&vector_table;
    __disable_irq(); 
    
    driver_system_config();    
        
    for(__IO uint16_t t=0;t<0x400;t++)
    {
        ((uint8_t*)p_vector_table)[t]=REG8((uint32_t)(&__Vectors)+t);
    }
    
    default_mpu_config();    
            
    nvic_vector_table_set((uint32_t)(&vector_table[0]), 0);   
        
    cache_enable();
        
    __enable_irq();

    systick_config();
    
    delay_init();
    
    #if (LCD_DEBUG == 1) 
        #ifdef TFT_RGB_LCD
            bsp_rgb_lcd_init();                 /* ��ʼ��LCD */
            bsp_rgb_lcd_clear(WHITE);
            //��ʾlogͼƬ
            bsp_rgb_lcd_show_log();    
            //���ô�ӡ����
            bsp_rgb_lcd_printf_init(10,109,bsp_rgb_lcd_parameter.width-1,bsp_rgb_lcd_parameter.height-1,FONT_ASCII_24_12,WHITE,BLUE); 
        #else
            bsp_8080_lcd_init();                 /* ��ʼ��LCD */
            bsp_8080_lcd_clear(WHITE);
            //��ʾlogͼƬ
            bsp_8080_lcd_show_log();    
            //���ô�ӡ����
            bsp_8080_lcd_printf_init(100,109,bsp_8080_lcd_parameter.width-1,bsp_8080_lcd_parameter.height-1,FONT_ASCII_16_8,WHITE,BLUE); 
        #endif        
    #endif    
}

/*!
* ˵��     1us��ʱ
* ����[1]  count��us��λ
* ����ֵ   ��
*/
SET_RAM_CODE void delay_us(uint32_t count)
{
    uint32_t temp;
    
    if(delay_us_mul==0)
    {
        delay_init();    
    }
    SysTick->CTRL  |= SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
    count=count*delay_us_mul;

    if(count>SysTick->LOAD ){
        count=SysTick->LOAD ;
    }
    temp=SysTick->VAL;
    if(temp>count)
    {
        while(SysTick->VAL>(temp-count) && SysTick->VAL<=temp);
    }
    else {
        while(SysTick->VAL<=temp);
        while(SysTick->VAL>(SysTick->LOAD-(count-temp)));        
    }
}

/*!
* ˵��     ����������ʱ
* ����[1]  count���������ڸ���
* ����ֵ   ��
*/
SET_RAM_CODE void delay_sysclk(uint32_t count)
{
    uint32_t temp;
    
    if(delay_us_mul==0)
    {
        delay_init();    
    }
    
    SysTick->CTRL  |= SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */   
    if(count>SysTick->LOAD ){
        count=SysTick->LOAD ;
    }
    temp=SysTick->VAL;
    if(temp>count)
    {
        while(SysTick->VAL>(temp-count) && SysTick->VAL<=temp);
    }
    else {
        while(SysTick->VAL<=temp);
        while(SysTick->VAL>(SysTick->LOAD-(count-temp)));        
    }
}

/*!
* ˵��     1ms��ʱ
* ����[1]  count��ma��λ
* ����ֵ   ��
*/
SET_RAM_CODE void delay_ms(uint32_t count)
{  
    count=count*10;
    do{
        delay_us(100);
    }while(count--);
}

/*!
* ˵��     1ms tick������������systic�жϵ���
* ����[1]  ��
* ����ֵ   ��
*/
SET_RAM_CODE void driver_tic_inc(void)
{
    driver_tick++;
    for(uint8_t i=0;i<TICK_HANDLE_SIZE;i++)
    {
        if(driver_tick % driver_tick_handle[i].tick_value ==0 &&  driver_tick_handle[i].tick_task_callback!=NULL)
        {
            driver_tick_handle[i].tick_task_callback();    
        }
    }
}

/*!
    \brief      memory compare function
    \param[in]  src: source data pointer
    \param[in]  dst: destination data pointer
    \param[in]  length: the compare data length
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
Drv_Err memory_compare(uint8_t* src, uint8_t* dst, uint16_t length) 
{
    while(length --){
        if(*src++ != *dst++)
            return DRV_ERROR;
    }
    return DRV_SUCCESS;
}

//ʹ�ô˺������ò���Ҫ�ٹ���MPU_REGION_NUMBERx�����Զ�����
/*!
    \param[in]  mpu_init_struct: MPU initialization structure
                  region_base_address: region base address
                  region_size: MPU_REGION_SIZE_32B, MPU_REGION_SIZE_64B, MPU_REGION_SIZE_128B, MPU_REGION_SIZE_256B, MPU_REGION_SIZE_512B, 
                               MPU_REGION_SIZE_1KB, MPU_REGION_SIZE_2KB, MPU_REGION_SIZE_4KB, MPU_REGION_SIZE_8KB, MPU_REGION_SIZE_16KB, 
                               MPU_REGION_SIZE_32KB, MPU_REGION_SIZE_64KB, MPU_REGION_SIZE_128KB, MPU_REGION_SIZE_256KB, MPU_REGION_SIZE_512KB,
                               MPU_REGION_SIZE_1MB, MPU_REGION_SIZE_2MB, MPU_REGION_SIZE_4MB, MPU_REGION_SIZE_8MB, MPU_REGION_SIZE_16MB, 
                               MPU_REGION_SIZE_32MB, MPU_REGION_SIZE_64MB, MPU_REGION_SIZE_128MB, MPU_REGION_SIZE_256MB, MPU_REGION_SIZE_512MB, 
                               MPU_REGION_SIZE_1GB, MPU_REGION_SIZE_2GB, MPU_REGION_SIZE_4GB
                  access_permission: MPU_AP_NO_ACCESS, MPU_AP_PRIV_RW, MPU_AP_PRIV_RW_UNPRIV_RO, MPU_AP_FULL_ACCESS, MPU_AP_PRIV_RO,
                                     MPU_AP_PRIV_UNPRIV_RO
                  access_bufferable: MPU_ACCESS_BUFFERABLE, MPU_ACCESS_NON_BUFFERABLE
                  access_cacheable: MPU_ACCESS_CACHEABLE, MPU_ACCESS_NON_CACHEABLE
                  access_shareable: MPU_ACCESS_SHAREABLE, MPU_ACCESS_NON_SHAREABLE
                  instruction_exec: MPU_INSTRUCTION_EXEC_PERMIT, MPU_INSTRUCTION_EXEC_NOT_PERMIT
                  tex_type: MPU_TEX_TYPE0, MPU_TEX_TYPE1, MPU_TEX_TYPE2
    \param[out] none
    \retval     none
*/
SET_FLASH_AREA void mpu_config(uint32_t region_base_address,uint32_t region_size,uint32_t access_permission,\
                                    uint32_t access_bufferable,uint32_t access_cacheable,uint32_t access_shareable,\
                                    uint32_t instruction_exec,uint32_t tex_type)
{
    mpu_region_init_struct mpu_init_struct;
    mpu_region_struct_para_init(&mpu_init_struct);
    
    static uint8_t MPU_REGION_NUMBERx=7;

//    /* disable the MPU */
//    ARM_MPU_SetRegion(0U, 0U);
    
    /* configure the MPU attributes for all */
    mpu_init_struct.region_base_address = region_base_address;
    mpu_init_struct.region_size         = region_size;
    mpu_init_struct.access_permission   = access_permission;
    mpu_init_struct.access_bufferable   = access_bufferable;
    mpu_init_struct.access_cacheable    = access_cacheable;
    mpu_init_struct.access_shareable    = access_shareable;
    mpu_init_struct.region_number       = MPU_REGION_NUMBERx;
    mpu_init_struct.subregion_disable   = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec    = instruction_exec;
    mpu_init_struct.tex_type            = tex_type;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();  
    
    /* enable the MPU */
    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);  

    MPU_REGION_NUMBERx++;
}


//Ĭ��MPU����
SET_FLASH_AREA static void default_mpu_config(void)
{
    mpu_region_init_struct mpu_init_struct;
    mpu_region_struct_para_init(&mpu_init_struct);

    /* disable the MPU */
    ARM_MPU_SetRegion(0U, 0U);
    
    /*Ĭ�������ⲿ�洢����*/
    /* configure the MPU attributes for all */
    mpu_init_struct.region_base_address = 0;
    mpu_init_struct.region_size         = MPU_REGION_SIZE_4GB;
    mpu_init_struct.access_permission   = MPU_AP_NO_ACCESS;
    mpu_init_struct.access_bufferable   = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.access_cacheable    = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable    = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number       = MPU_REGION_NUMBER0;
    mpu_init_struct.subregion_disable   = 0x87;
    mpu_init_struct.instruction_exec    = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();  
    
    /*SRAM0 1�ر�cacheԤ������̫������*/    
    /* configure the MPU attributes for DMA_RAM */
    mpu_init_struct.region_base_address = 0x30000000;
    mpu_init_struct.region_size         = MPU_REGION_SIZE_32KB;
    mpu_init_struct.access_permission   = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable   = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.access_cacheable    = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable    = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number       = MPU_REGION_NUMBER1;
    mpu_init_struct.subregion_disable   = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec    = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();   

    /*AXI SRAMԤ��64K�ر�cache��Ӳ��dma���ʣ���ɢ���غ�ʹ��SET_DMA_AREA_RAM*/  
    /* configure the MPU attributes for DMA AXI_SRAM */
    mpu_init_struct.region_base_address = 0x24070000;
    mpu_init_struct.region_size         = MPU_REGION_SIZE_64KB;
    mpu_init_struct.access_permission   = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable   = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.access_cacheable    = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable    = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number       = MPU_REGION_NUMBER2;
    mpu_init_struct.subregion_disable   = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec    = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /*SDRAMĬ�����ÿ�cache����֧�ַǶ�����ʣ���ɢ���غ�ʹ��SET_SDRAM*/  
    /* configure the MPU attributes for SDRAM */
    mpu_init_struct.region_base_address = 0xC0000000;
    mpu_init_struct.region_size         = MPU_REGION_SIZE_32MB;
    mpu_init_struct.access_permission   = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable   = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.access_cacheable    = MPU_ACCESS_CACHEABLE;
    mpu_init_struct.access_shareable    = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number       = MPU_REGION_NUMBER3;
    mpu_init_struct.subregion_disable   = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec    = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();  
    
    /*SDRAM����2MB�ر�cache��Ӳ��DMA���ʣ�����DCI��TLI����ɢ���غ�ʹ��SET_DMA_AREA_SDRAM*/  
    mpu_init_struct.region_base_address = 0xC0600000;
    mpu_init_struct.region_size         = MPU_REGION_SIZE_2MB;
    mpu_init_struct.access_permission   = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable   = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.access_cacheable    = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable    = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number       = MPU_REGION_NUMBER4;
    mpu_init_struct.subregion_disable   = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec    = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();  
    
    /*ִ�д˴�ʱITC�������Ѿ�������ϣ�����������ֻ����ֹ�ڴ��ƻ�����ɢ���غ�ʹ��SET_RAM_CODE*/      
    /* configure the MPU attributes for ITC */
    mpu_init_struct.region_base_address = (uint32_t)(0);
    mpu_init_struct.region_size         = MPU_REGION_SIZE_512KB;
    mpu_init_struct.access_permission   = MPU_AP_PRIV_UNPRIV_RO;
    mpu_init_struct.access_bufferable   = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.access_cacheable    = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable    = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number       = MPU_REGION_NUMBER5;
    mpu_init_struct.subregion_disable   = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec    = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();    

    /*ITC�����ٽ�ջ�����������óɿɶ�д������һ������*/ 
    /* configure the MPU attributes for ITC */
    mpu_init_struct.region_base_address = (0);
    mpu_init_struct.region_size         = MPU_REGION_SIZE_16KB;
    mpu_init_struct.access_permission   = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable   = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.access_cacheable    = MPU_ACCESS_CACHEABLE;
    mpu_init_struct.access_shareable    = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number       = MPU_REGION_NUMBER6;
    mpu_init_struct.subregion_disable   = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec    = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();  

    /* enable the MPU */
    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);       
}

//#define CHIP_DEF  //оƬ����״̬
//�ڽ���main֮ǰ��SystemInit�е���
SET_FLASH_AREA static void ob_ram_config(void)
{
    uint32_t itcram=0,dtcram=0,itcram_ecc,dtcram0_ecc,dtcram1_ecc;
    
    ob_tcm_ecc_get(&itcram_ecc,&dtcram0_ecc,&dtcram1_ecc);
    ob_tcm_shared_ram_size_get(&itcram,&dtcram);
    
#ifndef CHIP_DEF     
    if( itcram!=512 || dtcram!=0 ||\
                itcram_ecc!=OB_ITCMECCEN_ENABLE || dtcram0_ecc!=OB_DTCM0ECCEN_ENABLE || dtcram1_ecc!=OB_DTCM1ECCEN_ENABLE)
    {
        ob_unlock();
        
        ob_tcm_shared_ram_config(OB_ITCM_SHARED_RAM_512KB, OB_DTCM_SHARED_RAM_0KB);
        ob_tcm_ecc_config(OB_ITCMECCEN_ENABLE, OB_DTCM0ECCEN_ENABLE, OB_DTCM1ECCEN_ENABLE);
        
        ob_start();
        
        ob_lock(); 

        __DSB();                                                          /* Ensure all outstanding memory accesses included
                                                                           buffered write are completed before reset */
        SCB->AIRCR  = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos)    |
                               (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                                SCB_AIRCR_SYSRESETREQ_Msk    );         /* Keep priority group unchanged */
        __DSB();                                                          /* Ensure completion of memory access */

        for(;;)                                                           /* wait until reset */
        {
        __NOP();
        }
    } 
#else
    if( itcram!=64 || dtcram!=64 ||\
                itcram_ecc!=OB_ITCMECCEN_ENABLE || dtcram0_ecc!=OB_DTCM0ECCEN_ENABLE || dtcram1_ecc!=OB_DTCM1ECCEN_ENABLE)
    {
        ob_unlock();
        
        ob_tcm_shared_ram_config(OB_ITCM_SHARED_RAM_64KB, OB_DTCM_SHARED_RAM_64KB);
        ob_tcm_ecc_config(OB_ITCMECCEN_ENABLE, OB_DTCM0ECCEN_ENABLE, OB_DTCM1ECCEN_ENABLE);
        
        ob_start();
        
        ob_lock(); 

        __DSB();                                                          /* Ensure all outstanding memory accesses included
                                                                           buffered write are completed before reset */
        SCB->AIRCR  = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos)    |
                               (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                                SCB_AIRCR_SYSRESETREQ_Msk    );         /* Keep priority group unchanged */
        __DSB();                                                          /* Ensure completion of memory access */

        for(;;)                                                           /* wait until reset */
        {
        __NOP();
        }
    }
#endif    
}

SET_FLASH_AREA void driver_system_config(void)    
{
    ob_ram_config();    
}

