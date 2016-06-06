// PAT.cpp: implementation of the PAT class.
//
//////////////////////////////////////////////////////////////////////

#include "PAT.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool PAT::Write(writer* parent)
{
	writer *level,*level2;
	unsigned short tmpshort;

	parent->writetitle(IDS_PAT);
	level = parent->child();

	WriteGeneric(level);

	level->write(IDS_TSID, TransportStreamID());

	finddata(true,0);

	
	level->startlist(IDS_PROGSTR);
	while(currentpos < SectionPayloadLength())
	{
		if(!getushort(tmpshort))
			return false;
		level2 = level->child();

		level->listitem();
		level2->write(IDS_PROGNUM, tmpshort);

		if (tmpshort)
		{
			if(!getushort(tmpshort))
				return false;
			level2->write(IDS_PMTPID, (tmpshort & 0x1FFF));
		}
		else
		{
			if(!getushort(tmpshort))
				return false;
			level2->write(IDS_NETPID, (tmpshort & 0x1FFF));
		}
		level->removechild(level2);
	}
	level->endlist();

	parent->removechild(level);
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
