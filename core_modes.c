void SPI_Write_Byte_Universal(unsigned char d, unsigned char idleClock, unsigned char sampleEdge)
{
    unsigned char i;
    unsigned char shiftEdge = !sampleEdge;

    SCK = idleClock;
    for(i=0; i<8; i++)
    {
        if (shiftEdge == 0)
        {
            SCK = 0;                   // shift edge = falling
            MOSI = (d & 0x80) ? 1 : 0;
            d <<= 1;
            delay_us(2);
            SCK = 1;                   // sample edge = rising
            delay_us(2);
        }
        else
        {
            SCK = 1;                   // shift edge = rising
            MOSI = (d & 0x80) ? 1 : 0;
            d <<= 1;
            delay_us(2);
            SCK = 0;                   // sample edge = falling
            delay_us(2);
        }
    }
    SCK = idleClock;                  // return to idle
}

unsigned char SPI_Read_Byte_Universal(unsigned char idleClock, unsigned char sampleEdge)
{
    unsigned char i, d = 0;
    unsigned char shiftEdge = !sampleEdge;

    SCK = idleClock;
    for(i=0; i<8; i++)
    {
        if (sampleEdge == 0)
        {
            SCK = 1;                   // sample edge = falling
            delay_us(2);
            d <<= 1;
            if(MISO) d |= 0x01;
            SCK = 0;                   // shift edge = rising
            delay_us(2);
        }
        else
        {
            SCK = 0;                   // sample edge = rising
            delay_us(2);
            d <<= 1;
            if(MISO) d |= 0x01;
            SCK = 1;                   // shift edge = falling
            delay_us(2);
        }
    }
    SCK = idleClock;
    return d;
}
