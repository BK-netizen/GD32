#ifndef __BSP_TOUCH_CAP_H__
#define __BSP_TOUCH_CAP_H__

#include "driver_public.h"

#define g_gt_tnum  5 // Ĭ��֧�ֵĴ���������(5�㴥��)

#define TP_PRES_DOWN            0x80 // ����������
#define TP_CATH_PRES            0x40 // �а���������
#define CT_MAX_TOUCH 5    // ������֧�ֵĵ���,�̶�Ϊ5��


//I2C��д����	
#define GT_CMD_WR 		0XBA    //д����
#define GT_CMD_RD 		0XBB		//������
  
//GT9147 ���ּĴ������� 
#define GT_CTRL_REG 	0X8040   	//GT911���ƼĴ���
#define GT_CFGS_REG 	0X8047   	//GT911������ʼ��ַ�Ĵ���
#define GT_CHECK_REG 	0X80FF   	//GT911У��ͼĴ���
#define GT_PID_REG 		0X8140   	//GT911��ƷID�Ĵ���

#define GT_GSTID_REG 	0X814E   	//GT911��ǰ��⵽�Ĵ������
#define GT_TP1_REG 		0X8150  	//��һ�����������ݵ�ַ
#define GT_TP2_REG 		0X8158		//�ڶ������������ݵ�ַ
#define GT_TP3_REG 		0X8160		//���������������ݵ�ַ
#define GT_TP4_REG 		0X8168		//���ĸ����������ݵ�ַ
#define GT_TP5_REG 		0X8170		//��������������ݵ�ַ  
// ������������
typedef struct
{
    //    uint8_t (*init)(void);			//��ʼ��������������
    //    uint8_t (*scan)(uint8_t);				//ɨ�败����.0,��Ļɨ��;1,��������;
    //    void (*adjust)(void);		//������У׼
    uint16_t x[CT_MAX_TOUCH]; // ��ǰ����
    uint16_t y[CT_MAX_TOUCH]; // �����������5������,����������x[0],y[0]����:�˴�ɨ��ʱ,����������,��
    uint8_t sta;              // �ʵ�״̬
    float xfac;
    float yfac;
    short xoff;
    short yoff;
    uint8_t touchtype;
    uint16_t width;     /* LCD ��� */
    uint16_t height;    /* LCD �߶� */
} typde_lcd_touch_cap_struct;

extern typde_lcd_touch_cap_struct lcd_touch_cap_struct; // ������������touch.c���涨��

uint8_t bsp_gt911_init(uint16_t width,uint16_t height);
uint8_t bsp_gt911_scan(uint8_t mode);

#endif
