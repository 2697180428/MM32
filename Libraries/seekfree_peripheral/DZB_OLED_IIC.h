#ifndef _DZB_OLED_IIC_h
#define _DZB_OLED_IIC_h

#include "common.h"

//----宏定义OLED引脚----	 
#define OLED_IIC_SCL_PIN			C11
#define OLED_IIC_SDA_PIN			C12

#define  OLED_IIC_SCL         gpio_dir(OLED_IIC_SCL_PIN,GPO);  //设置C11为输出模式
#define  OLED_IIC_SDA         gpio_dir(OLED_IIC_SDA_PIN,GPO);  //设置C12为输出模式


#define OLED_IIC_SCL0()			GPIO_PIN_RESET (OLED_IIC_SCL_PIN)								// IO口输出低电平
#define OLED_IIC_SCL1()			GPIO_PIN_SET (OLED_IIC_SCL_PIN)									// IO口输出高电平
#define OLED_IIC_SDA0()			GPIO_PIN_RESET (OLED_IIC_SDA_PIN)								// IO口输出低电平
#define OLED_IIC_SDA1()			GPIO_PIN_SET (OLED_IIC_SDA_PIN)									// IO口输出高电平


//定义显示方向
//0 横屏模式
//1 横屏模式  旋转180
#define OLED_DISPLAY_DIR		0

#if (0==OLED_DISPLAY_DIR || 1==OLED_DISPLAY_DIR)
#define X_WIDTH					128
#define Y_WIDTH					64

#else
#error "OLED_DISPLAY_DIR 定义错误"
#endif
                            
#define	Brightness				0x7f										// 设置OLED亮度 越大越亮 范围0-0XFF
#define XLevelL					0x00
#define XLevelH					0x10
#define XLevel					((XLevelH&0x0F)*16+XLevelL)
#define Max_Column				128
#define Max_Row					64




void	OLED_Init(void);    
void  oled_iic_fill(uint8 bmp_data);
void  oled_iic_p6x8str(uint8 x,uint8 y,const int8 ch[]);
void	oled_iic_p8x16str			(uint8 x,uint8 y,const int8 ch[]);
void	oled_iic_uint16				(uint8 x, uint8 y, uint16 num);
void	oled_iic_int16				(uint8 x, uint8 y, int16 num);
void	oled_iic_printf_int32		(uint16 x,uint16 y,int32 dat,uint8 num);
void	oled_iic_printf_float		(uint16 x,uint16 y,double dat,uint8 num,uint8 pointnum);
void	oled_iic_dis_bmp			(uint16 high, uint16 width, uint8 *p,uint8 value);
void	oled_iic_print_chinese		(uint8 x, uint8 y, uint8 size, const uint8 *p, uint8 len);
#endif
