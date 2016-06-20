// DSMCCdata.cpp: implementation of the DSMCCdata class.
//
//////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "DSMCCdata.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool UNConfigMessage::Write(writer *level)
{
	return DSMCCdata::Write(level);
}


bool UNSessionMessage::Write(writer *level)
{
	return DSMCCdata::Write(level);
}

bool DownloadMessage::WritePrivData(int len, writer *level)
{
	writer *level1 = level->child();
	level->write(IDS_PDLEN, len);

	if (len)
		writeblock(IDS_PRIVDAT,len,level1);

	level->removechild(level1);
	return true;
}

bool DIRequest::Write(writer *level)
{
	writer *level2;
    unsigned short len;
	unsigned long ulong_data;
	unsigned short ushort_data;

	finddata(true,0);
	level2 = level->child();
	level->write(IDS_DIREQUEST);

	if (!getulong(ulong_data))
		return false;

	level2->write(IDS_LENGTH, ulong_data);

	if (!getushort(ushort_data))
		return false;

	level2->write(IDS_MAXBLOCK, ushort_data);

	compatibilitydesc(level2);

	if(!getushort(len))
		return false;

	level2->write(IDS_PDLEN, len);

	writeblock(IDS_PRIVDAT,len,level2);

	level->removechild(level2);

	return true;
}

bool DII::WriteModInfo(int len, writer *level)
{
	level->write(IDS_MODINFLEN,len);
	if (len)
		writeblock(IDS_MODINFO,len,level);
	return true;
}

bool DII::Write(writer *level)
{
	writer *level2,*level3;
	unsigned short i;
	unsigned long ulong_data;
	unsigned short ushort_data;
	unsigned char uchar_data;
	unsigned short numModules;

	if (!finddata(true,0))
        	return false;

	level2 = level->child();
	level->write(IDS_DII);

	if (!getulong(ulong_data))
		return false;
	level2->write(IDS_DLID, ulong_data);

	if (!getushort(ushort_data))
		return false;
	level2->write(IDS_BLOCKSIZE, ushort_data);

	if (!getbyte(uchar_data))
		return false;
	level2->write(IDS_WINDOWSIZE, uchar_data);

	if (!getbyte(uchar_data))
		return false;
	level2->write(IDS_ACKPER, uchar_data);

	if (!getulong(ulong_data))
		return false;
	level2->write(IDS_DLWIN, ulong_data);

	if (!getulong(ulong_data))
		return false;
	level2->write(IDS_DLSC, ulong_data);

	compatibilitydesc(level2);

	if(!getushort(numModules))
		return false;
	level2->write(IDS_NUMMOD, numModules);

	for (i = 0; i<numModules; i++)
	{
		unsigned short Module;
		unsigned char inflen;

		if(!getushort(Module))
			return false;

		level3 = level2->child();
		level2->write(IDS_MODID, Module);

		if (!getulong(ulong_data))
			return false;
		level3->write(IDS_SIZE, ulong_data);

		if (!getbyte(uchar_data))
			return false;
		level3->write(IDS_VER, uchar_data);

		if(!getbyte(inflen))
			return false;

		if (!WriteModInfo(inflen,level3))
			return false;
		level2->removechild(level3);

	}

	if(!getushort(ushort_data))
		return false;
	if (!WritePrivData(ushort_data,level2))
		return false;

	level->removechild(level2);
	return true;
}

bool DataModule::Write(writer *level)
{
    writer *level2;

    if (!finddata(true,0))
        return false;

	level2 = level->child();
	level->write(IDS_DDB);

	writeblock(IDS_DATA,Len(),level2);

	level->removechild(level2);
	return true;
}

bool DDRequest::Write(writer *level)
{
	writer *level2;
	unsigned short ushort_data;
	unsigned char uchar_data;

	finddata(true,0);
	level2 = level->child();
	level->write(IDS_DDREQMESS);
	if (!getushort(ushort_data))
		return false;
	level2->write(IDS_NUMMOD, ushort_data);

	if (!getushort(ushort_data))
		return false;
	level2->write(IDS_BLOCKNUM, ushort_data);

	if (!getbyte(uchar_data))
		return false;
	level2->write(IDS_REASON, uchar_data);

	level->removechild(level2);
	return true;
}
bool DLCancel::Write(writer *level)
{
    writer *level2;
	unsigned long ulong_data;
	unsigned short ushort_data;
	unsigned char uchar_data;

	finddata(true,0);
	level2 = level->child();
	level->write(IDS_DLCANCEL);

	if (!getulong(ulong_data))
		return false;
	level2->write(IDS_DLID, ulong_data);

	if (!getushort(ushort_data))
		return false;
	level2->write(IDS_NUMMOD, ushort_data);

	if (!getushort(ushort_data))
		return false;
	level2->write(IDS_BLOCKNUM, ushort_data);

	if (!getbyte(uchar_data))
		return false;
	level2->write(IDS_REASON, uchar_data);

	finddata(false,1);  // ignore reserved

	{
		unsigned short pdlen;
		if(!getushort(pdlen))
			return false;
		if (!WritePrivData(pdlen,level2))
			return false;
	}

	level->removechild(level2);
	return true;
}

bool DSI::Write(writer *level)
{
	writer *level2;
	unsigned short pdlen;

    if (!finddata(true,0))
        return false;

	level2 = level->child();
	level->write(IDS_DSI);

	writeblock(IDS_SERVID,20,level2);

	compatibilitydesc(level2);

	if(!getushort(pdlen))
		return false;

	if (!WritePrivData(pdlen,level2))
		return false;

	level->removechild(level2);
	return true;
}

bool ChanChangeMessage::Write(writer *level)
{
	return DSMCCdata::Write(level);
}

bool UNPassMessage::Write(writer *level)
{
	return DSMCCdata::Write(level);
}

bool DownloadMessage::CompSubDesc(writer *level)
{
	unsigned char type,len;
	writer *level1;

	if (!getbyte(type))
		return false;
			
	level1 = level->child();
	level->write(IDS_SUBDESCTYPE, type);
			
	if(!getbyte(len))
		return false;

	level1->write(IDS_LENGTH, len);

	if (len)
		writeblock(IDS_SUBDESCTYPE,len,level1);

	level->removechild(level1);
	return true;
}

void DownloadMessage::compatibilitydesc(writer *level)
{

    writer *level2,*level3;
	unsigned short count, len;
	unsigned char subcount;
    unsigned char uchar_data;
	unsigned short ushort_data;

	if(!getushort(len))
		return;

	if (len == 0) // no compatibility descriptor so bye bye
		return;

	level2 = level->child();
	level->write(IDS_COMPATDESC);

	level2->write(IDS_LENGTH, len);

	if(!getushort(count))
		return;

	level2->write(IDS_DESCCOUNT, count);

	while (count--)
	{
		if (!getbyte(uchar_data))
			return;
		level3 = level2->child();
		level2->write(IDS_DESCTYPE, uchar_data);

		if (!getbyte(uchar_data))
			return;
		level3->write(IDS_LENGTH, uchar_data);

		if (!getbyte(uchar_data))
			return;
		level3->write(IDS_SPECTYPE, uchar_data);

		writeblock(IDS_SPECDATA, 3, level3);

		if (!getushort(ushort_data))
			return;
		level3->write(IDS_MODEL, ushort_data);

		if (!getushort(ushort_data))
			return;
		level3->write(IDS_VER, ushort_data);
	
		if(!getbyte(subcount))
			return;

		level3->write(IDS_SUBDESCCOUNT, subcount);

		while(subcount--)
		{
			if (!CompSubDesc(level3))
				return;

		}
		level2->removechild(level3);
	}
	level->removechild(level2);

}

bool DSMCCdata::Write(writer *level)
{
    level->write((char*)"Unknown Table");
	return true;
}
