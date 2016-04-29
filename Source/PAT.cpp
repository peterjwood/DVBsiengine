// PAT.cpp: implementation of the PAT class.
//
//////////////////////////////////////////////////////////////////////

#include "PAT.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool PAT::Write(writer* parent)
{
	writer *level;
	unsigned short tmpshort;

	level = parent->write(IDS_PAT);

	WriteGeneric(level);

	level->write(IDS_TSID, TransportStreamID());

	finddata(true,0);

	while(currentpos < SectionPayloadLength())
	{
		if(!getushort(tmpshort))
			return false;

		level->write(IDS_PROGNUM, tmpshort);

		if (tmpshort)
		{
			if(!getushort(tmpshort))
				return false;
			level->write(IDS_PMTPID, (tmpshort & 0x1FFF));
		}
		else
		{
			if(!getushort(tmpshort))
				return false;
			level->write(IDS_NETPID, (tmpshort & 0x1FFF));
		}
	}

	return true;
}

unsigned short PAT::TransportStreamID()
{
	return ID();
}


bool PAT::getPIDs(bool *List, bool AddPAT, bool, bool , bool , bool , bool , bool )
{
	unsigned short tmpshort;
	bool retval = false;
	unsigned int count,value;

	if (!AddPAT)
		return false;

	finddata(true,0);


	while(currentpos < SectionPayloadLength())
	{
		finddata(false,2);
		if(!getushort(tmpshort))
			return false;

		if (!List[ (tmpshort & 0x1FFF)])
		{
			List[ (tmpshort & 0x1FFF)] = true;
			retval = true;
		}
	}

	return retval;
}
