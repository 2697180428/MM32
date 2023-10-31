/*********************************************************************************************************************
*
* ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* Copyright (c) 2022,DZB
* Author QQ :269718042
* �汾��V1.0
* �ʺ�оƬ���鶯 MM32SPIN27PS
* @Software	  MDK 5.24
* @date				2022-2-5
* @note
* 					���߶��壺
* 					------------------------------------
* 					ģ��ܽ�			��Ƭ���ܽ�
* 					SCL					�鿴DZB_OLED)IIC.h�ļ��ڵ�OLED_IIC_SCL_PIN�궨��  	C11
* 					SDA					�鿴DZB_OLED)IIC.h�ļ��ڵ�OLED_IIC_SDA_PIN�궨��    C12

			ʵ��ʹ��ֻ��Ҫ�޸�DZB_OLED)IIC.h�������º궨�弴�ɣ�
							#define OLED_IIC_SCL_PIN			C11
							#define OLED_IIC_SDA_PIN			C12
* 					------------------------------------
*All rights reserved
********************************************************************************************************************/

#include "headfile.h"

#define OLED_MODE_IIC // ʹ�õ�OLEDģʽ  1��OLED_MODE_IIC  2��OLED_MODE_SPI������̬�л�OLED��

#if defined OLED_MODE_IIC
#include "DZB_OLED_IIC.h"
#elif defined OLED_MODE_SPI
#include "SEEKFREE_OLED.h"
#error "OLED MODE CHOICE IS ERROR!"
#endif

int main(void)
{
	board_init(true);
	OLED_Init();
	oled_iic_p6x8str(24, 3, "MM32_OLED_TEST");

	while (1) {
	}
}
