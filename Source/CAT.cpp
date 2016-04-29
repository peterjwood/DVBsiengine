// CAT.cpp: implementation of the CAT class.
//
//////////////////////////////////////////////////////////////////////

#include "CAT.h"

bool CAT::Write(writer* parent)
{
	writer* level;
	unsigned short size = 0xFFFF;

	level = parent->write(IDS_CAT);

	WriteGeneric(level);

	while(size)
	{
		size = DecodeDescriptor(level,0xFFFF);
		finddata(false,size);
	}

	return true;

}

