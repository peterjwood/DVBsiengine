// CAT.cpp: implementation of the CAT class.
//
//////////////////////////////////////////////////////////////////////

#include "CAT.h"

bool CAT::Write(writer* parent)
{
	writer *level,*level2;
	unsigned short size = 0xFFFF;

	level = parent->child();
	parent->write(IDS_CAT);

	WriteGeneric(level);

	level2 = level->child();
	level->startlist(IDS_CAT);
	while(size)
	{
		level->listitem();
		size = DecodeDescriptor(level,0xFFFF);
		finddata(false,size);
	}
	level->endlist();

	level->removechild(level2);
	parent->removechild(level);
	return true;
}

