// DSMCC.cpp: implementation of the DSMCC class.
//
//////////////////////////////////////////////////////////////////////

#include "DSMCCbase.h"

bool DSMCCbase::getushort(unsigned short& data, bool inc)
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
bool DSMCCbase::getbyte(unsigned char& byte, bool inc)
{

	return getdata(1,&byte,inc);
}

bool DSMCCbase::get12bits(unsigned short& data, bool inc)
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

bool DSMCCbase::getulong(unsigned long& data, bool inc)
{
	unsigned char buff[4];

	if (getdata(4,buff,inc))
	{
		data = (buff[0] << 24 ) + (buff[1] << 16) + (buff[2] << 8) + buff[3];
		return true;
	}
	else
		return false;

}
bool DSMCCbase::getdata(long length, unsigned char *buffer, bool inc)
{
	if (buffer == NULL)
		return false;

	// Yes let's get out of here
	if (!dsmccreader->getdata(buffer,length))
		return false;

	// increment the start data pointer
	if (!inc)
		if(!dsmccreader->finddata(false,-length))
			return false;

	return true;
}

bool DSMCCbase::finddata(bool fromstart, long len)
{
	return dsmccreader->finddata(fromstart,len);
}

void DSMCCbase::writeblock(component_ids ID, unsigned long len, writer* level)
{
	long count = len;
	unsigned char buffer[1024];

	while (len > 1024)
	{
		if(!getdata(1024,buffer,true))
			return;
		level->bindata(ID,buffer,1024);
		len -= 1024;
	}
	if (len)
	{
		if(!getdata(len,buffer,true))
			return;
		level->bindata(ID,buffer,len);
	}
}

