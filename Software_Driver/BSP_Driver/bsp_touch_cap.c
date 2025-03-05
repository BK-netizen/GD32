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
#include "bsp_touch_cap.h"
#include "stdio.h"
#include "string.h"
#include "bsp_software_i2c.h"

typde_lcd_touch_cap_struct lcd_touch_cap_struct;



GPIO_DEF(TOUCH_CAP_RST,B,5,OUT_PP,RESET,NULL);

GPIO_DEF(TOUCH_CAP_INT,D,11,OUT_PP,RESET,NULL);


const uint16_t GT911_TPX_TBL[5]={GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};


// 电容触摸芯片IIC接口初始化
static void bsp_touch_cap_iic_init(void)
{
    bsp_software_i2c_init(&TOUCH_CAP_IIC);
}

//向GT911写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
//返回值:0,成功;1,失败.
static uint8_t bsp_gt911_wr_reg(uint16_t reg,uint8_t *buf,uint8_t len)
{    
    bsp_software_i2c_mem16_write(&TOUCH_CAP_IIC,GT_CMD_WR>>1,reg,len,buf);    
	return 0; 
}
//从GT911读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度			  
static void bsp_gt911_rd_reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
    bsp_software_i2c_mem16_read(&TOUCH_CAP_IIC,GT_CMD_WR>>1,reg,len,buf);  
} 

// 初始化gt911触摸屏
uint8_t bsp_gt911_init(uint16_t width,uint16_t height)
{
    uint8_t temp[5];
    
    lcd_touch_cap_struct.width=width;
    lcd_touch_cap_struct.height=height;
    // PD11设置为上拉输入(INT)
    /* enable the led clock */
    driver_gpio_general_init(&TOUCH_CAP_RST);
    driver_gpio_general_init(&TOUCH_CAP_INT);
    
    
    delay_ms(1);
//        /* configure led GPIO port */
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_15);

    // 初始化电容屏的I2C总线
    bsp_touch_cap_iic_init();

    delay_ms(10);
    driver_gpio_pin_set(&TOUCH_CAP_RST);
    delay_ms(100); 
    
    TOUCH_CAP_INT.gpio_mode=IN_NONE;
    driver_gpio_general_init(&TOUCH_CAP_INT);   

    bsp_gt911_rd_reg(GT_PID_REG,temp,4);//读取产品ID

    temp[4]=0;
	if(strcmp((char*)temp,"911")==0)//ID==911
	{
		temp[0]=0X02;			
		bsp_gt911_wr_reg(GT_CTRL_REG,temp,1);//软复位GT911
		bsp_gt911_rd_reg(GT_CFGS_REG,temp,1);//读取GT_CFGS_REG寄存器
		if(temp[0]<0X60)//默认版本比较低,需要更新flash配置
		{
			printf("Default Ver:%d\r\n",temp[0]);
		}
		delay_ms(10);
		temp[0]=0X00;	 
		bsp_gt911_wr_reg(GT_CTRL_REG,temp,1);//结束复位   
		return 0;
	}
    return 0;
}

//扫描触摸屏(采用查询方式)
//mode:0,正常扫描.
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
uint8_t bsp_gt911_scan(uint8_t mode)
{
	uint8_t buf[4];
	uint8_t i=0;
	uint8_t res=0;
	uint8_t temp;
    uint16_t temp_x=0,temp_y=0;;
//    uint16_t tempsta;
	static uint16_t t=0;//控制查询间隔,从而降低CPU占用率   
	t++;
	if((t%5)==0||t<5)//空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
	{
		bsp_gt911_rd_reg(GT_GSTID_REG,&mode,1);//读取触摸点的状态 
		//printf("mode:0x%x\r\n",mode);
		if((mode&0XF)&&((mode&0XF)<=g_gt_tnum))
		{
			temp=0XFF<<(mode&0XF);//将点的个数转换为1的位数,匹配lcd_touch_cap_struct.sta定义 
//            tempsta = lcd_touch_cap_struct.sta;        // 保存当前的lcd_touch_cap_struct.sta值
			lcd_touch_cap_struct.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			for(i=0;i<5;i++)
			{
				if(lcd_touch_cap_struct.sta&(1<<i))	//触摸有效?
				{
					bsp_gt911_rd_reg(GT911_TPX_TBL[i],buf,4);	//读取XY坐标值
                    if (!(lcd_touch_cap_struct.touchtype & 0X01))              // 横屏
					{
                        temp_x=((uint16_t)buf[1]<<8)+buf[0];
						temp_y=(((uint16_t)buf[3]<<8)+buf[2]);
                        
					}else //横屏
					{
                        temp_y=((uint16_t)buf[1]<<8)+buf[0];
						temp_x=lcd_touch_cap_struct.width-(((uint16_t)buf[3]<<8)+buf[2]);
					} 
                    
//                    if (!(lcd_touch_cap_struct.touchtype & 0X01))              // 横屏
//					{
//						lcd_touch_cap_struct.x[i]=((uint16_t)buf[1]<<8)+buf[0];
//						lcd_touch_cap_struct.y[i]=(((uint16_t)buf[3]<<8)+buf[2]);
//					}else //横屏
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
            t = 0; // 触发一次,则会最少连续监测10次,从而提高命中率
		}
 		if(mode&0X80&&((mode&0XF)<6))
		{
			temp=0;
			bsp_gt911_wr_reg(GT_GSTID_REG,&temp,1);//清标志 		
		}
	}
	if((mode&0X8F)==0X80)//无触摸点按下
	{ 
		if(lcd_touch_cap_struct.sta&TP_PRES_DOWN)	//之前是被按下的
		{
			lcd_touch_cap_struct.sta&=~(1<<7);	//标记按键松开
		}else						//之前就没有被按下
		{ 
			lcd_touch_cap_struct.x[0]=0xffff;
			lcd_touch_cap_struct.y[0]=0xffff;
			lcd_touch_cap_struct.sta&=0XE0;	//清除点有效标记	
		} 
	} 
	if(t>0xfff0) 
    {
        
        t=0;//重新从10开始计数
        temp=2;
        driver_gpio_pin_reset(&TOUCH_CAP_RST);
        delay_ms(1);
        driver_gpio_pin_set(&TOUCH_CAP_RST);
    }
	return res;
}
