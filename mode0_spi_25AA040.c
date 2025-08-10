#include <reg51.h>

sbit CS   = P1^0;
sbit SCK  = P1^1;
sbit MOSI = P1^2;
sbit MISO = P1^3;

sbit EN = P3^0;
sbit RS = P3^1;

#define LCD P2

void delay_us(unsigned int x)
{
    while(x--);
}

void delay_ms(unsigned int x)
{
    unsigned int i,j;
    for(i=0;i<x;i++)
        for(j=0;j<1275;j++);
}

void lcd_cmd(unsigned char cmd)
{
    RS=0;
    LCD=cmd;
    EN=1;
    delay_us(10);
    EN=0;
    delay_ms(2);
}

void lcd_data(unsigned char dat)
{
    RS=1;
    LCD=dat;
    EN=1;
    delay_us(10);
    EN=0;
    delay_ms(2);
}

void lcd_init()
{
    lcd_cmd(0x38);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
    delay_ms(2);
}

void SPI_Start()
{
    CS=0;
    delay_us(5);
}

void SPI_Stop()
{
    CS=1;
    delay_us(5);
}

void SPI_Write_Byte(unsigned char d)
{
    unsigned char i;
    for(i=0;i<8;i++)
    {
        SCK=0;                       // mode 0, CPOL = 0 ,CPHA = 0
        MOSI=(d&0x80)?1:0;
        d<<=1;
        delay_us(2);
        SCK=1;
        delay_us(2);
    }
    SCK=0;                           //since mode 0, 0 is idle, we assign SCK back to idle state
}

unsigned char SPI_Read_Byte()
{
    unsigned char i,d=0;
    for(i=0;i<8;i++)
    {
        d<<=1;
        SCK=1;
        delay_us(2);
        if(MISO) d|=0x01;
        SCK=0;
        delay_us(2);
    }
    return d;
}

void EEPROM_Write_Enable()
{
    SPI_Start();
    SPI_Write_Byte(0x06);
    SPI_Stop();
}

void EEPROM_Wait_Ready()
{
    unsigned char s;
    do
    {
        SPI_Start();
        SPI_Write_Byte(0x05);
        s=SPI_Read_Byte();
        SPI_Stop();
    }while(s&0x01);
}

void EEPROM_Write_Byte(unsigned int addr,unsigned char d)
{
    unsigned char cmd=0x02|((addr>>8)<<3);
    EEPROM_Write_Enable();
    SPI_Start();
    SPI_Write_Byte(cmd);
    SPI_Write_Byte(addr&0xFF);
    SPI_Write_Byte(d);
    SPI_Stop();
    EEPROM_Wait_Ready();
}

unsigned char EEPROM_Read_Byte(unsigned int addr)
{
    unsigned char cmd=0x03|((addr>>8)<<3);
    unsigned char d;
    SPI_Start();
    SPI_Write_Byte(cmd);
    SPI_Write_Byte(addr&0xFF);
    d=SPI_Read_Byte();
    SPI_Stop();
    return d;
}

void main()
{
    unsigned char ch;
    lcd_init();
    EEPROM_Write_Byte(0x001,'A');
    ch=EEPROM_Read_Byte(0x001);
    lcd_data(ch);
    while(1);
}
