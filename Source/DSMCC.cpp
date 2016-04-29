// DSMCC.cpp: implementation of the DSMCC class.
//
//////////////////////////////////////////////////////////////////////

#include "DSMCC.h"
#include "writer.h"
#include "zlib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RawSection *DSMCC::Allocate(RawSection *sect=NULL)
{
	unsigned char* data;
	DSMCC *LocalSect=NULL;

	if (sect != NULL)
	{
		data = sect->SectionPayload();
		switch (data[1])
		{
		case 1:
			LocalSect = (DSMCC*)new UNConfigMessage(sect);
		break;
		case 2:
			LocalSect = (DSMCC*)new UNSessionMessage(sect);
		break;
		case 3:
			switch ((data[2] <<8 ) + data[3])
			{
			case 0x1001:
				LocalSect = (DSMCC*)new DIRequest(sect);
			break;
			case 0x1002:
				LocalSect = (DSMCC*)new DII(sect);
			break;
			case 0x1003:
				LocalSect = (DSMCC*)new DDB(sect);
			break;
			case 0x1004:
				LocalSect = (DSMCC*)new DDRequest(sect);
			break;
			case 0x1005:
				LocalSect = (DSMCC*)new DLCancel(sect);
			break;
			case 0x1006:
				LocalSect = (DSMCC*)new DSI(sect);
			break;
			default:
				LocalSect = (DSMCC*)new DownloadMessage(sect);
				break;
			}
		break;
		case 4:
			LocalSect = (DSMCC*)new ChanChangeMessage(sect);
		break;
		case 5:
			LocalSect = (DSMCC*) new UNPassMessage(sect);
		break;
		default:
			LocalSect =  new DSMCC(sect);
		break;
		}

	}
	else
		LocalSect =  new DSMCC(sect);

	return (RawSection*) LocalSect;
}

bool UNConfigMessage::Write(writer *level)
{
	return DSMCC::Write(level);
}


bool UNSessionMessage::Write(writer *level)
{
	return DSMCC::Write(level);
}

bool DownloadMessage::WritePrivData(int len, writer *level)
{
	writer *level1 = level->write(IDS_PDLEN, len);

	if (len)
		writeblock(IDS_PRIVDAT,len,level1);

	return true;
}



bool DIRequest::Write(writer *level)
{
	writer *level2;
	unsigned short len,i;
	unsigned long ulong_data;
	unsigned short ushort_data;

	level->write(IDS_DSMCCTAB, TableType());

	WriteGeneric(level);

	finddata(true,0);
	if (!WriteHeader(level))
		return false;

	level2 = level->write(IDS_DIREQUEST);

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
	unsigned short len,i;
	unsigned long ulong_data;
	unsigned short ushort_data;
	unsigned char uchar_data;
	unsigned short numModules;

	level->write(IDS_DSMCCTAB, TableType());

	WriteGeneric(level);

	finddata(true,0);
	if (!WriteHeader(level))
		return false;


	level2 = level->write(IDS_DII);

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

		level3 = level2->write(IDS_MODID, Module);

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

	}

	if(!getushort(ushort_data))
		return false;
	if (!WritePrivData(ushort_data,level2))
		return false;


	return true;
}

bool DDB::Write(writer *level)
{
	writer *level2,*level3;
	unsigned short len,i;
	unsigned long ulong_data;
	unsigned short ushort_data;
	unsigned char uchar_data;

	level->write(IDS_DSMCCTAB, TableType());

	WriteGeneric(level);

	finddata(true,0);
	if (!WriteHeader(level))
		return false;

	level2 = level->write(IDS_DDB);

	if(!getushort(ushort_data))
		return false;

	level2->write(IDS_MODID, ushort_data);

	if (!getbyte(uchar_data))
		return false;
	level2->write(IDS_VER, uchar_data);

	finddata(false,1); // ignore reserved

	if (!getushort(ushort_data))
		return false;
	level2->write(IDS_BLOCKNUM, ushort_data);

	writeblock(IDS_DATA,SectionPayloadLength() - currentpos,level2);

	return true;
}

bool DDRequest::Write(writer *level)
{
	writer *level2;
	unsigned long ulong_data;
	unsigned short ushort_data;
	unsigned char uchar_data;

	level->write(IDS_DSMCCTAB, TableType());

	WriteGeneric(level);

	finddata(true,0);
	if (!WriteHeader(level))
		return false;

	level2 = level->write(IDS_DDREQMESS);
	if (!getushort(ushort_data))
		return false;
	level2->write(IDS_NUMMOD, ushort_data);

	if (!getushort(ushort_data))
		return false;
	level2->write(IDS_BLOCKNUM, ushort_data);

	if (!getbyte(uchar_data))
		return false;
	level2->write(IDS_REASON, uchar_data);

	return true;
}
bool DLCancel::Write(writer *level)
{
	writer *level2,*level3;
	unsigned short len,i;
	unsigned long ulong_data;
	unsigned short ushort_data;
	unsigned char uchar_data;

	level->write(IDS_DSMCCTAB, TableType());

	WriteGeneric(level);

	finddata(true,0);
	if (!WriteHeader(level))
		return false;

	level2 = level->write(IDS_DLCANCEL);

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

	return true;
}

bool DSI::Write(writer *level)
{
	writer *level2;
	unsigned short pdlen;
	unsigned char *checkdata;

	level->write(IDS_DSMCCTAB, TableType());

	WriteGeneric(level);


	finddata(true,0);
	if (!WriteHeader(level))
		return false;

	level2 = level->write(IDS_DSI);

	writeblock(IDS_SERVID,20,level2);

	compatibilitydesc(level2);

	if(!getushort(pdlen))
		return false;

	if (!WritePrivData(pdlen,level2))
		return false;
	return true;
}

bool ChanChangeMessage::Write(writer *level)
{
	return DSMCC::Write(level);
}

bool UNPassMessage::Write(writer *level)
{
	return DSMCC::Write(level);
}

bool DownloadMessage::CompSubDesc(writer *level)
{
	unsigned char type,len;
	writer *level1;

	if (!getbyte(type))
		return false;
			
	level1 = level->write(IDS_SUBDESCTYPE, type);
			
	if(!getbyte(len))
		return false;

	level1->write(IDS_LENGTH, len);

	if (len)
		writeblock(IDS_SUBDESCTYPE,len,level1);

	return true;
}

void DownloadMessage::compatibilitydesc(writer *level)
{

	writer *level2,*level3, *level4;
	unsigned short count, len;
	unsigned char subcount;
	unsigned char uchar_data,tempchar,tempchar1;
	unsigned short ushort_data;

	if(!getushort(len))
		return;

	if (len == 0) // no compatibility descriptor so bye bye
		return;

	level2 = level->write(IDS_COMPATDESC);

	level2->write(IDS_LENGTH, len);

	if(!getushort(count))
		return;

	level2->write(IDS_DESCCOUNT, count);

	while (count--)
	{
		if (!getbyte(uchar_data))
			return;
		level3 = level2->write(IDS_DESCTYPE, uchar_data);

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
	}

}
bool DSMCC::WriteGeneric(writer *parent)
{
	parent->write(IDS_PID,hPID);

	parent->write(IDS_VER, TableVersion());
	parent->write(IDS_TIDEXT,ID());
	parent->write(IDS_LASTSEC,LastSectionNumber());

	return true;
}

bool DSMCC::WriteHeader(writer *level)
{
	unsigned char adaptlen,uchar_data;
	unsigned short messagelen, ushort_data;
	unsigned long ulong_data;

	level->write(IDS_SECT,SectionNumber());

	if(!getbyte(uchar_data))
		return false;

	level->write(IDS_PROTDESC, uchar_data);

	if(!getbyte(uchar_data))
		return false;

	level->write(IDS_TYPE,uchar_data);

	if(!getushort(ushort_data))
		return false;

	level->write(IDS_MESSID,ushort_data);

	if(!getulong(ulong_data))
		return false;
		
	level->write(IDS_TRANSID,ulong_data);

	// miss reserved byte
	finddata(false,1);

	if(!getbyte(adaptlen))
		return false;
	level->write(IDS_ADAPTLEN,adaptlen);
		
	if(!getushort(messagelen))
		return false;
	level->write(IDS_MESSLEN,messagelen);

	if (adaptlen)
	{
		writer *adaptlevel;

		adaptlevel = level->write(IDS_ADAPT);
			
		if(!getbyte(uchar_data))
			return false;
		adaptlevel->write(IDS_TYPE, uchar_data);

		writeblock(IDS_ADAPT,adaptlen,adaptlevel);
	}
	return true;
}

bool DSMCC::Write(writer *level)
{
	writer *level2;

	level2 = level->write(IDS_DSMCCTAB, TableType());

	WriteGeneric(level);

	finddata(true,0);
	if (!WriteHeader(level2))
		return false;

	return true;
}


