// SISection.cpp: implementation of the SISection class.
//
//////////////////////////////////////////////////////////////////////

#include "RawSection.h"
#include "PESSection.h"
#include "SISection.h"

RawSection *RawSection::Allocate(RawSection *sect)
{
	if (sect == NULL)
		return NULL;

	if ((sect->sectiondata[0] == 0) && (sect->sectiondata[1] == 0) && (sect->sectiondata[2] == 1))
		return PESSect::Allocate(sect);

	return SISection::Allocate(sect);
}

RawSection* RawSection::Allocate(packet *p)
{
	if (p->isPES)
		return (RawSection*) new PESSect(p);
	return (RawSection*)new SISection(p);
}

RawSection* RawSection::Allocate(unsigned char *buffer,int len)
{
	return SISection::Allocate(buffer,len);
}
RawSection::RawSection(unsigned char *buffer,int len)
{
	int size;
	RawSection();
	hPID = 0;

	if (len < 4 )
		return;

	size = (((buffer[1] & 15) << 8) + buffer[2] ) + 7;

	if (size > len)
		return;

	sectiondata.insert(sectiondata.end(),buffer,buffer+size);

}

bool RawSection::Write(writer *level)
{
	writer *level1 = level->child();
	level->write(IDS_UNTABTYPE,sectiondata[0]);

	// go to the start of the buffer
	finddata(true, 0);

	level1->write(IDS_PID,hPID);;

	level->removechild(level1);
	return true;
}

bool RawSection::getushort(unsigned short& data, bool inc)
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
bool RawSection::getbyte(unsigned char& byte, bool inc)
{

	return getdata(1,&byte,inc);
}

bool RawSection::get12bits(unsigned short& data, bool inc)
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

bool RawSection::getulong(unsigned long& data, bool inc)
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
unsigned char *RawSection::getrawdata(unsigned long length, bool inc)
{
	// is the offset in the section?
	if (currentpos < SectionPayloadLength() )
	{
		// is the section big enough to hold all the data?
		if ( length + currentpos -1 < SectionPayloadLength())
		{
			short prespos = currentpos;
			if (inc)
				currentpos += length;

			// Yes let's get out of here
			return &SectionPayload()[prespos];
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}

	return NULL;

}
bool RawSection::getdata(unsigned long length, unsigned char *buffer, bool inc)
{
	if (buffer == NULL)
		return false;

	// is the offset in the section?
	if (currentpos < SectionPayloadLength() )
	{
		// is the current section big enough to hold all the data?
		if ( length + currentpos -1 < SectionPayloadLength())
		{
			// Yes let's get out of here
			memcpy(buffer,&SectionPayload()[currentpos], length);
		}
		else
		{
			//no return false
			return false;
		}
	}
	else
	{
		return false;
	}

	// increment the start data pointer
	if (inc)
		currentpos += length;

	return true;
}

bool RawSection::finddata(bool fromstart, long len)
{
	if (fromstart)
		currentpos = len;
	else
		currentpos += len;

	return true;
}

void RawSection::writeblock(component_ids ID, unsigned long len, writer* level)
{

	if (len > SectionPayloadLength() + 1 -currentpos)
		return;

	if (len)
	{
		unsigned char *data = getrawdata(len,true);
		level->bindata(ID,data,len);
	}
}

