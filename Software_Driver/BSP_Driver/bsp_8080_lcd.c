/*!
 * �ļ����ƣ�  bsp_8080_lcd.c
 * ��    ����  bsp lcd interface
 * �汾��      2023-12-03, V1.0
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

#include "bsp_8080_lcd.h"
#include "driver_public.h"
#include "driver_gpio.h"
#include "driver_i2c.h"
#include "driver_exmc.h"
#include "driver_timer.h"
#include "driver_dma.h"



#include "./bsp_8080_lcd_init.c"



#include <stdarg.h>

//�û����ö�����
#define BACK_LIGHT_DUTY 80

TIMER_CH_DEF(LCD_8080_BL,TIMER3,1,TIMER_CH_PWM_HIGH,D,13,AF_PP,GPIO_AF_2);

#define EXMC_Ax  12
AFIO_DEF(EXMC_Ax_GPIO,G,2,AF_PP,GPIO_AF_12);

#define EXMC_NEx 1
AFIO_DEF(EXMC_NEx_GPIO,G,9,AF_PP,GPIO_AF_12);

GPIO_DEF(LCD_8080_RST,D,12,OUT_PP,RESET,NULL);



//�����ڲ�������
#define EXMC_LCD_D  REG16(((uint32_t)(EXMC_BANK0_NORSRAM_REGIONx_ADDR(EXMC_NEx)))|BIT(EXMC_Ax)*2)
#define EXMC_LCD_C  REG16(((uint32_t)(EXMC_BANK0_NORSRAM_REGIONx_ADDR(EXMC_NEx))))


typde_8080_lcd_printf_parameter_struct bsp_8080_lcd_pritnf_parameter;


/* ����LCD��Ҫ���� */
typde_8080_lcd_parameter_struct bsp_8080_lcd_parameter;


static __IO uint8_t lcd_backlight_duty=0;

static uint16_t bsp_8080_lcd_read_data(void);



static void bsp_8080_lcd_port_init(void)
{    
    driver_timer_channel_init(&LCD_8080_BL,10000000,100);//1M/1000 pwm ck=1K
       
    driver_timer_pwm_on(&LCD_8080_BL);
    
    driver_dma_mem_init(&DMA_MEM);
    
    driver_gpio_general_init(&EXMC_Ax_GPIO);
    driver_gpio_general_init(&EXMC_NEx_GPIO);    
    
    driver_exmc_lcd_init(EXMC_LCD(EXMC_NEx));
}

void bsp_8080_lcd_backlight_on(void)
{    
    driver_timer_pwm_on(&LCD_8080_BL);
    driver_timer_pwm_duty_set(&LCD_8080_BL,lcd_backlight_duty);    
}

void bsp_8080_lcd_backlight_off(void)
{   
    driver_timer_pwm_off(&LCD_8080_BL);    
    driver_timer_pwm_duty_set(&LCD_8080_BL,0);
}

void bsp_8080_lcd_backlight_duty_set(uint8_t backlight_value)
{    
    lcd_backlight_duty=backlight_value;
    driver_timer_pwm_duty_set(&LCD_8080_BL,lcd_backlight_duty);
}


/**
 * ˵��       ��ʼ��LCD
 *   @note      �ó�ʼ���������Գ�ʼ�������ͺŵ�LCD(�����.c�ļ���ǰ�������)
 *
 * ����       ��
 * ����ֵ      ��
 */
uint32_t bsp_8080_lcd_init(void)
{
    
    bsp_8080_lcd_port_init();

    driver_gpio_general_init(&LCD_8080_RST);
    
    driver_gpio_pin_reset(&LCD_8080_RST);
    delay_ms(10);
    driver_gpio_pin_set(&LCD_8080_RST);
    
    delay_ms(10);    
    bsp_8080_lcd_backlight_duty_set(BACK_LIGHT_DUTY);
    
    bsp_8080_lcd_backlight_on();          /* �������� */    
    
    /* LCD�Ļ�����ɫ�ͱ���ɫ */
    bsp_8080_lcd_parameter.g_point_color = WHITE;    /* ������ɫ */
    bsp_8080_lcd_parameter.g_back_color  = BLACK;    /* ����ɫ */    
    
    delay_ms(1);     /* ��ʼ��FSMC��,����ȴ�һ��ʱ����ܿ�ʼ��ʼ�� */

    /* ����9341 ID�Ķ�ȡ */
    bsp_8080_lcd_wr_regno(0XD3);
    bsp_8080_lcd_parameter.id = bsp_8080_lcd_read_data();  /* dummy read */
    bsp_8080_lcd_parameter.id = bsp_8080_lcd_read_data();  /* ����0X00 */
    bsp_8080_lcd_parameter.id = bsp_8080_lcd_read_data();  /* ��ȡ0X93 */
    bsp_8080_lcd_parameter.id <<= 8;
    bsp_8080_lcd_parameter.id |= bsp_8080_lcd_read_data(); /* ��ȡ0X41 */


    if (bsp_8080_lcd_parameter.id == 0X9488)
    {
        lcd_ex_ili9488_reginit();   /* ִ��ILI9388��ʼ�� */
    }     
    else if (bsp_8080_lcd_parameter.id == 0X9341)
    {
        lcd_ex_ili9341_reginit();   /* ִ��ILI9341��ʼ�� */
    }else{
        bsp_8080_lcd_parameter.id = 0X9488;
        bsp_8080_lcd_display_dir(0); /* Ĭ��Ϊ���� */
        bsp_8080_lcd_scan_dir(DFT_SCAN_DIR);     /* Ĭ��ɨ�跽�� */        
        return DRV_ERROR;
    }

    bsp_8080_lcd_display_dir(0); /* Ĭ��Ϊ���� */
    bsp_8080_lcd_scan_dir(DFT_SCAN_DIR);     /* Ĭ��ɨ�跽�� */
    bsp_8080_lcd_clear(WHITE);
    
    return DRV_SUCCESS;
}



/**
 * ˵��       LCDд����
 * ����       data: Ҫд�������
 * ����ֵ      ��
 */
void bsp_8080_lcd_wr_data(__IO uint16_t data)
{
//    delay_sysclk(1);            
    EXMC_LCD_D = data;
}

/**
 * ˵��       LCDд�Ĵ������/��ַ����
 * ����       regno: �Ĵ������/��ַ
 * ����ֵ      ��
 */
void bsp_8080_lcd_wr_regno(__IO uint16_t regno)
{
//    delay_sysclk(1);        
    EXMC_LCD_C = regno;   /* д��Ҫд�ļĴ������ */
}

/**
 * ˵��       LCDд�Ĵ���
 * ����       regno:�Ĵ������/��ַ
 * ����       data:Ҫд�������
 * ����ֵ      ��
 */
void bsp_8080_lcd_write_reg(__IO uint16_t regno,__IO uint16_t data)
{
//    delay_sysclk(1);     
    EXMC_LCD_C = regno;   /* д��Ҫд�ļĴ������ */
//    delay_sysclk(1);     
    EXMC_LCD_D = data;    /* д������ */
}

/**
 * ˵��       LCD������
 * ����       ��
 * ����ֵ      ��ȡ��������
 */
static uint16_t bsp_8080_lcd_read_data(void)
{
//    delay_sysclk(1);
    return EXMC_LCD_D;
}

/**
 * ˵��       ��ȡ��ĳ�����ɫֵ
 * ����       x,y:����
 * ����ֵ      �˵����ɫ(32λ��ɫ,�������LTDC)
 */
uint32_t bsp_8080_lcd_read_point(uint16_t x, uint16_t y)
{
    uint16_t r = 0, g = 0, b = 0;

    if (x >= bsp_8080_lcd_parameter.width || y >= bsp_8080_lcd_parameter.height)return 0;   /* �����˷�Χ,ֱ�ӷ��� */

    bsp_8080_lcd_set_cursor(x, y);       /* �������� */

    bsp_8080_lcd_wr_regno(0X2E);     /* 9341/5310/1963/7789 �ȷ��Ͷ�GRAMָ�� */

    r = bsp_8080_lcd_read_data();          /* �ٶ�(dummy read) */

    if (bsp_8080_lcd_parameter.id == 0X1963)return r;   /* 1963 ֱ�Ӷ��Ϳ��� */

    r = bsp_8080_lcd_read_data();          /* ʵ��������ɫ */

    /* 9341/5310/5510/7789 Ҫ��2�ζ��� */
    b = bsp_8080_lcd_read_data();
    g = r & 0XFF;               /* ���� 9341/5310/5510/7789, ��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ */
    g <<= 8;
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));  /* 9341/5310/5510/7789 ��Ҫ��ʽת��һ�� */
}

/**
 * ˵��       LCD������ʾ
 * ����       ��
 * ����ֵ      ��
 */
void bsp_8080_lcd_display_on(void)
{
    bsp_8080_lcd_wr_regno(0X29);     /* ������ʾ */
}

/**
 * ˵��       LCD�ر���ʾ
 * ����       ��
 * ����ֵ      ��
 */
void bsp_8080_lcd_display_off(void)
{
    bsp_8080_lcd_wr_regno(0X28);     /* �ر���ʾ */
}

/**
 * ˵��       ���ù��λ��(��RGB����Ч)
 * ����       x,y: ����
 * ����ֵ      ��
 */
void bsp_8080_lcd_set_cursor(uint16_t x, uint16_t y)
{
    bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setxcmd);
    bsp_8080_lcd_wr_data(x >> 8);
    bsp_8080_lcd_wr_data(x & 0XFF);
    bsp_8080_lcd_wr_data(bsp_8080_lcd_parameter.width >> 8);
    bsp_8080_lcd_wr_data(bsp_8080_lcd_parameter.width & 0XFF);        
    bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setycmd);
    bsp_8080_lcd_wr_data(y >> 8);
    bsp_8080_lcd_wr_data(y & 0XFF);
    bsp_8080_lcd_wr_data(bsp_8080_lcd_parameter.height >> 8);
    bsp_8080_lcd_wr_data(bsp_8080_lcd_parameter.height & 0XFF);          

}

/**
 * ˵��       ����LCD���Զ�ɨ�跽��(��RGB����Ч)
 *   @note
 *              9341/5310/5510/1963/7789��IC�Ѿ�ʵ�ʲ���
 *              ע��:�����������ܻ��ܵ��˺������õ�Ӱ��(������9341),
 *              ����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
 *
 * ����       dir:0~7,����8������(���嶨���lcd.h)
 * ����ֵ      ��
 */
void bsp_8080_lcd_scan_dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
 
    /* ����ɨ�跽ʽ ���� 0X36/0X3600 �Ĵ��� bit 5,6,7 λ��ֵ */
    switch (dir)
    {
        case L2R_U2D:/* ������,���ϵ��� */
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;

        case L2R_D2U:/* ������,���µ��� */
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;

        case R2L_U2D:/* ���ҵ���,���ϵ��� */
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;

        case R2L_D2U:/* ���ҵ���,���µ��� */
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;

        case U2D_L2R:/* ���ϵ���,������ */
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;

        case U2D_R2L:/* ���ϵ���,���ҵ��� */
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;

        case D2U_L2R:/* ���µ���,������ */
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;

        case D2U_R2L:/* ���µ���,���ҵ��� */
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

    dirreg = 0X36;  /* �Ծ��󲿷�����IC, ��0X36�Ĵ������� */


    /* 9341 & 7789 Ҫ����BGRλ */
    if (bsp_8080_lcd_parameter.id == 0X9341 || bsp_8080_lcd_parameter.id == 0X9488 || bsp_8080_lcd_parameter.id == 0X7789)
    {
        regval |= 0X08;
    }

    bsp_8080_lcd_write_reg(dirreg, regval);

    /* ������ʾ����(����)��С */
    if (bsp_8080_lcd_parameter.id == 0X5510)
    {
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setxcmd);
        bsp_8080_lcd_wr_data(0);
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setxcmd + 1);
        bsp_8080_lcd_wr_data(0);
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setxcmd + 2);
        bsp_8080_lcd_wr_data((bsp_8080_lcd_parameter.width - 1) >> 8);
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setxcmd + 3);
        bsp_8080_lcd_wr_data((bsp_8080_lcd_parameter.width - 1) & 0XFF);
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setycmd);
        bsp_8080_lcd_wr_data(0);
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setycmd + 1);
        bsp_8080_lcd_wr_data(0);
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setycmd + 2);
        bsp_8080_lcd_wr_data((bsp_8080_lcd_parameter.height - 1) >> 8);
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setycmd + 3);
        bsp_8080_lcd_wr_data((bsp_8080_lcd_parameter.height - 1) & 0XFF);
    }
    else
    {
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setxcmd);
        bsp_8080_lcd_wr_data(0);
        bsp_8080_lcd_wr_data(0);
        bsp_8080_lcd_wr_data((bsp_8080_lcd_parameter.width - 1) >> 8);
        bsp_8080_lcd_wr_data((bsp_8080_lcd_parameter.width - 1) & 0XFF);
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setycmd);
        bsp_8080_lcd_wr_data(0);
        bsp_8080_lcd_wr_data(0);
        bsp_8080_lcd_wr_data((bsp_8080_lcd_parameter.height - 1) >> 8);
        bsp_8080_lcd_wr_data((bsp_8080_lcd_parameter.height - 1) & 0XFF);
    }
}

/**
 * ˵��       �л���ʾ����ģʽ��ֱ���޸�LCDģʽ���� RAMˢ��ģʽ
 * ����       display_mode:LCD_MODE\SRAM_MODE
 * ����       display_ram:SRAM_MODEģʽʱ���Դ��ַ 
 * ����ֵ      ��
 */
void bsp_8080_lcd_mode_config(DISPLAY_MODE display_mode,uint16_t* display_ram)
{
    if(display_mode!=LCD_MODE){
        bsp_8080_lcd_parameter.display_ram=display_ram;    
    }
    bsp_8080_lcd_parameter.display_mode=display_mode;
}

/**
 * ˵��       ����
 * ����       x,y: ����
 * ����       color: �����ɫ(32λ��ɫ,�������LTDC)
 * ����ֵ      ��
 */
void bsp_8080_lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
    if(x>bsp_8080_lcd_parameter.width || y>bsp_8080_lcd_parameter.height)
    {
        return;
    }
    
    if(bsp_8080_lcd_parameter.display_mode == LCD_MODE)
    {
        bsp_8080_lcd_set_cursor(x, y);       /* ���ù��λ�� */
        EXMC_LCD_C = bsp_8080_lcd_parameter.wramcmd;    /* ��ʼд��GRAM */
        EXMC_LCD_D = color;
    }else
    {
        *(__IO uint16_t*)(((uint32_t)(bsp_8080_lcd_parameter.display_ram)) + 2*((bsp_8080_lcd_parameter.width*y) + x)) = color;        
    }
}

/**
 * ˵��       SSD1963�����������ú���
 * ����       pwm: ����ȼ�,0~100.Խ��Խ��.
 * ����ֵ      ��
 */
void bsp_8080_lcd_ssd_backlight_set(uint8_t pwm)
{
    bsp_8080_lcd_wr_regno(0xBE);         /* ����PWM��� */
    bsp_8080_lcd_wr_data(0x05);          /* 1����PWMƵ�� */
    bsp_8080_lcd_wr_data(pwm * 2.55);    /* 2����PWMռ�ձ� */
    bsp_8080_lcd_wr_data(0x01);          /* 3����C */
    bsp_8080_lcd_wr_data(0xFF);          /* 4����D */
    bsp_8080_lcd_wr_data(0x00);          /* 5����E */
    bsp_8080_lcd_wr_data(0x00);          /* 6����F */
}

/**
 * ˵��       ����LCD��ʾ����
 * ����       dir:0,����; 1,����
 * ����ֵ      ��
 */
void bsp_8080_lcd_display_dir(uint8_t dir)
{
    bsp_8080_lcd_parameter.dir = dir;   /* ����/���� */

    if (dir == 0)       /* ���� */
    {
        bsp_8080_lcd_parameter.width = 240;
        bsp_8080_lcd_parameter.height = 320;

        if (bsp_8080_lcd_parameter.id == 0X1963)
        {
            bsp_8080_lcd_parameter.wramcmd = 0X2C;  /* ����д��GRAM��ָ�� */
            bsp_8080_lcd_parameter.setxcmd = 0X2B;  /* ����дX����ָ�� */
            bsp_8080_lcd_parameter.setycmd = 0X2A;  /* ����дY����ָ�� */
            bsp_8080_lcd_parameter.width = 480;     /* ���ÿ��480 */
            bsp_8080_lcd_parameter.height = 800;    /* ���ø߶�800 */
        }
        else   /* ����IC, ����: 9341 / 5310 / 7789��IC */
        {
            bsp_8080_lcd_parameter.wramcmd = 0X2C;
            bsp_8080_lcd_parameter.setxcmd = 0X2A;
            bsp_8080_lcd_parameter.setycmd = 0X2B;
        }

        if ( bsp_8080_lcd_parameter.id == 0X9488)    /* �����5310 ���ʾ�� 320*480�ֱ��� */
        {
            bsp_8080_lcd_parameter.width = 320;
            bsp_8080_lcd_parameter.height = 480;
        }    
    }
    else                /* ���� */
    {
        bsp_8080_lcd_parameter.width = 320;         /* Ĭ�Ͽ�� */
        bsp_8080_lcd_parameter.height = 240;        /* Ĭ�ϸ߶� */

        if (bsp_8080_lcd_parameter.id == 0X1963)
        {
            bsp_8080_lcd_parameter.wramcmd = 0X2C;  /* ����д��GRAM��ָ�� */
            bsp_8080_lcd_parameter.setxcmd = 0X2A;  /* ����дX����ָ�� */
            bsp_8080_lcd_parameter.setycmd = 0X2B;  /* ����дY����ָ�� */
            bsp_8080_lcd_parameter.width = 800;     /* ���ÿ��800 */
            bsp_8080_lcd_parameter.height = 480;    /* ���ø߶�480 */
        }
        else   /* ����IC, ����: 9341 / 5310 / 7789��IC */
        {
            bsp_8080_lcd_parameter.wramcmd = 0X2C;
            bsp_8080_lcd_parameter.setxcmd = 0X2A;
            bsp_8080_lcd_parameter.setycmd = 0X2B;
        }


        if (bsp_8080_lcd_parameter.id == 0X9488)    /* �����5310 ���ʾ�� 320*480�ֱ��� */
        {
            bsp_8080_lcd_parameter.width = 480;
            bsp_8080_lcd_parameter.height = 320;
        }
    }
}

/**
 * ˵��       ���ô���(��RGB����Ч),���Զ����û������굽�������Ͻ�(sx,sy).
 * ����       sx,sy:������ʼ����(���Ͻ�)
 * ����       width,height:���ڿ�Ⱥ͸߶�,�������0!!
 *   @note      �����С:width*height.
 *
 * ����ֵ      ��
 */
void bsp_8080_lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t twidth, theight;
    
    if(sx>bsp_8080_lcd_parameter.width || sy>bsp_8080_lcd_parameter.height )
    {
        return;
    }
    
    twidth = sx + width - 1;
    theight = sy + height - 1;

    if (bsp_8080_lcd_parameter.id == 0X1963 && bsp_8080_lcd_parameter.dir != 1)    /* 1963�������⴦�� */
    {
        sx = bsp_8080_lcd_parameter.width - width - sx;
        height = sy + height - 1;
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setxcmd);
        bsp_8080_lcd_wr_data(sx >> 8);
        bsp_8080_lcd_wr_data(sx & 0XFF);
        bsp_8080_lcd_wr_data((sx + width - 1) >> 8);
        bsp_8080_lcd_wr_data((sx + width - 1) & 0XFF);
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setycmd);
        bsp_8080_lcd_wr_data(sy >> 8);
        bsp_8080_lcd_wr_data(sy & 0XFF);
        bsp_8080_lcd_wr_data(height >> 8);
        bsp_8080_lcd_wr_data(height & 0XFF);
    }
    else    /* 9341/5310/7789/1963���� �� ���ô��� */
    {
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setxcmd);
        bsp_8080_lcd_wr_data(sx >> 8);
        bsp_8080_lcd_wr_data(sx & 0XFF);
        bsp_8080_lcd_wr_data(twidth >> 8);
        bsp_8080_lcd_wr_data(twidth & 0XFF);
        bsp_8080_lcd_wr_regno(bsp_8080_lcd_parameter.setycmd);
        bsp_8080_lcd_wr_data(sy >> 8);
        bsp_8080_lcd_wr_data(sy & 0XFF);
        bsp_8080_lcd_wr_data(theight >> 8);
        bsp_8080_lcd_wr_data(theight & 0XFF);
    }
}


/**
 * ˵��       ��������
 * ����       color: Ҫ��������ɫ
 * ����ֵ      ��
 */
void bsp_8080_lcd_clear(uint16_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = bsp_8080_lcd_parameter.width;
    bsp_8080_lcd_parameter.g_back_color = color;
    
    bsp_8080_lcd_parameter.g_point_color = ~color;

    totalpoint *= bsp_8080_lcd_parameter.height;    /* �õ��ܵ��� */
    bsp_8080_lcd_set_cursor(0x00, 0x0000);   /* ���ù��λ�� */
    EXMC_LCD_C = bsp_8080_lcd_parameter.wramcmd;        /* ��ʼд��GRAM */

    for (index = 0; index < totalpoint; index++)
    {
        EXMC_LCD_D = color;
   }    
}

/**
 * ˵��       ��ָ����������䵥����ɫ
 * ����       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 * ����       color: Ҫ������ɫ(32λ��ɫ,�������LTDC)
 * ����ֵ      ��
 */
void bsp_8080_lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    uint16_t height, width,x,y;
    uint32_t i;
    if(sx>bsp_8080_lcd_parameter.width || ex>bsp_8080_lcd_parameter.width || sy>bsp_8080_lcd_parameter.height || ey>bsp_8080_lcd_parameter.height)
    {
        return;
    }
    
    width = ex - sx + 1;            /* �õ����Ŀ�� */
    height = ey - sy + 1;           /* �߶� */

    if(bsp_8080_lcd_parameter.display_mode == LCD_MODE)
    {    
        bsp_8080_lcd_set_window(sx,sy,width,height);
        EXMC_LCD_C = bsp_8080_lcd_parameter.wramcmd;    
        for(i = 0; i < height*width; i++)
        {
            EXMC_LCD_D = color;     
        }
    }
    else
    {    
        for(y = sy; y < sy + height; y++){      
            for(x = sx; x < sx + width; x++){ 
                *(__IO uint16_t*)( (uint32_t)(bsp_8080_lcd_parameter.display_ram) + 2*((bsp_8080_lcd_parameter.width*y) + x) ) = color;
            }                
        } 
    }        
      
//    if(bsp_8080_lcd_parameter.display_mode == LCD_MODE)
//    {
//        bsp_8080_lcd_set_cursor(x, y);       /* ���ù��λ�� */
//        EXMC_LCD_C = bsp_8080_lcd_parameter.wramcmd;    /* ��ʼд��GRAM */
//        EXMC_LCD_D = color;
//    }else
//    {
//        *(__IO uint16_t*)(((uint32_t)(&bsp_8080_lcd_parameter.display_ram)) + 2*((bsp_8080_lcd_parameter.width*y) + x)) = color;        
//    }    
}

/**
 * ˵��       ��ָ�����������ָ����ɫ��
 * ����       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 * ����       color: Ҫ������ɫ�����׵�ַ
 * ����ֵ      ��
 */
void bsp_8080_lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t height, width;
    if(sx>bsp_8080_lcd_parameter.width || ex>bsp_8080_lcd_parameter.width || sy>bsp_8080_lcd_parameter.height || ey>bsp_8080_lcd_parameter.height)
    {
        return;
    }    
//    uint32_t i;
    width = ex - sx + 1;            /* �õ����Ŀ�� */
    height = ey - sy + 1;           /* �߶� */
    
    
    if(bsp_8080_lcd_parameter.display_mode == LCD_MODE)
    {   
        bsp_8080_lcd_set_window(sx,sy,width,height);
        EXMC_LCD_C = bsp_8080_lcd_parameter.wramcmd;
        driver_dma_mem_to_exmclcd_start((void*)&EXMC_LCD_D,&color[0],DMA_Width_16BIT,height*width);     
//        for(uin32_t i = 0; i < height*width; i++)
//        {
//            EXMC_LCD_D = color[i];     
//        }
    }    
    else 
    {       
        for(uint16_t y = sy; y <= ey; y++)
        {
            for(uint16_t x = sx; x <= ex; x++)
            {
                bsp_8080_lcd_parameter.display_ram[y*width+x]=color[(y-sy)*width+(x-sx)];
            }
        }
    }
    
}

/**
 * ˵��       ˢ��LCD
 * ����       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 * ����       color: Ҫ������ɫ�����׵�ַ
 * ����ֵ      ��
 */
void bsp_8080_lcd_sram_mode_updata(void)
{
    if(bsp_8080_lcd_parameter.display_mode == SRAM_MODE)
    {       
        bsp_8080_lcd_color_fill(0,0,bsp_8080_lcd_parameter.width-1,bsp_8080_lcd_parameter.height-1,bsp_8080_lcd_parameter.display_ram);
        
        bsp_8080_lcd_set_window(0,0,bsp_8080_lcd_parameter.width,bsp_8080_lcd_parameter.height);
        EXMC_LCD_C = bsp_8080_lcd_parameter.wramcmd;
        driver_dma_mem_to_exmclcd_start((void*)&EXMC_LCD_D,bsp_8080_lcd_parameter.display_ram,DMA_Width_16BIT,bsp_8080_lcd_parameter.height*bsp_8080_lcd_parameter.width);          
    }
}

/**
 * ˵��       ����
 * ����       x1,y1: �������
 * ����       x2,y2: �յ�����
 * ����       color: �ߵ���ɫ
 * ����ֵ      ��
 */
void bsp_8080_lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    delta_x = x2 - x1;          /* ������������ */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)incx = 1;   /* ���õ������� */
    else if (delta_x == 0)incx = 0; /* ��ֱ�� */
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; /* ˮƽ�� */
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x;  /* ѡȡ�������������� */
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )   /* ������� */
    {
        bsp_8080_lcd_draw_point(row, col, color); /* ���� */
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * ˵��       ��ˮƽ��
 * ����       x0,y0: �������
 * ����       len  : �߳���
 * ����       color: ���ε���ɫ
 * ����ֵ      ��
 */
void bsp_8080_lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > bsp_8080_lcd_parameter.width) || (y > bsp_8080_lcd_parameter.height))return;

    bsp_8080_lcd_fill(x, y, x + len - 1, y, color);
}

/**
 * ˵��       ������
 * ����       x1,y1: �������
 * ����       x2,y2: �յ�����
 * ����       color: ���ε���ɫ
 * ����ֵ      ��
 */
void bsp_8080_lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    bsp_8080_lcd_draw_line(x1, y1, x2, y1, color);
    bsp_8080_lcd_draw_line(x1, y1, x1, y2, color);
    bsp_8080_lcd_draw_line(x1, y2, x2, y2, color);
    bsp_8080_lcd_draw_line(x2, y1, x2, y2, color);
}

/**
 * ˵��       ��Բ
 * ����       x,y  : Բ��������
 * ����       r    : �뾶
 * ����       color: Բ����ɫ
 * ����ֵ      ��
 */
void bsp_8080_lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       /* �ж��¸���λ�õı�־ */

    while (a <= b)
    {
        bsp_8080_lcd_draw_point(x0 + a, y0 - b, color);  /* 5 */
        bsp_8080_lcd_draw_point(x0 + b, y0 - a, color);  /* 0 */
        bsp_8080_lcd_draw_point(x0 + b, y0 + a, color);  /* 4 */
        bsp_8080_lcd_draw_point(x0 + a, y0 + b, color);  /* 6 */
        bsp_8080_lcd_draw_point(x0 - a, y0 + b, color);  /* 1 */
        bsp_8080_lcd_draw_point(x0 - b, y0 + a, color);
        bsp_8080_lcd_draw_point(x0 - a, y0 - b, color);  /* 2 */
        bsp_8080_lcd_draw_point(x0 - b, y0 - a, color);  /* 7 */
        a++;

        /* ʹ��Bresenham�㷨��Բ */
        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/**
 * ˵��       ���ʵ��Բ
 * ����       x0,y0: Բ��������
 * ����       r    : �뾶
 * ����       color: Բ����ɫ
 * ����ֵ      ��
 */
void bsp_8080_lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    uint32_t i;
    uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
    uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    uint32_t xr = r;

    bsp_8080_lcd_draw_hline(x - r, y, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
        if ((i * i + xr * xr) > sqmax)
        {
            /* draw lines from outside */
            if (xr > imax)
            {
                bsp_8080_lcd_draw_hline (x - i + 1, y + xr, 2 * (i - 1), color);
                bsp_8080_lcd_draw_hline (x - i + 1, y - xr, 2 * (i - 1), color);
            }

            xr--;
        }

        /* draw lines from inside (center) */
        bsp_8080_lcd_draw_hline(x - xr, y + i, 2 * xr, color);
        bsp_8080_lcd_draw_hline(x - xr, y - i, 2 * xr, color);
    }
}

/**
 * ˵��       ��ָ��λ����ʾһ���ַ�
 * ����       x,y  : ����
 * ����       chr  : Ҫ��ʾ���ַ�:" "--->"~"
 * ����       size : �����С FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * ����       mode : ���ӷ�ʽ(1); �ǵ��ӷ�ʽ(0);
 * ����ֵ      ��
 */
void bsp_8080_lcd_show_char(uint16_t x, uint16_t y, char chr, FONT_ASCII size, uint8_t mode, uint16_t color,uint16_t back_color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* �õ�����һ���ַ���Ӧ������ռ���ֽ��� */
    chr = chr - ' ';    /* �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩ */

    switch (size)
    {
        case FONT_ASCII_12_6:
            pfont = (uint8_t *)ascii_12_6[chr];  /* ����1206���� */
            break;

        case FONT_ASCII_16_8:
            pfont = (uint8_t *)ascii_16_8[chr];  /* ����1608���� */
            break;

        case FONT_ASCII_24_12:
            pfont = (uint8_t *)ascii_24_12[chr];  /* ����2412���� */
            break;

        case FONT_ASCII_32_16:
            pfont = (uint8_t *)ascii_32_16[chr];  /* ����3216���� */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];    /* ��ȡ�ַ��ĵ������� */

        for (t1 = 0; t1 < 8; t1++)   /* һ���ֽ�8���� */
        {
            if (temp & 0x80)        /* ��Ч��,��Ҫ��ʾ */
            {
                bsp_8080_lcd_draw_point(x, y, color);        /* �������,Ҫ��ʾ����� */
            }
            else if (mode == 0)     /* ��Ч��,����ʾ */
            {
                bsp_8080_lcd_draw_point(x, y, back_color); /* ������ɫ,�൱������㲻��ʾ(ע�ⱳ��ɫ��ȫ�ֱ�������) */
            }

            temp <<= 1; /* ��λ, �Ա��ȡ��һ��λ��״̬ */
            y++;

            if (y >= bsp_8080_lcd_parameter.height)return;  /* �������� */

            if ((y - y0) == size)   /* ��ʾ��һ����? */
            {
                y = y0; /* y���긴λ */
                x++;    /* x������� */

                if (x >= bsp_8080_lcd_parameter.width)return;   /* x���곬������ */

                break;
            }
        }
    }
}

/**
 * ˵��       ƽ������, m^n
 * ����       m: ����
 * ����       n: ָ��
 * ����ֵ      m��n�η�
 */
static uint32_t bsp_8080_lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)result *= m;

    return result;
}

/**
 * ˵��       ��ʾlen������
 * ����       x,y : ��ʼ����
 * ����       num : ��ֵ(0 ~ 2^32)
 * ����       len : ��ʾ���ֵ�λ��
 * ����       size: ѡ������ FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * ����ֵ      ��
 */
void bsp_8080_lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, FONT_ASCII size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* ������ʾλ��ѭ�� */
    {
        temp = (num / bsp_8080_lcd_pow(10, len - t - 1)) % 10;   /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))   /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                bsp_8080_lcd_show_char(x + (size / 2)*t, y, ' ', size, 0, color,bsp_8080_lcd_parameter.g_back_color);/* ��ʾ�ո�,ռλ */
                continue;   /* �����¸�һλ */
            }
            else
            {
                enshow = 1; /* ʹ����ʾ */
            }

        }

        bsp_8080_lcd_show_char(x + (size / 2)*t, y, temp + '0', size, 0, color,bsp_8080_lcd_parameter.g_back_color); /* ��ʾ�ַ� */
    }
}

/**
 * ˵��       ��չ��ʾlen������(��λ��0Ҳ��ʾ)
 * ����       x,y : ��ʼ����
 * ����       num : ��ֵ(0 ~ 2^32)
 * ����       len : ��ʾ���ֵ�λ��
 * ����       size: ѡ������ FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * ����       mode: ��ʾģʽ
 *              [7]:0,�����;1,���0.
 *              [6:1]:����
 *              [0]:0,�ǵ�����ʾ;1,������ʾ.
 *
 * ����ֵ      ��
 */
void bsp_8080_lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, FONT_ASCII size, uint8_t mode, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* ������ʾλ��ѭ�� */
    {
        temp = (num / bsp_8080_lcd_pow(10, len - t - 1)) % 10;    /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))   /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                if (mode & 0X80)   /* ��λ��Ҫ���0 */
                {
                    bsp_8080_lcd_show_char(x + (size / 2)*t, y, '0', size, mode & 0X01, color,bsp_8080_lcd_parameter.g_back_color);  /* ��0ռλ */
                }
                else
                {
                    bsp_8080_lcd_show_char(x + (size / 2)*t, y, ' ', size, mode & 0X01, color,bsp_8080_lcd_parameter.g_back_color);  /* �ÿո�ռλ */
                }

                continue;
            }
            else
            {
                enshow = 1; /* ʹ����ʾ */
            }

        }

        bsp_8080_lcd_show_char(x + (size / 2)*t, y, temp + '0', size, mode & 0X01, color,bsp_8080_lcd_parameter.g_back_color);
    }
}

/**
 * ˵��       ��ʾ�ַ���
 * ����       x,y         : ��ʼ����
 * ����       width,height: �����С
 * ����       size        : ѡ������ FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * ����       p           : �ַ����׵�ַ
 * ����ֵ      ��
 */
void bsp_8080_lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, FONT_ASCII size, char *p, uint16_t color)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while( ((*p <= '~') && (*p >= ' ')) || (*p =='\r') || (*p =='\n') )   /* �ж��ǲ��ǷǷ��ַ�! */
    {
        if((*p =='\r'))
        {
            x = x0;
        }        
        else if((*p =='\n'))
        {
            y += size;        
        }
        else if (x >= width)
        {
            x = x0;
            y += size;
        }
        if (y >= height)break;  /* �˳� */

        bsp_8080_lcd_show_char(x, y, *p, size, 0, color,bsp_8080_lcd_parameter.g_back_color);
        x += size / 2;
        p++;
    }
}

/**
 * ˵��       LCD��ӡ���ڳ�ʼ��
 * ����       x,y         : ��ʼ����
 * ����       x_end,y_end:  ����������
 * ����       size        : ѡ������ FONT_ASCII_12_6/FONT_ASCII_16_8/FONT_ASCII_24_12/FONT_ASCII_32_16
 * ����       back_color  : ������ɫ
 * ����       point_color   ������ɫ
 * ����ֵ      ��
 */
void bsp_8080_lcd_printf_init(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end,FONT_ASCII char_size,uint16_t back_color,uint16_t point_color)
{
    bsp_8080_lcd_pritnf_parameter.x_start=x_start;
    bsp_8080_lcd_pritnf_parameter.y_start=y_start;

    bsp_8080_lcd_pritnf_parameter.x_end=x_end;
    bsp_8080_lcd_pritnf_parameter.y_end=y_end;

    bsp_8080_lcd_pritnf_parameter.x_now=x_start;
    bsp_8080_lcd_pritnf_parameter.y_now=y_start;

    bsp_8080_lcd_pritnf_parameter.size=char_size;

    bsp_8080_lcd_pritnf_parameter.back_color=back_color;   
    bsp_8080_lcd_pritnf_parameter.point_color=point_color; 

    bsp_8080_lcd_fill(bsp_8080_lcd_pritnf_parameter.x_start,bsp_8080_lcd_pritnf_parameter.y_start,bsp_8080_lcd_pritnf_parameter.x_end,bsp_8080_lcd_pritnf_parameter.y_end,bsp_8080_lcd_pritnf_parameter.back_color);    
}


/**
 * ˵��       LCD��ӡ
 * ����       ...��printf��ͬ�÷����Զ�����
 * ����ֵ      ��
 */
void bsp_8080_lcd_printf(const char * sFormat, ...) 
{
    #define PRINTF_MAX_LEN 100
    char printf_buffer[PRINTF_MAX_LEN];
    char* p=printf_buffer;
    uint16_t len=0,count=0;
    va_list ParamList;
    va_start(ParamList, sFormat);     
    vsprintf(printf_buffer,sFormat, ParamList);     
    va_end(ParamList); 

    len=strlen(printf_buffer);    

    while( ( ((*p <= '~') && (*p >= ' ')) || (*p =='\r') || (*p =='\n') ) && (count<len) )   /* �ж��ǲ��ǷǷ��ַ�! */
    {
        if((*p =='\r'))
        {
            bsp_8080_lcd_pritnf_parameter.x_now = bsp_8080_lcd_pritnf_parameter.x_start;
        }        
        else if((*p =='\n'))
        {
            bsp_8080_lcd_pritnf_parameter.y_now += bsp_8080_lcd_pritnf_parameter.size; 
            bsp_8080_lcd_fill(bsp_8080_lcd_pritnf_parameter.x_now,bsp_8080_lcd_pritnf_parameter.y_now,bsp_8080_lcd_pritnf_parameter.x_end,bsp_8080_lcd_pritnf_parameter.y_now+bsp_8080_lcd_pritnf_parameter.size,bsp_8080_lcd_pritnf_parameter.back_color);            
        }
        else if( (bsp_8080_lcd_pritnf_parameter.x_now + bsp_8080_lcd_pritnf_parameter.size/2) > bsp_8080_lcd_pritnf_parameter.x_end)
        {
            bsp_8080_lcd_pritnf_parameter.x_now = bsp_8080_lcd_pritnf_parameter.x_start;
            bsp_8080_lcd_pritnf_parameter.y_now += bsp_8080_lcd_pritnf_parameter.size;
            bsp_8080_lcd_fill(bsp_8080_lcd_pritnf_parameter.x_now,bsp_8080_lcd_pritnf_parameter.y_now,bsp_8080_lcd_pritnf_parameter.x_end,bsp_8080_lcd_pritnf_parameter.y_now+bsp_8080_lcd_pritnf_parameter.size,bsp_8080_lcd_pritnf_parameter.back_color);                        
        }
        else if ( (bsp_8080_lcd_pritnf_parameter.y_now+bsp_8080_lcd_pritnf_parameter.size) > bsp_8080_lcd_pritnf_parameter.y_end)
        {
            bsp_8080_lcd_pritnf_parameter.x_now = bsp_8080_lcd_pritnf_parameter.x_start;
            bsp_8080_lcd_pritnf_parameter.y_now = bsp_8080_lcd_pritnf_parameter.y_start;            
            bsp_8080_lcd_fill(bsp_8080_lcd_pritnf_parameter.x_start,bsp_8080_lcd_pritnf_parameter.y_start,bsp_8080_lcd_pritnf_parameter.x_end,bsp_8080_lcd_pritnf_parameter.y_end,bsp_8080_lcd_pritnf_parameter.back_color);
        }

        if((*p !='\r')&&(*p !='\n'))
        {
            bsp_8080_lcd_show_char(bsp_8080_lcd_pritnf_parameter.x_now, bsp_8080_lcd_pritnf_parameter.y_now, *p, bsp_8080_lcd_pritnf_parameter.size, 0, bsp_8080_lcd_pritnf_parameter.point_color,bsp_8080_lcd_pritnf_parameter.back_color);
            bsp_8080_lcd_pritnf_parameter.x_now += bsp_8080_lcd_pritnf_parameter.size / 2;            
        }        
        p++;
        count++;
    }

}


void bsp_8080_lcd_show_log(void)
{
    //��ʾlogͼƬ
    bsp_8080_lcd_color_fill(60,0,233,99,(uint16_t*)gd_log_picture);
}



