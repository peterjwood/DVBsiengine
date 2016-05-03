// reader.h: interface for the reader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(READER_H__INCLUDED_)
#define READER_H__INCLUDED_

class reader
{
public:
	reader(){};
	~reader(){};
    virtual unsigned long Len()=0;
    virtual bool getdata(long len, unsigned char *buffer, bool inc=true)=0;
    virtual bool finddata(bool fromstart, long len)=0;
    virtual unsigned long CurrentPos(void)=0;

	bool getulong(unsigned long& data, bool inc = true)
	{
		unsigned char buff[4];

		if (getdata(4, buff,inc))
		{
			data = (buff[0] << 24 ) + (buff[1] << 16) + (buff[2] << 8) + buff[3];
			return true;
		}
		else
			return false;

	};

    bool getbyte(unsigned char& data, bool inc = true){return getdata(1,&data,inc);}

	bool getushort(unsigned short& data, bool inc = true)
	{
		unsigned char buff[2];

		if (getdata(2,buff,inc))
		{
			data = (buff[0] << 8 ) + buff[1];
			return true;
		}
		else
			return false;
    }

	bool get12bits(unsigned short& data, bool inc = true)
	{

		unsigned char buff[2];

		if (getdata(2,buff,inc))
		{
			data = ( (buff[0] & 0x0F) << 8 ) + buff[1];
			return true;
		}
		else
			return false;

    }
};
#endif
