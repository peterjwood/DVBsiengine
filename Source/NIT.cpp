// NIT.cpp: implementation of the NIT class.
//
//////////////////////////////////////////////////////////////////////

#include "NIT.h"

bool NIT::Write(writer* parent)
{
	writer *level;

	level = parent->child();
	parent->writetitle(IDS_NIT);

	if (TableType() == 0x40)
		level->write(IDS_ACTUAL,IDS_TRUE);
	else if (TableType() == 0x41)
		level->write(IDS_ACTUAL,IDS_FALSE);
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


	parent->startlist(IDS_DESCRIPTORS);
	while (looplen)
	{
		level = parent->child();
		parent->listitem();
		unsigned short size = DecodeDescriptor(level,looplen);
		looplen -= size;
		finddata(false,size);
		parent->removechild(level);
	}
	parent->endlist();

}

bool NIT::TSLoop(writer* parent)
{
	writer *level,*level2;
	unsigned short TSlooplen, looplen, ushort_data;

	if(!get12bits(TSlooplen))
		return false;

	parent->startlist(IDS_TRANSSTREAM);
	while(TSlooplen)
	{

		level = parent->child();
		parent->listitem();
		if(!getushort(ushort_data))
			return false;
		level->write(IDS_TSID, ushort_data);

		TSlooplen-=2;

		if(!getushort(ushort_data))
			return false;
		level->write(IDS_ONID, ushort_data);

		TSlooplen-=2;

		if(!get12bits(looplen))
			return false;
		TSlooplen-=2;

		level->startlist(IDS_DESCRIPTORS);
		while (looplen)
		{
			level2 = level->child();
			level->listitem();
			unsigned short size = DecodeDescriptor(level2,looplen);
			looplen -= size;
			finddata(false,size);
			TSlooplen-= size;
			level->removechild(level2);
		}
		level->endlist();
		parent->removechild(level);
	}
	parent->endlist();

	return true;
}

