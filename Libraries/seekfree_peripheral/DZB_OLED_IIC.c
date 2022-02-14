#include "zf_gpio.h"
#include "zf_systick.h"
#include "SEEKFREE_IIC.h"
#include "SEEKFREE_FONT.h"
#include "SEEKFREE_PRINTF.h"
#include "DZB_OLED_IIC.h"

//内部数据定义
static uint16 simiic_delay_time=2;										// 模拟IIC的延时时间，ICM等传感器应设置为100

void OLED_IIC_Init(void)
{
	 gpio_init(OLED_IIC_SCL_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
	 gpio_init(OLED_IIC_SDA_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
}

static void SDA_OUT(void)  //设置SDA为输出模式  GPIO_Mode_Out_PP-推挽输出
{
	 gpio_init(OLED_IIC_SDA_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
}

static void SDA_IN(void)  //设置SDA为输入模式   GPI_PULL_UP——上拉输入
{
	 gpio_init(OLED_IIC_SDA_PIN, GPO, GPIO_HIGH, GPI_PULL_UP);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC起始信号
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void IIC_Start(void)
{
    SDA_OUT();
    OLED_IIC_SDA1();
    OLED_IIC_SCL1();
    systick_delay_us(simiic_delay_time);
    OLED_IIC_SDA0();
    systick_delay_us(simiic_delay_time);
    OLED_IIC_SCL0();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC停止信号
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
static void IIC_Stop(void)
{
    OLED_IIC_SCL1();
    OLED_IIC_SDA0();
    systick_delay_us(simiic_delay_time);
    OLED_IIC_SDA1();
    systick_delay_us(simiic_delay_time);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC等待应答
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 如果IIC通讯失败可以尝试增加simiic_delay_time的值
//   返回1--应答出错
//   放回0--应答正确  针对与OLED，可以忽略掉这部分ACK，减少程序运行时间，但针对与mpu6050来说，则需要这部分应答！！！！
//-------------------------------------------------------------------------------------------------------------------
static u8 IIC_Wait_Ask(void)
{
    	OLED_IIC_SCL0();
	    SDA_IN();
	    systick_delay_us(simiic_delay_time);
			OLED_IIC_SCL1();
			systick_delay_us(simiic_delay_time);
			SDA_OUT();
	    OLED_IIC_SCL0();
		  systick_delay_us(simiic_delay_time);
	    return 0;	
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		模拟IIC 字节发送 送完后接收从应答 不考虑从应答位
// @param		c				发送c(可以是数据也可是地址)
// @return		void
// Sample usage:
// @note		内部调用 用户无需关心 
//-------------------------------------------------------------------------------------------------------------------
static void IIC_WriteByte(u8 sand_data)
{
    u8 i;
    SDA_OUT();
    for(i=0;i<8;i++)
    {
        OLED_IIC_SCL0();
        systick_delay_us(simiic_delay_time);
        if(sand_data & 0x80)    
            OLED_IIC_SDA1();
        else
            OLED_IIC_SDA0();
        OLED_IIC_SCL1();
        systick_delay_us(simiic_delay_time);
        OLED_IIC_SCL0();
        sand_data<<=1;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED写命令
// @param		cmd				命令
// @return		void
// @since		v1.2
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void OLED_WriteCmd(u8 command)
{
    IIC_Start();
    IIC_WriteByte(0x78);
    IIC_Wait_Ask();
    IIC_WriteByte(0x00);
    IIC_Wait_Ask();
    IIC_WriteByte(command);
    IIC_Wait_Ask();
    IIC_Stop();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED写命令
// @param		cmd				命令
// @return		void
// @since		v1.2
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void OLED_WriteData(u8 data)
{
    IIC_Start();
    IIC_WriteByte(0x78);
    IIC_Wait_Ask();
    IIC_WriteByte(0x40);
    IIC_Wait_Ask();
    IIC_WriteByte(data);
    IIC_Wait_Ask();
    IIC_Stop();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		转换16进制ascii码
// @param		hex				ascii码
// @param		Print			存放地址
// @return		void
// @since		v1.0
// Sample usage:
// @note		内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void oled_hexascii(uint16 hex,int8 * Print)
{
	uint8 hexcheck ;
	uint8 TEMP ;
	TEMP = 6 ;
	Print[TEMP ]='\0';
	while(TEMP)
	{
		TEMP -- ;
		hexcheck  =  hex%10 ;
		hex   /=10 ;
		Print[TEMP ]  = hexcheck + 0x30 ;
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED_IIC初始化函数
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void	OLED_Init(void)  
{
	  OLED_IIC_Init();        //初始化
    systick_delay_ms(100); 

    OLED_WriteCmd(0xAE); //display off
    OLED_WriteCmd(0x20); //Set Memory Addressing Mode    
    OLED_WriteCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    OLED_WriteCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    OLED_WriteCmd(0xc8); //Set COM Output Scan Direction
    OLED_WriteCmd(0x00); //---set low column address
    OLED_WriteCmd(0x10); //---set high column address
    OLED_WriteCmd(0x40); //--set start line address
    OLED_WriteCmd(0x81); //--set contrast control register
    OLED_WriteCmd(0xff); //???? 0x00~0xff
    OLED_WriteCmd(0xa1); //--set segment re-map 0 to 127
    OLED_WriteCmd(0xa6); //--set normal display
    OLED_WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
    OLED_WriteCmd(0x3F); //
    OLED_WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    OLED_WriteCmd(0xd3); //-set display offset
    OLED_WriteCmd(0x00); //-not offset
    OLED_WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    OLED_WriteCmd(0xf0); //--set divide ratio
    OLED_WriteCmd(0xd9); //--set pre-charge period
    OLED_WriteCmd(0x22); //
    OLED_WriteCmd(0xda); //--set com pins hardware configuration
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xdb); //--set vcomh
    OLED_WriteCmd(0x20); //0x20,0.77xVcc
    OLED_WriteCmd(0x8d); //--set DC-DC enable
    OLED_WriteCmd(0x14); //
    OLED_WriteCmd(0xaf); //--turn on oled panel
		oled_iic_fill(0x00);
}

void OLED_ON(void)
{
    OLED_WriteCmd(0X8D);
    OLED_WriteCmd(0X14); 
    OLED_WriteCmd(0XAF); 
}

void OLED_SetPos(unsigned char x, unsigned char y)
{ 
		OLED_WriteCmd(0xb0+y);
		OLED_WriteCmd(((x&0xf0)>>4)|0x10);
		OLED_WriteCmd((x&0x0f)|0x01);
}

static void OLED_Fill(unsigned char fill_Data)
{
    unsigned char m,n;
    for(m=0;m<8;m++)
    {
        OLED_WriteCmd(0xb0+m);       //page0-page1
        OLED_WriteCmd(0x00);     //low column start address
        OLED_WriteCmd(0x10);     //high column start address
        for(n=0;n<128;n++)
            {
                OLED_WriteData(fill_Data);
            }
    }
}


void OLED_CLS(void)
{
    OLED_Fill(0xff);
}


//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED清屏函数
// @param		bmp_data		填充颜色选着(0x00 or 0xff)
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void oled_iic_fill(uint8 bmp_data)
{
	uint8 y,x;

	for(y=0;y<8;y++)
	{
		OLED_WriteCmd(0xb0+y);
		OLED_WriteCmd(0x01);
		OLED_WriteCmd(0x10);
		for(x=0;x<X_WIDTH;x++)	OLED_WriteData(bmp_data); 
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED显示字符串(6*8字体)
// @param		x				x轴坐标设置0-127
// @param		y				y轴坐标设置0-7
// @param		ch[]			字符串
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void oled_iic_p6x8str(uint8 x,uint8 y,const int8 ch[])
{
	uint8 c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>126)
		{
			x=0;
			y++;
		}
		OLED_SetPos(x,y);
		for(i=0;i<6;i++)
			OLED_WriteData(oled_6x8[c][i]);
		x+=6;
		j++;
	}
}



//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED显示字符串(8*16字体)
// @param		x				x轴坐标设置0-127
// @param		y				y轴坐标设置0-7
// @param		ch[]			字符串
// @return		void
// @since		v1.0
// Sample usage:				务必注意，本函数字符列高位16，因此y应该是每次递增2
//-------------------------------------------------------------------------------------------------------------------
void oled_iic_p8x16str(uint8 x,uint8 y,const int8 ch[])
{
	uint8 c=0,i=0,j=0;

	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120)
		{
			x=0;
			y++;
		}
		OLED_SetPos(x,y);
		for(i=0;i<8;i++)
			OLED_WriteData(oled_8x16[c*16+i]);

		OLED_SetPos(x,y+1);
		for(i=0;i<8;i++)
			OLED_WriteData(oled_8x16[c*16+i+8]);
		x+=8;
		j++;
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED显示无符号数(6*8字体)
// @param		x				x轴坐标设置0-127
// @param		y				y轴坐标设置0-7
// @param		num				无符号数
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void oled_iic_uint16(uint8 x, uint8 y, uint16 num)
{
	int8 ch[7];

	oled_hexascii(num,ch);
	oled_iic_p6x8str(x, y, &ch[1]);												// 显示数字  6*8字体
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED显示有符号数(6*8字体)
// @param		x				x轴坐标设置0-127
// @param		y				y轴坐标设置0-7
// @param		num				有符号数
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void oled_iic_int16(uint8 x, uint8 y, int16 num)
{
	int8 ch[7];
	if(num<0)
	{
		num = -num;
		oled_iic_p6x8str(x, y, "-");
	}
	else
		oled_iic_p6x8str(x, y, " ");
	x+=6;       

	oled_hexascii(num,ch);
	oled_iic_p6x8str(x, y, &ch[1]);												// 显示数字  6*8字体
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED显示32位有符号(去除整数部分无效的0)
// @param		x				x轴坐标设置0-127
// @param		y				y轴坐标设置0-7
// @param		dat				需要显示的变量，数据类型uint32
// @param		num				需要显示的位数 最高10位  不包含正负号
// @return		void
// @since		v1.0
// Sample usage:				oled_printf_int32(0,0,x,5);//x可以为int32 uint16 int16 uint8 int8类型
// Sample usage:				负数会显示一个 ‘-’号   正数显示一个空格
//-------------------------------------------------------------------------------------------------------------------
void oled_iic_printf_int32(uint16 x,uint16 y,int32 dat,uint8 num)
{
	int8    buff[34];
	uint8   length;

	if(10<num)      num = 10;

	num++;
	if(0>dat)
		length = zf_sprintf( &buff[0],"%d",dat);							// 负数
	else
	{
		buff[0] = ' ';
		length = zf_sprintf( &buff[1],"%d",dat);
		length++;
	}
	while(length < num)
	{
		buff[length] = ' ';
		length++;
	}
	buff[num] = '\0';

	oled_iic_p6x8str(x, y, buff);												// 显示数字
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED显示浮点数(去除整数部分无效的0)
// @param		x				x轴坐标设置0-127
// @param		y				y轴坐标设置0-7
// @param		dat				需要显示的变量，数据类型float或double
// @param		num				整数位显示长度   最高10位  
// @param		pointnum		小数位显示长度   最高6位
// @return		void
// @since		v1.0
// Sample usage:		oled_printf_float(0,0,x,2,3);//显示浮点数   整数显示2位   小数显示三位
// @note				特别注意当发现小数部分显示的值与你写入的值不一样的时候，
// 						可能是由于浮点数精度丢失问题导致的，这并不是显示函数的问题，
// 						有关问题的详情，请自行百度学习   浮点数精度丢失问题。
// 						负数会显示一个 ‘-’号   正数显示一个空格
//-------------------------------------------------------------------------------------------------------------------
void oled_iic_printf_float(uint16 x,uint16 y,double dat,uint8 num,uint8 pointnum)
{
	uint8	length;
	int8	buff[34];
	int8	start,end,point;

	if(6<pointnum)
		pointnum = 6;
	if(10<num)
		num = 10;

	if(0>dat)
		length = zf_sprintf( &buff[0],"%f",dat);							// 负数
	else
	{
		length = zf_sprintf( &buff[1],"%f",dat);
		length++;
	}
	point = length - 7;														// 计算小数点位置
	start = point - num - 1;												// 计算起始位
	end = point + pointnum + 1;												// 计算结束位
	while(0>start)															// 整数位不够  末尾应该填充空格
	{
		buff[end] = ' ';
		end++;
		start++;
	}

	if(0>dat)
		buff[start] = '-';
	else
		buff[start] = ' ';
	buff[end] = '\0';

	oled_iic_p6x8str(x, y, buff);												// 显示数字
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED显示图像
// @param		high			图像高度
// @param		width			图像宽度
// @param		*p				图像地址（数组）
// @param		value			二值化阀值
// @return		void
// @since		v1.0
// Sample usage:
// @Note		使用Image2lcd V3.2软件取模   C语言数组   水平扫描   
// 				宽度高度自行设置   颜色反转  256色
//-------------------------------------------------------------------------------------------------------------------
void oled_iic_dis_bmp(uint16 high, uint16 width, uint8 *p,uint8 value)
{
	int16 i,j;
	int16 temp,temp1;
	uint8 dat;


	temp1 = high%8;
	if(temp1 == 0)
		temp = high/8;
	else
		temp = high/8+1;

	for(i=0; i<temp; i++)
	{
		OLED_SetPos(0,i);
		for(j=0; j<width; j++)
		{
			dat = 0;
			if( i<(temp-1) || !temp1 || temp1>=1)dat |= (*(p+i*8*width+j+width*0) > value? 1: 0)<<0;
			if( i<(temp-1) || !temp1 || temp1>=2)dat |= (*(p+i*8*width+j+width*1) > value? 1: 0)<<1;
			if( i<(temp-1) || !temp1 || temp1>=3)dat |= (*(p+i*8*width+j+width*2) > value? 1: 0)<<2;
			if( i<(temp-1) || !temp1 || temp1>=4)dat |= (*(p+i*8*width+j+width*3) > value? 1: 0)<<3;
			if( i<(temp-1) || !temp1 || temp1>=5)dat |= (*(p+i*8*width+j+width*4) > value? 1: 0)<<4;
			if( i<(temp-1) || !temp1 || temp1>=6)dat |= (*(p+i*8*width+j+width*5) > value? 1: 0)<<5;
			if( i<(temp-1) || !temp1 || temp1>=7)dat |= (*(p+i*8*width+j+width*6) > value? 1: 0)<<6;
			if( i<(temp-1) || !temp1 || temp1>=8)dat |= (*(p+i*8*width+j+width*7) > value? 1: 0)<<7;

			OLED_WriteData(dat);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		汉字显示
// @param		x				横坐标 0-127
// @param		y				纵坐标 0-7
// @param		size			取模的时候设置的汉字字体大小，也就是一个汉字占用的点阵长宽为多少个点，取模的时候需要长宽是一样的。
// @param		*p				需要显示的汉字数组
// @param		len				需要显示多少位
// @return		void
// @since		v1.0
// Sample usage:
// @Note		使用PCtoLCD2002软件取模		阴码、逐行式、顺向		16*16
//-------------------------------------------------------------------------------------------------------------------
void oled_iic_print_chinese(uint8 x, uint8 y, uint8 size, const uint8 *p, uint8 len)
{
	int16 i,j,k;

	for(i=0; i<len; i++)
	{
		for(j=0; j<(size/8); j++)
		{
			OLED_SetPos(x+i*size,y+j);
			for(k=0; k<16; k++)
			{
				OLED_WriteData(*p);
				p++;
			}
		}
	}
}

