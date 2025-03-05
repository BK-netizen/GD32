/*!
 * �ļ����ƣ� driver_gpio.c
 * ��    ���� gpio �����ļ�
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

#include "driver_gpio.h"

/*!  GPIO����ע��ʾ��

GPIO_DEF(LED2,E,5,OUT_PP,SET,NULL);              // PE5����ΪLED2,OUTPPģʽ��Ĭ�ϵ�ƽ�ߣ��޻ص�����

GPIO_DEF(KEY0,E,2,INT_LOW, SET,KEY0_IRQHandler); // PE2����ΪKEY0,�͵�ƽ�����ж�ģʽ��Ĭ�ϵ�ƽ�ߣ��жϻص�ִ��KEY0_IRQHandler����

GPIO_DEF(USER_KEY,E,3,IN_PU,SET,NULL);               // PE3����ΪUSER_KEY,��������ģʽ��Ĭ�ϵ�ƽ��

AFIO_DEF(UART0_TX,B,6,AF_PP,GPIO_USART0_REMAP);  //����PB6����ΪUART0_TX����������ģʽ��������ӳ�����ΪGPIO_USART0_REMAP

*/

/*!
* ˵��     gpio��ʼ��
* ����[1]  gpioע�����
* ����ֵ   ��
*/
void driver_gpio_general_init(typdef_gpio_general *gpio)
{
    const static uint8_t MODE_TABLE[]={GPIO_MODE_OUTPUT,GPIO_MODE_OUTPUT,GPIO_MODE_INPUT,GPIO_MODE_INPUT,GPIO_MODE_INPUT,GPIO_MODE_INPUT,GPIO_MODE_INPUT,GPIO_MODE_AF,GPIO_MODE_AF};
    const static uint8_t PULL_MODE[]={GPIO_PUPD_NONE,GPIO_PUPD_PULLUP,GPIO_PUPD_NONE,GPIO_PUPD_PULLUP,GPIO_PUPD_PULLDOWN,GPIO_PUPD_PULLUP,GPIO_PUPD_PULLDOWN,GPIO_PUPD_NONE,GPIO_PUPD_PULLUP};
    const static uint8_t OUT_TYPE[]={GPIO_OTYPE_PP,GPIO_OTYPE_OD,GPIO_OTYPE_OD,GPIO_OTYPE_OD,GPIO_OTYPE_OD,GPIO_OTYPE_OD,GPIO_OTYPE_OD,GPIO_OTYPE_PP,GPIO_OTYPE_OD};

    
    if(gpio!=NULL){
        /* ��GPIOʱ�� */
        rcu_periph_clock_enable(gpio->rcu_port);
        rcu_periph_clock_enable(RCU_SYSCFG);
        
        /* ����ģʽԤ�� */
        if (gpio->gpio_mode == AF_PP || gpio->gpio_mode == AF_OD)
        {
            gpio_af_set(gpio->port,gpio->afio_mode,gpio->pin);
        }    
            
        /* Ԥ��IO�����ƽ */
        if (gpio->gpio_mode == OUT_PP || gpio->gpio_mode == OUT_OD)
        {
            gpio_bit_write(gpio->port,gpio->pin,gpio->default_state);
        }
        
        /* ��ʼ��GPIOģʽ */
        gpio_mode_set(gpio->port,MODE_TABLE[gpio->gpio_mode],PULL_MODE[gpio->gpio_mode],gpio->pin);    
        /* ��ʼ��GPIO���ģʽ */
        gpio_output_options_set(gpio->port,OUT_TYPE[gpio->gpio_mode],gpio->speed,gpio->pin);       
      
        if (gpio->gpio_mode == INT_HIGH || gpio->gpio_mode == INT_LOW)
        {
            //* EXTIԴ���� */
//            gpio_exti_source_select(gpio->int_port,gpio->int_pin);            
            syscfg_exti_line_config(gpio->int_port,gpio->int_pin);
            
            /* ��ӦEXTI��ģʽ���� */
            if(gpio->gpio_mode==INT_HIGH){
                exti_init(gpio->extix, EXTI_INTERRUPT, EXTI_TRIG_RISING);
            }else{
                exti_init(gpio->extix, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
            }            
            exti_interrupt_flag_clear(gpio->extix);
        }
    }
}

/*!
* ˵��     gpio���ֵ����
* ����[1]  gpioע�����
* ����[2]  gpio���ֵ
* ����ֵ   ��
*/
void driver_gpio_pin_write(  typdef_gpio_general *gpio, bit_status bit_vaule)
{
    gpio_bit_write(gpio->port,gpio->pin,bit_vaule);
}

/*!
* ˵��     gpio�����
* ����[1]  gpioע�����
* ����ֵ   ��
*/
void driver_gpio_pin_set(  typdef_gpio_general *gpio)
{
    gpio_bit_set(gpio->port, gpio->pin);
}

/*!
* ˵��     gpio�����
* ����[1]  gpioע�����
* ����ֵ   ��
*/
void driver_gpio_pin_reset(  typdef_gpio_general *gpio)
{
    gpio_bit_reset(gpio->port, gpio->pin);
}

/*!
* ˵��     gpio�����ת
* ����[1]  gpioע�����
* ����ֵ   ��
*/
void driver_gpio_pin_toggle(  typdef_gpio_general *gpio)
{
    gpio_bit_write(gpio->port,gpio->pin,(bit_status)(gpio_input_bit_get(gpio->port, gpio->pin)^1));
}

/*!
* ˵��     gpio�����ȡ
* ����[1]  gpioע�����
* ����ֵ   ��
*/
bit_status driver_gpio_pin_get( typdef_gpio_general *gpio)
{
    return (bit_status)gpio_input_bit_get(gpio->port, gpio->pin);
}

/*!
* ˵��     gpio�����˲���ȡ
* ����[1]  gpioע�����
* ����ֵ   ��
*/
bit_status dvire_gpio_pin_filter_get(typdef_gpio_general *gpio,uint8_t filter_num)
{
        uint8_t bit_num=0;

        for(uint8_t i=0;i<filter_num;i++)
        {
            delay_sysclk(20);
            bit_num+=driver_gpio_pin_get(gpio);
        }
        if(bit_num<filter_num/2)
            return RESET;
        else
            return SET;
}

/*!
* ˵��     gpio�����ж�ģʽ�жϷ���������
* ����[1]  gpioע�����
* ����ֵ   ��
*/
void dvire_gpio_exti_handle(typdef_gpio_general *gpio)
{
    bit_status int_input_bit=RESET;
    if(exti_flag_get(gpio->extix)==SET)
    {
        exti_flag_clear(gpio->extix);
        //int_input_bit=dvire_gpio_pin_filter_get(gpio,10);

        if( (gpio->gpio_mode==INT_LOW && int_input_bit==RESET) || (gpio->gpio_mode==INT_HIGH && int_input_bit==SET) )
        {
            if(gpio->int_callback!=NULL)
            {                
                gpio->int_callback((typdef_gpio_general *)gpio);                
            }    
        }   
    }
}

