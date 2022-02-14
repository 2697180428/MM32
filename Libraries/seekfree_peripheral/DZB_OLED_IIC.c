#include "zf_gpio.h"
#include "zf_systick.h"
#include "SEEKFREE_IIC.h"
#include "SEEKFREE_FONT.h"
#include "SEEKFREE_PRINTF.h"
#include "DZB_OLED_IIC.h"

//�ڲ����ݶ���
static uint16 simiic_delay_time=2;										// ģ��IIC����ʱʱ�䣬ICM�ȴ�����Ӧ����Ϊ100

void OLED_IIC_Init(void)
{
	 gpio_init(OLED_IIC_SCL_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
	 gpio_init(OLED_IIC_SDA_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
}

static void SDA_OUT(void)  //����SDAΪ���ģʽ  GPIO_Mode_Out_PP-�������
{
	 gpio_init(OLED_IIC_SDA_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
}

static void SDA_IN(void)  //����SDAΪ����ģʽ   GPI_PULL_UP������������
{
	 gpio_init(OLED_IIC_SDA_PIN, GPO, GPIO_HIGH, GPI_PULL_UP);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		ģ��IIC��ʼ�ź�
// @return		void
// Sample usage:
// @note		�ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������simiic_delay_time��ֵ
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
// @brief		ģ��IICֹͣ�ź�
// @return		void
// Sample usage:
// @note		�ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������simiic_delay_time��ֵ
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
// @brief		ģ��IIC�ȴ�Ӧ��
// @return		void
// Sample usage:
// @note		�ڲ����� �û�������� ���IICͨѶʧ�ܿ��Գ�������simiic_delay_time��ֵ
//   ����1--Ӧ�����
//   �Ż�0--Ӧ����ȷ  �����OLED�����Ժ��Ե��ⲿ��ACK�����ٳ�������ʱ�䣬�������mpu6050��˵������Ҫ�ⲿ��Ӧ�𣡣�����
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
// @brief		ģ��IIC �ֽڷ��� �������մ�Ӧ�� �����Ǵ�Ӧ��λ
// @param		c				����c(����������Ҳ���ǵ�ַ)
// @return		void
// Sample usage:
// @note		�ڲ����� �û�������� 
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
// @brief		OLEDд����
// @param		cmd				����
// @return		void
// @since		v1.2
// Sample usage:
// @note		�ڲ����� �û��������
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
// @brief		OLEDд����
// @param		cmd				����
// @return		void
// @since		v1.2
// Sample usage:
// @note		�ڲ����� �û��������
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
// @brief		ת��16����ascii��
// @param		hex				ascii��
// @param		Print			��ŵ�ַ
// @return		void
// @since		v1.0
// Sample usage:
// @note		�ڲ����� �û��������
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
// @brief		OLED_IIC��ʼ������
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void	OLED_Init(void)  
{
	  OLED_IIC_Init();        //��ʼ��
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
// @brief		OLED��������
// @param		bmp_data		�����ɫѡ��(0x00 or 0xff)
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
// @brief		OLED��ʾ�ַ���(6*8����)
// @param		x				x����������0-127
// @param		y				y����������0-7
// @param		ch[]			�ַ���
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
// @brief		OLED��ʾ�ַ���(8*16����)
// @param		x				x����������0-127
// @param		y				y����������0-7
// @param		ch[]			�ַ���
// @return		void
// @since		v1.0
// Sample usage:				���ע�⣬�������ַ��и�λ16�����yӦ����ÿ�ε���2
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
// @brief		OLED��ʾ�޷�����(6*8����)
// @param		x				x����������0-127
// @param		y				y����������0-7
// @param		num				�޷�����
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void oled_iic_uint16(uint8 x, uint8 y, uint16 num)
{
	int8 ch[7];

	oled_hexascii(num,ch);
	oled_iic_p6x8str(x, y, &ch[1]);												// ��ʾ����  6*8����
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED��ʾ�з�����(6*8����)
// @param		x				x����������0-127
// @param		y				y����������0-7
// @param		num				�з�����
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
	oled_iic_p6x8str(x, y, &ch[1]);												// ��ʾ����  6*8����
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED��ʾ32λ�з���(ȥ������������Ч��0)
// @param		x				x����������0-127
// @param		y				y����������0-7
// @param		dat				��Ҫ��ʾ�ı�������������uint32
// @param		num				��Ҫ��ʾ��λ�� ���10λ  ������������
// @return		void
// @since		v1.0
// Sample usage:				oled_printf_int32(0,0,x,5);//x����Ϊint32 uint16 int16 uint8 int8����
// Sample usage:				��������ʾһ�� ��-����   ������ʾһ���ո�
//-------------------------------------------------------------------------------------------------------------------
void oled_iic_printf_int32(uint16 x,uint16 y,int32 dat,uint8 num)
{
	int8    buff[34];
	uint8   length;

	if(10<num)      num = 10;

	num++;
	if(0>dat)
		length = zf_sprintf( &buff[0],"%d",dat);							// ����
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

	oled_iic_p6x8str(x, y, buff);												// ��ʾ����
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED��ʾ������(ȥ������������Ч��0)
// @param		x				x����������0-127
// @param		y				y����������0-7
// @param		dat				��Ҫ��ʾ�ı�������������float��double
// @param		num				����λ��ʾ����   ���10λ  
// @param		pointnum		С��λ��ʾ����   ���6λ
// @return		void
// @since		v1.0
// Sample usage:		oled_printf_float(0,0,x,2,3);//��ʾ������   ������ʾ2λ   С����ʾ��λ
// @note				�ر�ע�⵱����С��������ʾ��ֵ����д���ֵ��һ����ʱ��
// 						���������ڸ��������ȶ�ʧ���⵼�µģ��Ⲣ������ʾ���������⣬
// 						�й���������飬�����аٶ�ѧϰ   ���������ȶ�ʧ���⡣
// 						��������ʾһ�� ��-����   ������ʾһ���ո�
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
		length = zf_sprintf( &buff[0],"%f",dat);							// ����
	else
	{
		length = zf_sprintf( &buff[1],"%f",dat);
		length++;
	}
	point = length - 7;														// ����С����λ��
	start = point - num - 1;												// ������ʼλ
	end = point + pointnum + 1;												// �������λ
	while(0>start)															// ����λ����  ĩβӦ�����ո�
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

	oled_iic_p6x8str(x, y, buff);												// ��ʾ����
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		OLED��ʾͼ��
// @param		high			ͼ��߶�
// @param		width			ͼ����
// @param		*p				ͼ���ַ�����飩
// @param		value			��ֵ����ֵ
// @return		void
// @since		v1.0
// Sample usage:
// @Note		ʹ��Image2lcd V3.2���ȡģ   C��������   ˮƽɨ��   
// 				��ȸ߶���������   ��ɫ��ת  256ɫ
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
// @brief		������ʾ
// @param		x				������ 0-127
// @param		y				������ 0-7
// @param		size			ȡģ��ʱ�����õĺ��������С��Ҳ����һ������ռ�õĵ��󳤿�Ϊ���ٸ��㣬ȡģ��ʱ����Ҫ������һ���ġ�
// @param		*p				��Ҫ��ʾ�ĺ�������
// @param		len				��Ҫ��ʾ����λ
// @return		void
// @since		v1.0
// Sample usage:
// @Note		ʹ��PCtoLCD2002���ȡģ		���롢����ʽ��˳��		16*16
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

