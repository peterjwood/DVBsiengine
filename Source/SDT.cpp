// SDT.cpp: implementation of the SDT class.
//
//////////////////////////////////////////////////////////////////////

#include "SDT.h"

bool SDT::Write(writer* parent)
{
	writer *level;
	unsigned short tmpshort;

	level = parent->write(IDS_SDT);

	if (TableType() == 0x42)
		level->write(IDS_ACTUAL);
	else if (TableType() == 0x46)
		level->write(IDS_OTHER);
	else
		return false;

	WriteGeneric(level);

	level->write(IDS_TSID, TransportStreamID());

	finddata(true,0);

	if(!getushort(tmpshort))
		return false;
	level->write(IDS_ONID, tmpshort);

	finddata(false,1);  // ignore the reserved byte

	ServiceLoop(level);

	return true;
}

unsigned short SDT::TransportStreamID()
{
	return ID();

}

bool SDT::ServiceLoop(writer* parent)
{
	writer *level,*level2;
	unsigned char tmp;
	unsigned short looplen,tmpshort;

	level = parent->write(IDS_SERVICES);

	while(currentpos < SectionPayloadLength())
	{

		if(!getushort(tmpshort))
			return false;
		level2 = level->write(IDS_SERVICEID, tmpshort);

		if(!getbyte(tmp))
			return false;

		if (tmp & 2)
		{
			level2->write( IDS_EITSCHDATA);
		}
		if (tmp & 1)
		{
			level2->write( IDS_EITPRESFOLL);
		}

		if(!getbyte(tmp,false))
			return false;
		
		switch ((tmp & 0xE0)>>5)
		{
		case 0:
			level2->write(IDS_UNDEF);
			break;
		case 1:
			level2->write(IDS_NOTRUN);
			break;
		case 2:
			level2->write(IDS_SOON);
			break;
		case 3:
			level2->write(IDS_PAUSING);
			break;
		case 4:
			level2->write(IDS_RUN);
			break;
		default:
			level2->write(IDS_RESERR);
			break;
		}

		if (tmp & 0x10)
		{
			level2->write(IDS_CAACT);
		}
		else
		{
			level2->write(IDS_CANACT);
		}

		if(!get12bits(looplen))
			return false;

		while (looplen)
		{
			unsigned short size = DecodeDescriptor(level2,looplen);
			if (!size)
				return false;
			looplen -= size;
			finddata(false,size);
		}
	}

	return true;
}

