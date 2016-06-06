// DSMCCdatasection.cpp: implementation of the DSMCCdatasection class.
//
//////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "DSMCCsection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DSMCCdatasection *DSMCCdatasection::Allocate(reader *r)
{
	DSMCCdatasection *LocalD=NULL;
	unsigned char data[4];


	if (r == NULL)
		return LocalD;

	r->finddata(true,0);
	r->getdata(4,data);
	r->finddata(true,0);

	if ((r->Len() > 12) && (data[0] == 0x11))
	{
		switch (data[1])
		{
		case 1:
                        LocalD = (DSMCCdatasection*)new UNConfigMessageSect(r);
		break;
		case 2:
                        LocalD = (DSMCCdatasection*)new UNSessionMessageSect(r);
		break;
		case 3:
			switch ((data[2] <<8 ) + data[3])
			{
			case 0x1001:
                                LocalD = (DSMCCdatasection*)new DIRequestSect(r);
			break;
			case 0x1002:
                                LocalD = (DSMCCdatasection*)new DIISect(r);
			break;
			case 0x1003:
                                LocalD = (DSMCCdatasection*)new DDBSect(r);
			break;
			case 0x1004:
                                LocalD = (DSMCCdatasection*)new DDRequestSect(r);
			break;
			case 0x1005:
                                LocalD = (DSMCCdatasection*)new DLCancelSect(r);
			break;
			case 0x1006:
                                LocalD = (DSMCCdatasection*)new DSISect(r);
			break;
			default:
                                LocalD = (DSMCCdatasection*)new DownloadMessageSect(r);
				break;
			}
		break;
		case 4:
                        LocalD = (DSMCCdatasection*)new ChanChangeMessageSect(r);
		break;
		case 5:
                        LocalD = (DSMCCdatasection*) new UNPassMessageSect(r);
		break;
		default:
			LocalD =  new DSMCCdatasection(r);
		break;
		}
		if (r->Len() < LocalD->Len())
		{
			delete LocalD;
			LocalD= NULL;
		}
			

	}
	else
		LocalD =  NULL;

	return LocalD;
}

DSMCCdatasection::DSMCCdatasection(reader *r)
{
	currentpos = 0;
	dsmccdata = r;
}

bool UNConfigMessageSect::Write(writer *level)
{
	return DSMCCdatasection::Write(level);
}


bool UNSessionMessageSect::Write(writer *level)
{
	return DSMCCdatasection::Write(level);
}

bool DownloadMessageSect::WritePrivData(int len, writer *level)
{
	writer *level1 = level->child();
	level->write(IDS_PDLEN, len);

	if (len)
		writeblock(IDS_PRIVDAT,len,level1);

	level->removechild(level1);
	return true;
}



bool DIRequestSect::Write(writer *level)
{
	writer *level2;
    unsigned short len;
	unsigned long ulong_data;
	unsigned short ushort_data;

	finddata(true,0);
	if (!WriteHeader(level))
		return false;

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

bool DIISect::WriteModInfo(int len, writer *level)
{
	level->write(IDS_MODINFLEN,len);
	if (len)
		writeblock(IDS_MODINFO,len,level);
	return true;
}


bool DIISect::Write(writer *level)
{
	writer *level2,*level3;
    unsigned short i;
	unsigned long ulong_data;
	unsigned short ushort_data;
	unsigned char uchar_data;
	unsigned short numModules;

	finddata(true,0);
	if (!WriteHeader(level))
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

bool DDBSect::Write(writer *level)
{
    writer *level2;
	unsigned short ushort_data;
	unsigned char uchar_data;

	finddata(true,0);
	if (!WriteHeader(level))
		return false;

	level2 = level->child();
	level->write(IDS_DDB);

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

	writeblock(IDS_DATA,Len() - currentpos,level2);

	level->removechild(level2);

	return true;
}

bool DDRequestSect::Write(writer *level)
{
	writer *level2;
	unsigned short ushort_data;
	unsigned char uchar_data;

	finddata(true,0);
	if (!WriteHeader(level))
		return false;

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
bool DLCancelSect::Write(writer *level)
{
    writer *level2;
	unsigned long ulong_data;
	unsigned short ushort_data;
	unsigned char uchar_data;

	finddata(true,0);
	if (!WriteHeader(level))
		return false;

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

bool DSISect::Write(writer *level)
{
	writer *level2;
	unsigned short pdlen;

	finddata(true,0);
	if (!WriteHeader(level))
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

bool ChanChangeMessageSect::Write(writer *level)
{
	return DSMCCdatasection::Write(level);
}

bool UNPassMessageSect::Write(writer *level)
{
	return DSMCCdatasection::Write(level);
}

bool DownloadMessageSect::CompSubDesc(writer *level)
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

void DownloadMessageSect::compatibilitydesc(writer *level)
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

bool DSMCCdatasection::WriteHeader(writer *level)
{
	unsigned char adaptlen,uchar_data;
	unsigned short messagelen, ushort_data;
	unsigned long ulong_data;

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

		adaptlevel = level->child();
		level->write(IDS_ADAPT);
			
		if(!getbyte(uchar_data))
			return false;
		adaptlevel->write(IDS_TYPE, uchar_data);

		writeblock(IDS_ADAPT,adaptlen,adaptlevel);
		level->removechild(adaptlevel);
	}
	return true;
}

bool DSMCCdatasection::Write(writer *level)
{
	finddata(true,0);
    if (!WriteHeader(level))
		return false;

	return true;
}

bool DSMCCdatasection::getushort(unsigned short& data, bool inc)
{
	unsigned char buff[2];

	if (getdata(2,buff,inc))
	{
		data = (buff[0] << 8 ) + buff[1];
		return true;
	}
	else
		return false;
}
bool DSMCCdatasection::getbyte(unsigned char& byte, bool inc)
{

	return getdata(1,&byte,inc);
}

bool DSMCCdatasection::get12bits(unsigned short& data, bool inc)
{

	unsigned char buff[2];

	if (getdata(2,buff,inc))
	{
		data = ( (buff[0] & 0x0F) << 8 ) + buff[1];
		return true;
	}
	else
		return false;

}

bool DSMCCdatasection::getulong(unsigned long& data, bool inc)
{
	unsigned char buff[4];

	if (getdata(4,buff,inc))
	{
		data = (buff[0] << 24 ) + (buff[1] << 16) + (buff[2] << 8) + buff[3];
		return true;
	}
	else
		return false;

}
bool DSMCCdatasection::getdata(unsigned long length, unsigned char *buffer, bool inc)
{
	if (buffer == NULL)
		return false;

	// is the offset in the section?
	if (currentpos < Len() )
	{
		// is the current section big enough to hold all the data?
		if ( length + currentpos -1 < Len())
		{
			// Yes let's get out of here
			dsmccdata->finddata(true,currentpos);
			dsmccdata->getdata(length,buffer,inc);
		}
		else
		{
			//no return false
			return false;
		}
	}
	else
	{
		return false;
	}

	// increment the start data pointer
	if (inc)
		currentpos += length;

	return true;
}

bool DSMCCdatasection::finddata(bool fromstart, long len)
{
	if (fromstart)
		currentpos = len;
	else
		currentpos += len;

	return true;
}

void DSMCCdatasection::writeblock(component_ids ID, unsigned long len, writer* level)
{
	if (len > Len() + 1 -currentpos)
		return;

	if (len)
	{
		unsigned char *buffer = new unsigned char[len];
		getdata(len,buffer,true);
		level->bindata(ID,buffer,len);
		delete buffer;
	}
}

bool DSMCCdatasection::Write(char *name,char *ext)
{
    char localfilename[1024];

    int localfile = -1;

    if (name != NULL)
    {
        if (ext !=NULL)
            sprintf(localfilename, "%s%s.%lx.dsmcc",name,ext,TransID());
        else
            sprintf(localfilename, "%s.%lx.dsmcc",name,TransID());
    }
    else
        return false;

    localfile = open(localfilename,O_WRONLY|O_CREAT,0666);

    if (localfile == -1)
        return false;

    finddata(true,DSMCCHeaderLen());

    while (getdata(1024,(unsigned char*)localfilename,true))
        write(localfile,localfilename,1024);

    if ((Len() - DSMCCHeaderLen()) % 1024)
    {
        getdata((Len() - DSMCCHeaderLen()) % 1024,(unsigned char*)localfilename,true);
        write(localfile,localfilename,(Len() - DSMCCHeaderLen()) % 1024);
    }
    close(localfile);

    if (headerAdaptLen())
    {
        if (ext != NULL)
            sprintf(localfilename, "%s%s.%lx.adapt",name,ext,TransID());
        else
            sprintf(localfilename, "%s.%lx.adapt",name,TransID());

        localfile = open(localfilename,O_WRONLY|O_CREAT,0666);
        if (localfile == -1)
            return false;
        finddata(true,12);
        getdata(headerAdaptLen(),(unsigned char*)localfilename,true);
        write(localfile,localfilename,headerAdaptLen());
        close(localfile);
    }

    return true;
}
