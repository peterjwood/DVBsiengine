// PES.cpp: implementation of the PES class.
//
//////////////////////////////////////////////////////////////////////

#include "PES.h"

bool PES::Write(writer* parent)
{
	writer *level;
	unsigned long templong;
	unsigned char tmpchar;
	unsigned short tmpshort;

	unsigned short seclen;
	bool extPresent = false;

	level = parent->write(IDS_PES, PID());

	seclen = SectionLength();

	finddata(true,2);

	if(!getbyte(tmpchar))
		return false;

	level->write(IDS_PESID,tmpchar);

	if (tmpchar == 0xBE)
	{
		level->write(IDS_PESPAD);
	}
	else if (tmpchar == 0xBF)
	{
		level->write(IDS_PRIVATEPES2);
	}
	else if (tmpchar == 0xBD)
	{
		level->write(IDS_PRIVATEPES1);
		extPresent = true;
	}
	else if ((tmpchar >=0xC0) && (tmpchar <= 0xDF))
	{
		level->write(IDS_PESAUDIO);
		extPresent = true;
	}
	else if ((tmpchar >=0xE0) && (tmpchar <= 0xEF))
	{
		level->write(IDS_PESVIDEO);
		extPresent = true;
	}

	
	if(!getushort(tmpshort))
		return false;
	level->write(IDS_LENGTH,tmpshort);

	//seclen -= 3;

	if (extPresent)
	{
		unsigned char headerdatalen;

		if (!getbyte(tmpchar))
			return false;

		templong = ((unsigned long)tmpchar << 16);
		
		if (!getbyte(tmpchar))
			return false;
		templong += ((unsigned long)tmpchar << 8);

		if (!getbyte(tmpchar))
			return false;
		templong += tmpchar;

		level->write(IDS_PESEXT,templong);

		if (templong & 0x300000)
		{
			level->write(IDS_PESSCRAM,(templong&0x300000)>>20);
		}
		if (templong & 0x80000)
		{
			level->write(IDS_PESPRIO);
		}
		if (templong & 0x40000)
		{
			level->write(IDS_PESDATAALIGN);
		}
		if (templong & 0x20000)
		{
			level->write(IDS_PESCOPY);
		}
		if (templong & 0x10000)
		{
			level->write(IDS_PESORIG);
		}
		if (templong & 0xC000)
		{
			level->write(IDS_PTSDTS,(templong&0xC000)>>14);
		}
		if (templong & 0x2000)
		{
			level->write(IDS_PESESCR);
		}
		if (templong & 0x1000)
		{
			level->write(IDS_ESRATE);
		}
		if (templong & 0x800)
		{
			level->write(IDS_DSMTRICK);
		}
		if (templong & 0x400)
		{
			level->write(IDS_ADDCOPY);
		}
		if (templong & 0x200)
		{
			level->write(IDS_PESCRCFLAG);
		}
		if (templong & 0x100)
		{
			level->write(IDS_PESEXTFLAG);
		}
		level->write(IDS_PESHDRLEN,templong & 0xFF);

		//seclen -= 3;
		headerdatalen = (unsigned char)(templong & 0xFF);
		if (templong & 0xC000)
		{
			if (headerdatalen >= 5)
			{
				unsigned long PTSval;
				unsigned char PTSTop;
				if(!getbyte(PTSTop))
					return false;

				PTSval = (unsigned long)(PTSTop & 0x6) << 29;
				if (PTSTop & 0x8)
					PTSTop = 1;
				else
					PTSTop = 0;

				if(!getushort(tmpshort))
					return false;
				PTSval += (unsigned long)(tmpshort & 0xFFFE) << 15;
				if(!getushort(tmpshort))
					return false;
				PTSval += (unsigned long)(tmpshort & 0xFFFE) >> 1;
				level->write(IDS_PESPTSTOP,PTSTop);
				level->write(IDS_PESPTSVAL,PTSval);

			}
			headerdatalen -= 5;
			//seclen -= 5;
		}
		if (templong & 0x4000)
		{
			if (headerdatalen >= 5)
			{
				unsigned long DTSval;
				unsigned char DTSTop;
				if(!getbyte(DTSTop))
					return false;

				DTSval = (unsigned long)(DTSTop & 0x6) << 29;
				if (DTSTop & 0xf)
					DTSTop = 1;
				else
					DTSTop = 0;

				if(!getushort(tmpshort))
					return false;
				DTSval += (unsigned long)(tmpshort & 0xFFFE) << 15;
				if(!getushort(tmpshort))
					return false;
				DTSval += (unsigned long)(tmpshort & 0xFFFE) >> 1;
				level->write(IDS_PESDTSTOP,DTSTop);
				level->write(IDS_PESDTSVAL,DTSval);

			}
			headerdatalen -= 5;
			//seclen -= 5;
		}

		finddata(false,headerdatalen);
		//seclen -= headerdatalen;

	}

	finddata(true,0);
	writeblock(IDS_PESDATA,seclen,level);

	return true;
}

