// BAT.cpp: implementation of the BAT class.
//
//////////////////////////////////////////////////////////////////////

#include "BAT.h"

bool BAT::Write(writer *parent)
{
	writer *level;

	level = parent->child();
	parent->write(IDS_BATMAIN);
	
	level->write(IDS_ID, ID());
	 
	WriteGeneric(level);

	BouquetDescriptorLoop(level);

	TSLoop(level);

	parent->removechild(level);
	return true;
}


void BAT::BouquetDescriptorLoop(writer *parent)
{
	writer *level;
	unsigned short looplen = 0;
	
	if(!get12bits(looplen))
		return;

	level = parent->child();
	parent->write(IDS_BATLOOP);

	level->write(IDS_LENGTH,looplen);

	parent->startlist(IDS_BATLOOP);
	while (looplen)
	{
		parent->listitem();
		unsigned short size = DecodeDescriptor(level,looplen);
		looplen -= size;
		finddata(false,size);
	}
	parent->endlist();

	parent->removechild(level);

}

bool BAT::TSLoop(writer *parent)
{
	writer *level;
	unsigned short TSlooplen = 0, looplen = 0;

	if (!get12bits(TSlooplen))
		return false;

	level = parent->child();
	parent->write(IDS_TSLOOPLEN, TSlooplen);

	parent->startlist(IDS_TRANSSTREAM);
	while(TSlooplen >= 6)
	{
		unsigned short data;
		writer *level2;

		parent->listitem();
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
		level2 = level->child();
		level->startlist(IDS_DESCRIPTORS);
		while (looplen)
		{
			level->listitem();
			unsigned short size = DecodeDescriptor(level2,looplen);
			looplen -= size;
			finddata(false,size);
			TSlooplen-= size;
		}
		level->endlist();
		level->removechild(level2);
	}
	parent->endlist();

	parent->removechild(level);
	return true;
}

