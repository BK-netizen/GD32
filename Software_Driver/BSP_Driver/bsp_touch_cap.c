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
#include "bsp_touch_cap.h"
#include "stdio.h"
#include "string.h"
#include "bsp_software_i2c.h"

typde_lcd_touch_cap_struct lcd_touch_cap_struct;



GPIO_DEF(TOUCH_CAP_RST,B,5,OUT_PP,RESET,NULL);

GPIO_DEF(TOUCH_CAP_INT,D,11,OUT_PP,RESET,NULL);


const uint16_t GT911_TPX_TBL[5]={GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};


// ���ݴ���оƬIIC�ӿڳ�ʼ��
static void bsp_touch_cap_iic_init(void)
{
    bsp_software_i2c_init(&TOUCH_CAP_IIC);
}

//��GT911д��һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:д���ݳ���
//����ֵ:0,�ɹ�;1,ʧ��.
static uint8_t bsp_gt911_wr_reg(uint16_t reg,uint8_t *buf,uint8_t len)
{    
    bsp_software_i2c_mem16_write(&TOUCH_CAP_IIC,GT_CMD_WR>>1,reg,len,buf);    
	return 0; 
}
//��GT911����һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:�����ݳ���			  
static void bsp_gt911_rd_reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
    bsp_software_i2c_mem16_read(&TOUCH_CAP_IIC,GT_CMD_WR>>1,reg,len,buf);  
} 

// ��ʼ��gt911������
uint8_t bsp_gt911_init(uint16_t width,uint16_t height)
{
    uint8_t temp[5];
    
    lcd_touch_cap_struct.width=width;
    lcd_touch_cap_struct.height=height;
    // PD11����Ϊ��������(INT)
    /* enable the led clock */
    driver_gpio_general_init(&TOUCH_CAP_RST);
    driver_gpio_general_init(&TOUCH_CAP_INT);
    
    
    delay_ms(1);
//        /* configure led GPIO port */
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_15);

    // ��ʼ����������I2C����
    bsp_touch_cap_iic_init();

    delay_ms(10);
    driver_gpio_pin_set(&TOUCH_CAP_RST);
    delay_ms(100); 
    
    TOUCH_CAP_INT.gpio_mode=IN_NONE;
    driver_gpio_general_init(&TOUCH_CAP_INT);   

    bsp_gt911_rd_reg(GT_PID_REG,temp,4);//��ȡ��ƷID

    temp[4]=0;
	if(strcmp((char*)temp,"911")==0)//ID==911
	{
		temp[0]=0X02;			
		bsp_gt911_wr_reg(GT_CTRL_REG,temp,1);//��λGT911
		bsp_gt911_rd_reg(GT_CFGS_REG,temp,1);//��ȡGT_CFGS_REG�Ĵ���
		if(temp[0]<0X60)//Ĭ�ϰ汾�Ƚϵ�,��Ҫ����flash����
		{
			printf("Default Ver:%d\r\n",temp[0]);
		}
		delay_ms(10);
		temp[0]=0X00;	 
		bsp_gt911_wr_reg(GT_CTRL_REG,temp,1);//������λ   
		return 0;
	}
    return 0;
}

//ɨ�败����(���ò�ѯ��ʽ)
//mode:0,����ɨ��.
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
uint8_t bsp_gt911_scan(uint8_t mode)
{
	uint8_t buf[4];
	uint8_t i=0;
	uint8_t res=0;
	uint8_t temp;
    uint16_t temp_x=0,temp_y=0;;
//    uint16_t tempsta;
	static uint16_t t=0;//���Ʋ�ѯ���,�Ӷ�����CPUռ����   
	t++;
	if((t%5)==0||t<5)//����ʱ,ÿ����10��CTP_Scan�����ż��1��,�Ӷ���ʡCPUʹ����
	{
		bsp_gt911_rd_reg(GT_GSTID_REG,&mode,1);//��ȡ�������״̬ 
		//printf("mode:0x%x\r\n",mode);
		if((mode&0XF)&&((mode&0XF)<=g_gt_tnum))
		{
			temp=0XFF<<(mode&0XF);//����ĸ���ת��Ϊ1��λ��,ƥ��lcd_touch_cap_struct.sta���� 
//            tempsta = lcd_touch_cap_struct.sta;        // ���浱ǰ��lcd_touch_cap_struct.staֵ
			lcd_touch_cap_struct.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			for(i=0;i<5;i++)
			{
				if(lcd_touch_cap_struct.sta&(1<<i))	//������Ч?
				{
					bsp_gt911_rd_reg(GT911_TPX_TBL[i],buf,4);	//��ȡXY����ֵ
                    if (!(lcd_touch_cap_struct.touchtype & 0X01))              // ����
					{
                        temp_x=((uint16_t)buf[1]<<8)+buf[0];
						temp_y=(((uint16_t)buf[3]<<8)+buf[2]);
                        
					}else //����
					{
                        temp_y=((uint16_t)buf[1]<<8)+buf[0];
						temp_x=lcd_touch_cap_struct.width-(((uint16_t)buf[3]<<8)+buf[2]);
					} 
                    
//                    if (!(lcd_touch_cap_struct.touchtype & 0X01))              // ����
//					{
//						lcd_touch_cap_struct.x[i]=((uint16_t)buf[1]<<8)+buf[0];
//						lcd_touch_cap_struct.y[i]=(((uint16_t)buf[3]<<8)+buf[2]);
//					}else //����
//					{
//						lcd_touch_cap_struct.y[i]=((uint16_t)buf[1]<<8)+buf[0];
//						lcd_touch_cap_struct.x[i]=lcd_touch_cap_struct.width-(((uint16_t)buf[3]<<8)+buf[2]);
//					}                      
                    
                    if(temp_x > lcd_touch_cap_struct.width || temp_y > lcd_touch_cap_struct.height)
                    {
                        lcd_touch_cap_struct.sta&=~(1<<i);    
                    }else
                    {
                        lcd_touch_cap_struct.x[i]=temp_x;
                        lcd_touch_cap_struct.y[i]=temp_y;  
                    }
				}			
			} 
			res=1;
            t = 0; // ����һ��,��������������10��,�Ӷ����������
		}
 		if(mode&0X80&&((mode&0XF)<6))
		{
			temp=0;
			bsp_gt911_wr_reg(GT_GSTID_REG,&temp,1);//���־ 		
		}
	}
	if((mode&0X8F)==0X80)//�޴����㰴��
	{ 
		if(lcd_touch_cap_struct.sta&TP_PRES_DOWN)	//֮ǰ�Ǳ����µ�
		{
			lcd_touch_cap_struct.sta&=~(1<<7);	//��ǰ����ɿ�
		}else						//֮ǰ��û�б�����
		{ 
			lcd_touch_cap_struct.x[0]=0xffff;
			lcd_touch_cap_struct.y[0]=0xffff;
			lcd_touch_cap_struct.sta&=0XE0;	//�������Ч���	
		} 
	} 
	if(t>0xfff0) 
    {
        
        t=0;//���´�10��ʼ����
        temp=2;
        driver_gpio_pin_reset(&TOUCH_CAP_RST);
        delay_ms(1);
        driver_gpio_pin_set(&TOUCH_CAP_RST);
    }
	return res;
}
