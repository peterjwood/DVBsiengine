// EMC.cpp: implementation of the EMC class.
//
//////////////////////////////////////////////////////////////////////

#include "ECM.h"

bool ECM::Write(writer* parent)
{
	writer* level;

	level = parent->write(IDS_ECM, TableType());

	level->write(IDS_LENGTH, SectionLength());

	finddata(true,0);

	writeblock(IDS_PRIVDAT,SectionPayloadLength(),level);

	return true;

}

