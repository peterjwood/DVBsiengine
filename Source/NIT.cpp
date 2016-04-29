// NIT.cpp: implementation of the NIT class.
//
//////////////////////////////////////////////////////////////////////

#include "NIT.h"

bool NIT::Write(writer* parent)
{
	writer *level;

	level = parent->write(IDS_NIT);

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

	level = parent->write(IDS_NDL);

	while (looplen)
	{
		unsigned short size = DecodeDescriptor(level,looplen);
		looplen -= size;
		finddata(false,size);
	}


}

bool NIT::TSLoop(writer* parent)
{
	writer *level,*level2;
	unsigned short TSlooplen, looplen, ushort_data;

	if(!get12bits(TSlooplen))
		return false;

	level = parent->write(IDS_TRANSSTREAM);

	while(TSlooplen)
	{

		if(!getushort(ushort_data))
			return false;
		level2 = level->write(IDS_TSID, ushort_data);

		TSlooplen-=2;

		if(!getushort(ushort_data))
			return false;
		level2->write(IDS_ONID, ushort_data);

		TSlooplen-=2;

		if(!get12bits(looplen))
			return false;
		TSlooplen-=2;

		while (looplen)
		{
			unsigned short size = DecodeDescriptor(level2,looplen);
			looplen -= size;
			finddata(false,size);
			TSlooplen-= size;
		}
	}

	return true;
}

