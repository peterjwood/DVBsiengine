// PMT.cpp: implementation of the PMT class.
//
//////////////////////////////////////////////////////////////////////

#include "PMT.h"

bool PMT::Write(writer* parent)
{
	writer *level;
	unsigned short tmpshort;

	level = parent->child();
	parent->writetitle(IDS_PMT);

	WriteGeneric(level);

	level->write(IDS_PROGNUM, ProgramNumber());

	finddata(true,0);
	if(!getushort(tmpshort))
		return false;
	level->write(IDS_PCRPID, tmpshort & 0x1FFF);

	ProgramInfoLoop(level);

	StreamLoop(level);
	parent->removechild(level);

	return true;
}

unsigned short PMT::ProgramNumber()
{
	return ID();

}

void PMT::ProgramInfoLoop(writer *parent)
{
	writer *level;
	unsigned short looplen;
	if(!get12bits(looplen))
		return;

	parent->write(IDS_LENGTH,looplen);

	parent->startlist(IDS_PROGINF);
	while (looplen)
	{
		level = parent->child();
		parent->listitem();
		unsigned short size = DecodeDescriptor(level,looplen);
		looplen -= size;
		finddata(false,size);
		parent->removechild(level);
	}
	parent->endlist();
}

bool PMT::StreamLoop(writer *parent)
{
	writer *level,*level2;
	unsigned short looplen,ushort_data;
	unsigned char uchar_data;

	parent->startlist(IDS_PROGSTR);
	while(currentpos < SectionPayloadLength())
	{
		level = parent->child();
		parent->listitem();
		if(!getbyte(uchar_data))
			return false;
		level->write(IDS_STRTYPE,uchar_data);

		if(!getushort(ushort_data))
			return false;
		level->write(IDS_ELEMPID,ushort_data & 0x1FFF);

		if(!get12bits(looplen))
			return false;

		level->write(IDS_ESINF);

		level->startlist(IDS_DESCRIPTORS);
		while (looplen)
		{
			level->listitem();
			level2 = level->child();
			unsigned short size = DecodeDescriptor(level2,looplen);
			looplen -= size;
			if (!size)
				return false;
			finddata(false,size);
			level->removechild(level2);
		}
		level->endlist();
		parent->removechild(level);
	}
	parent->endlist();

	return true;
}

bool PMT::getPIDs(bool *List, bool AddPAT, bool AddPMT, bool AddDSMCC, bool AddSubs, bool AddTTxt, bool AddVid, bool AddAudio)
{
	bool wantPID  = false;
	bool retval = false;
	unsigned char uchar_data;

	if (!AddPMT)
		return false;

	finddata(true,4);

	while(currentpos < SectionPayloadLength())
	{
		
	if(!getbyte(uchar_data))
		return false;

	switch(uchar_data)
	{
	case 0x00:
//		output.WriteString( "Reserved 0\r\n");
		break;
	case 0x01:
//		output.WriteString( "Video Type 1\r\n");
		break;
	case 0x02:
//		output.WriteString( "Video Type 2\r\n");
		break;
	case 0x03:
//		output.WriteString( "Audio 3\r\n");
		break;
	case 0x04:
//		output.WriteString( "Audio 4\r\n");
		break;
	case 0x05:
//		output.WriteString( "13818 private sections\r\n");
		break;
	case 0x06:
//		output.WriteString( "Private data in PES\r\n");
		if (AddTTxt || AddSubs)
			wantPID= true;
		break;
	case 0x07:
//		output.WriteString( "MHEG\r\n");
		break;
	case 0x08:
//		output.WriteString( "Annex A DSMCC\r\n");
		break;
	case 0x09:
//		output.WriteString( "ITU-T 222\r\n");
		break;
	case 0x0a:
//		output.WriteString( "13818-6 A\r\n");
		break;
	case 0x0b:
//		output.WriteString( "13818-6 B\r\n");
		if (AddDSMCC)
			wantPID = true;
		break;
	case 0x0c:
		if (AddDSMCC)
			wantPID = true;
//		output.WriteString( "13818-6 C\r\n");
		break;
	case 0x0d:
		if (AddDSMCC)
			wantPID = true;
//		output.WriteString( "13818-6 D\r\n");
		break;
	case 0x0e:
//		output.WriteString( "13818-1 Aux\r\n");
		break;
	default:
	break;

	}

	{
		unsigned short tmpshort;
		if (wantPID)
		{
			if(!getushort(tmpshort))
				return false;
			tmpshort &= 0x1FFF;
                        if (!List[tmpshort])
			{
				List[tmpshort]=true;
				retval = true;
			}
			wantPID = false;
		}
		else
			finddata(false,2);
		

		if (!get12bits(tmpshort))
			return false;
		finddata(false,tmpshort);
		}
	}
	return retval;	
}
