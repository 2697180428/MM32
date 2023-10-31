/*********************************************************************************************************************
*
* 本程序只供学习使用，未经作者许可，不得用于其它任何用途
* Copyright (c) 2022,DZB
* Author QQ :269718042
* 版本：V1.0
* 适合芯片：灵动 MM32SPIN27PS
* @Software	  MDK 5.24
* @date				2022-2-5
* @note
* 					接线定义：
* 					------------------------------------
* 					模块管脚			单片机管脚
* 					SCL					查看DZB_OLED)IIC.h文件内的OLED_IIC_SCL_PIN宏定义  	C11
* 					SDA					查看DZB_OLED)IIC.h文件内的OLED_IIC_SDA_PIN宏定义    C12

			实际使用只需要修改DZB_OLED)IIC.h里面以下宏定义即可：
							#define OLED_IIC_SCL_PIN			C11
							#define OLED_IIC_SDA_PIN			C12
* 					------------------------------------
*All rights reserved
********************************************************************************************************************/

#include "headfile.h"

#define OLED_MODE_IIC // 使用的OLED模式  1、OLED_MODE_IIC  2、OLED_MODE_SPI——动态切换OLED库

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
