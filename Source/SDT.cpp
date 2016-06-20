// SDT.cpp: implementation of the SDT class.
//
//////////////////////////////////////////////////////////////////////

#include "SDT.h"

bool SDT::Write(writer* parent)
{
	writer *level;
	unsigned short tmpshort;

	level = parent->child();
	parent->writetitle(IDS_SDT);

	if (TableType() == 0x42)
		level->write(IDS_ACTUAL,IDS_TRUE);
	else if (TableType() == 0x46)
		level->write(IDS_ACTUAL,IDS_FALSE);
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

	parent->removechild(level);
	parent->enditem();
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

	parent->startlist(IDS_SERVICES);
	while(currentpos < SectionPayloadLength())
	{

		level = parent->child();
		parent->listitem();
		if(!getushort(tmpshort))
			return false;
		level->write(IDS_SERVICEID, tmpshort);

		if(!getbyte(tmp))
			return false;

		if (tmp & 2)
		{
			level->write( IDS_EITSCHDATA);
		}
		if (tmp & 1)
		{
			level->write( IDS_EITPRESFOLL);
		}

		if(!getbyte(tmp,false))
			return false;
		
		switch ((tmp & 0xE0)>>5)
		{
		case 0:
			level->write(IDS_RUNSTAT,IDS_UNDEF);
			break;
		case 1:
			level->write(IDS_RUNSTAT,IDS_NOTRUN);
			break;
		case 2:
			level->write(IDS_RUNSTAT,IDS_SOON);
			break;
		case 3:
			level->write(IDS_RUNSTAT,IDS_PAUSING);
			break;
		case 4:
			level->write(IDS_RUNSTAT,IDS_RUN);
			break;
		default:
			level->write(IDS_RUNSTAT,IDS_RESERR);
			break;
		}

		if (tmp & 0x10)
		{
			level->write(IDS_CAACT,IDS_TRUE);
		}
		else
		{
			level->write(IDS_CAACT,IDS_FALSE);
		}

		if(!get12bits(looplen))
			return false;

		level->startlist(IDS_DESCRIPTORS);
		while (looplen)
		{
			level2 = level->child();
			level->listitem();
			unsigned short size = DecodeDescriptor(level2,looplen);
			if (!size)
				return false;
			looplen -= size;
			finddata(false,size);
			level->removechild(level2);
		}
		level->endlist();
		parent->removechild(level);
	}
	parent->endlist();

	return true;
}

