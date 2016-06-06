// NIT.cpp: implementation of the NIT class.
//
//////////////////////////////////////////////////////////////////////

#include "NIT.h"

bool NIT::Write(writer* parent)
{
	writer *level;

	level = parent->child();
	parent->write(IDS_NIT);

	if (TableType() == 0x40)
		level->write(IDS_ACTUAL);
	else if (TableType() == 0x41)
		level->write(IDS_OTHER);
	else
		return false;

	WriteGeneric(level);

	level->write(IDS_NID, NetworkID());

	finddata(true,0);
	NetworkDescriptorLoop(level);

	TSLoop(level);
	parent->removechild(level);
	return true;

}

unsigned short NIT::NetworkID()
{
	return ID();

}

void NIT::NetworkDescriptorLoop(writer* parent)
{
	writer *level;
	unsigned short looplen;
	if(!get12bits(looplen))
		return;

	level = parent->child();
	parent->write(IDS_NDL);

	parent->startlist(IDS_DESCRIPTORS);
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

bool NIT::TSLoop(writer* parent)
{
	writer *level,*level2;
	unsigned short TSlooplen, looplen, ushort_data;

	if(!get12bits(TSlooplen))
		return false;

	level = parent->child();
	parent->write(IDS_TRANSSTREAM);

	parent->startlist(IDS_TRANSSTREAM);
	while(TSlooplen)
	{

		parent->listitem();
		if(!getushort(ushort_data))
			return false;
		level2 = level->child();
		level->write(IDS_TSID, ushort_data);

		TSlooplen-=2;

		if(!getushort(ushort_data))
			return false;
		level2->write(IDS_ONID, ushort_data);

		TSlooplen-=2;

		if(!get12bits(looplen))
			return false;
		TSlooplen-=2;

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

