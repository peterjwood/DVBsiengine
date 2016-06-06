// EMC.cpp: implementation of the EMC class.
//
//////////////////////////////////////////////////////////////////////

#include "ECM.h"

bool ECM::Write(writer* parent)
{
	writer* level;

	parent->write(IDS_ECM, TableType());
	level = parent->child();

	level->write(IDS_LENGTH, SectionLength());

	finddata(true,0);

	writeblock(IDS_PRIVDAT,SectionPayloadLength(),level);

	parent->removechild(level);
	return true;

}

