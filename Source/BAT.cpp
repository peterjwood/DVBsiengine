// BAT.cpp: implementation of the BAT class.
//
//////////////////////////////////////////////////////////////////////

#include "BAT.h"

bool BAT::Write(writer *parent)
{
	writer *level;

	level = parent->write(IDS_BATMAIN);
	
	level->write(IDS_ID, ID());
	 
	WriteGeneric(level);

	BouquetDescriptorLoop(level);

	TSLoop(level);

	return true;
}


void BAT::BouquetDescriptorLoop(writer *parent)
{
	writer *level;
	unsigned short looplen = 0;
	
	if(!get12bits(looplen))
		return;

	level = parent->write(IDS_BATLOOP);

	level->write(IDS_LENGTH,looplen);

	while (looplen)
	{
		unsigned short size = DecodeDescriptor(level,looplen);
		looplen -= size;
		finddata(false,size);
	}


}

bool BAT::TSLoop(writer *parent)
{
	writer *level;
	unsigned short TSlooplen = 0, looplen = 0;

	if (!get12bits(TSlooplen))
		return false;

	level = parent->write(IDS_TSLOOPLEN, TSlooplen);

	while(TSlooplen >= 6)
	{
		unsigned short data;

		if (!getushort(data))
			return false;
		level->write(IDS_TRANSSTREAM, data);

		TSlooplen-=2;

		if (!getushort(data))
			return false;

		level->write(IDS_ONID,data);

		TSlooplen-=2;

		if (!get12bits(looplen))
			break;
		TSlooplen-=2;

		while (looplen)
		{
			unsigned short size = DecodeDescriptor(level,looplen);
			looplen -= size;
			finddata(false,size);
			TSlooplen-= size;
		}
	}

	return true;
}

