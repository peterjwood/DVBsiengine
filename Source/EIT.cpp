// EIT.cpp: implementation of the EIT class.
//
//////////////////////////////////////////////////////////////////////

#include "EIT.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool EIT::Write(writer* parent)
{
	writer *level;
	unsigned char uchar_data;
	unsigned short ushort_data;


	level = parent->child();
	parent->writetitle(IDS_EIT);

	if (TableType() == 0x4E)
		level->write(IDS_ACTUAL);
	else if (TableType() == 0x4F)
		level->write(IDS_OTHER);
	else if ((TableType() > 0x4F) && (TableType() < 0x60))
		level->write(IDS_ACTUALSCH);
	else if ((TableType() > 0x5F) && (TableType() < 0x70))
		level->write(IDS_OTHERSCH);
	else
		return false;

	if (CurrentNext())
		level->write(IDS_CUR);
	else
		level->write(IDS_NEXT);


	level->write(IDS_VER, TableVersion());

	level->write(IDS_SERVICEID, ServiceID());

	finddata(true,0);

	if(!getushort(ushort_data))
		return false;
	level->write(IDS_TSID, ushort_data);

	if(!getushort(ushort_data))
		return false;
	level->write(IDS_ONID, ushort_data);

	if(!getbyte(uchar_data))
		return false;
	level->write(IDS_SLASTSEC, uchar_data);

	if(!getbyte(uchar_data))
		return false;
	level->write(IDS_LASTTABID, uchar_data);

	EventLoop(level);

	parent->removechild(level);
	parent->enditem();
	return true;

}
unsigned short EIT::ServiceID()
{
	return ID();

}

void EIT::EventLoop(writer* parent)
{
	writer *level,*level2;
	unsigned short looplen, year,month,day,tmp,ushort_data;
	unsigned char uchar_data,uchar_data1,uchar_data2;

	if(!getushort(ushort_data))
		return;
	level = parent->child();
	parent->writetitle(IDS_EVENT);
	level->write(IDS_EVENTID, ushort_data);

	writeblock(IDS_STIME,5,level);

	writeblock(IDS_DUR,3,level);

	if(!getbyte(uchar_data))
		return;
	tmp = uchar_data;
	switch ((tmp & 0xE0)>>5)
	{
	case 0:
		level->write(IDS_RUNSTAT,IDS_UNDEF);
		break;
	case 1:
		level->write(IDS_RUNSTAT,IDS_NOTRUN);
		break;
	case 2:
		level->write(IDS_RUNSTAT,IDS_SOON);
		break;
	case 3:
		level->write(IDS_RUNSTAT,IDS_PAUSING);
		break;
	case 4:
		level->write(IDS_RUNSTAT,IDS_RUN);
		break;
	default:
		level->write(IDS_RUNSTAT,IDS_RESERR);
		break;
	}

	if (tmp & 0x10)
	{
		level->write(IDS_CAACT,IDS_TRUE);
	}
	else
	{
		level->write(IDS_CAACT,IDS_FALSE);
	}

	if(!getbyte(uchar_data))
		return;
	looplen = uchar_data + ((tmp & 0xF) << 8);


	level->startlist(IDS_EVDESCLOOP);
	while (looplen)
	{
		level2 = level->child();
		level->listitem();
		unsigned short size = DecodeDescriptor(level2,looplen);
		looplen -= size;
		finddata(false,size);

		level->removechild(level2);
		if (!size)
			return;
	}
	level->endlist();
	parent->removechild(level);
	
}



