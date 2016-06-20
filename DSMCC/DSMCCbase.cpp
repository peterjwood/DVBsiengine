// DSMCC.cpp: implementation of the DSMCC class.
//
//////////////////////////////////////////////////////////////////////

#include "DSMCCbase.h"

bool DSMCCbase::writeblock(component_ids ID, unsigned long len, writer* level)
{
	unsigned char buffer[1024];

	while (len > 1024)
	{
		if(!getdata(1024,buffer,true))
			return false;
		level->bindata(ID,buffer,1024);
		len -= 1024;
	}
	if (len)
	{
		if(!getdata(len,buffer,true))
			return false;
		level->bindata(ID,buffer,len);
	}
	return true;
}

