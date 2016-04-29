// DSMCC.cpp: implementation of the DSMCC class.
//
//////////////////////////////////////////////////////////////////////

#include "OCDSMCC.h"

bool OCDII::Write(writer *level)
{
	writer *level2,*level3;
	unsigned short len,i;
	unsigned long ulong_data;
	unsigned short ushort_data,numModules;
	unsigned char uchar_data;
	unsigned short pdlen;
	unsigned char useinflen;


	finddata(true,0);
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

		if (inflen > 0)
		{
			unsigned char tapscount;

			if (!getulong(ulong_data))
				return false;
			level3->write(IDS_MTO, ulong_data);
	
			if (!getulong(ulong_data))
				return false;
			level3->write(IDS_BTO, ulong_data);

			if (!getulong(ulong_data))
				return false;
			level3->write(IDS_MBT, ulong_data);

			if(!getbyte(tapscount))
				return false;
			level3->write(IDS_TAPSC,tapscount);

			while (tapscount--)
			{
				unsigned char selectorlen;

				if (!getushort(ushort_data))
					return false;
				level3->write(IDS_ID, ushort_data);

				if (!getushort(ushort_data))
					return false;
				level3->write(IDS_USE, ushort_data);

				if (!getushort(ushort_data))
					return false;
				level3->write(IDS_ASSTAG, ushort_data);

				if(!getbyte(selectorlen))
					return false;
				level3->write(IDS_SELLEN,selectorlen);

				writeblock(IDS_SELECTOR,selectorlen,level3);

			}

			if(!getbyte(useinflen))
				return false;

			level3->write(IDS_USEINFLEN,useinflen);

			while (useinflen > 0)
			{
				unsigned char desclen;
				writer *level4;
				unsigned char descID;
				if(!getbyte(descID))
					return false;

				if (descID == 0x09)
				{

					// add to a compressed modules list 
					level4 = level3->write(IDS_COMPDESC);

					finddata(false,1);  // ignore length byte as len is 4

					if (!getbyte(uchar_data))
						return false;
					level4->write(IDS_COMPMETH, uchar_data);

					if (!getulong(ulong_data))
						return false;
					level4->write(IDS_ORIGSIZ, ulong_data);

					useinflen -= 7;
	
				}
				else
				{
					level4 = level3->write(IDS_OTHERDESC, descID);

					if(!getbyte(desclen))
						return false;
					level4 = level4->write(IDS_DESCLEN, desclen);
	
					useinflen -= desclen + 2;
	
					writeblock(IDS_OTHERDESC,desclen,level4);
				}
			}
		}

	}

	if(!getushort(pdlen))
		return false;
	level3 = level2->write(IDS_PDLEN, pdlen);

	writeblock(IDS_PRIVDAT,pdlen,level3);


	return true;
}

bool OCModule::Write(writer *level)
{
	finddata(true,0);

	BIOPmessage(level);

	return true;
}

bool OCDSI::Write(writer *level)
{
	writer *level2,*level3;
	unsigned short pdlen;

	unsigned char *checkdata;

	finddata(true,0);

	level2 = level->write(IDS_DSI);

	writeblock(IDS_SERVID,20,level2);

	compatibilitydesc(level2);

	if(!getushort(pdlen))
		return false;
	level3 = level2->write(IDS_PDLEN, pdlen);

	ServiceGateway(level2);

	return true;
}

void DSMCCOCbase::compatibilitydesc(writer *level)
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
			unsigned char len1;
			if (!getbyte(uchar_data))
				return;
			level4 = level3->write(IDS_SUBDESCTYPE, uchar_data);
			
			if(!getbyte(len1))
				return;
			level4->write(IDS_LENGTH, len1);

			writeblock(IDS_SUBDESCTYPE,len1,level4);
		}
	}

}

void DSMCCOCbase::IOR(writer *level)
{
	writer *level2;
	unsigned long len;

	level2 = level->write(IDS_IOPIOR);
	if(!getulong(len))
		return;
	level2->write(IDS_DATALEN, len);

	if (len)
	{
		writeblock(IDS_IOPIOR,len,level2);
	}

	if(!getulong(len))
		return;

	level2->write(IDS_LENGTH, len);

	while (len--)
		taggedprofiles(level2);

}

void DSMCCOCbase::taggedprofiles(writer *level)
{
	writer *level2;
	unsigned long len,ulong_data;
	unsigned char count;
	unsigned char uchar_data;
	
	level2 = level->write(IDS_PROF);

	if(!getulong(ulong_data))
		return;
	level2->write(IDS_TAG, ulong_data);

	if(!getulong(len))
		return;

	level2->write(IDS_DATALEN, len);

	if(!getbyte(uchar_data))
		return;
	level2->write(IDS_BYTEORD, uchar_data);

	if(!getbyte(count))
		return;
	level2->write(IDS_COMPCOUNT, count);

	while (count--)
	{
		unsigned long locType;
		if(! getulong(locType))
			return;
		switch (locType)
		{
		case 0x49534f50:
			ObjectLocation(level2);
			break;
		case 0x49534f40:
			ConnBinder(level2);
			break;
		default:
		{
			unsigned char locLen;

			level2->write(IDS_UNKCOMP, locType);
			if(!getbyte(locLen))
				return;
			writeblock(IDS_UNKCOMP,locLen,level2);
		}
		break;
		}
	}

}

void DSMCCOCbase::ObjectLocation(writer *level)
{
	writer *level2;
	unsigned long ulong_data;
	unsigned short ushort_data;
	unsigned char len,uchar_data;

	level2 = level->write(IDS_BIOPOL);

	if(!getbyte(uchar_data))
		return;
	level2->write(IDS_LENGTH, uchar_data);

	if(!getulong(ulong_data))
		return;
	level2->write(IDS_CARID, ulong_data);

	if(!getushort(ushort_data))
		return;
	level2->write(IDS_MODID, ushort_data);

	if(!getbyte(uchar_data))
		return;
	level2->write(IDS_MAJVER, uchar_data);

	if(!getbyte(uchar_data))
		return;
	level2->write(IDS_MINVER, uchar_data);

	if(!getbyte(len))
		return;
	level2->write(IDS_OBJKEYLEN, len);

	writeblock(IDS_OBJKEY,len,level2);
}

void DSMCCOCbase::ConnBinder(writer *level)
{
	writer *level2;
	unsigned char len,uchar_data;

	level2 = level->write(IDS_DSMCONN);

	if(!getbyte(uchar_data))
		return;
	level2->write(IDS_LENGTH, uchar_data);

	if(!getbyte(len))
		return;
	level2->write(IDS_TAPSC,len);
	while (len--)
		Taps(level2);
}


void DSMCCOCbase::Taps(writer *level)
{
	writer *level2;
	unsigned short ushort_data;
	unsigned long ulong_data;
	unsigned char uchar_data;

	if(!getushort(ushort_data))
		return;
	level2 = level->write(IDS_TAP, ushort_data);

	if(!getushort(ushort_data))
		return;
	level2->write(IDS_USE, ushort_data);

	if(!getushort(ushort_data))
		return;
	level2->write(IDS_ASSTAG, ushort_data);

	if(!getbyte(uchar_data))
		return;
	level2->write(IDS_SELLEN, uchar_data);

	if(!getushort(ushort_data))
		return;
	level2->write(IDS_SELTYPE, ushort_data);

	if(!getulong(ulong_data))
		return;
	level2->write(IDS_TRANSID, ulong_data);

	if(!getulong(ulong_data))
		return;
	level2->write(IDS_TIMEOUT, ulong_data);
}

void OCModule::BIOPmessage(writer *level)
{
	writer *level2;
	unsigned long len,type;
	unsigned char uchar_data;

#if 0
	if (compressed())
	{
	    z_stream d_stream; /* decompression stream */
	    int err;

	    d_stream.zalloc = (alloc_func)0;
		d_stream.zfree = (free_func)0;
		d_stream.opaque = (voidpf)0;

	    d_stream.next_in  = sourcebuffer;
	    d_stream.avail_in = sourcepos;
		d_stream.next_out = inflatebuffer;
		d_stream.avail_out = 16*1024*1024;

	    err = inflateInit(&d_stream);

        err = inflate(&d_stream, Z_FINISH);
		if (err != Z_STREAM_END)
			return;

	    err = inflateEnd(&d_stream);
		inflatelen = d_stream.total_out;
	}
	else
	{
		inflatelen = sourcepos;
		memcpy(inflatebuffer,sourcebuffer,sourcepos);
	}
#endif
	//make sure we are at the start
	finddata(true,0);
	while (1) // the failure to read data will drop out of the loop
	{
		unsigned long biopdata;

		if(!getulong(biopdata))
			return;
		if (biopdata == 0x42494F50)
		{
			level2 = level->write(IDS_BIOPMESS);
		}
		else
			return;

		if(!getbyte(uchar_data))
			return;
		level2->write(IDS_MAJVER, uchar_data); // if this value has bit 4 set then could indicate compression

		if(!getbyte(uchar_data))
			return;
		level2->write(IDS_MINVER, uchar_data);

		if(!getbyte(uchar_data))
			return;
		level2->write(IDS_BYTEORD, uchar_data);

		if(!getbyte(uchar_data))
			return;
		level2->write(IDS_TYPE, uchar_data);

		if(!getulong(len))
			return;
		level2->write(IDS_MESSIZE, len);

		{
			unsigned char keylen;
			if(!getbyte(keylen))
				return;

			level2->write(IDS_OBJKEYLEN, keylen);
			writeblock(IDS_OBJKEY,keylen,level2);
		}

		{
			unsigned long kindlen;
			if (!getulong(kindlen))
				return;
			level2->write(IDS_OBJKINDLEN, kindlen);
			if (kindlen != 4)
			{
				writeblock(IDS_UNKNOWN,kindlen,level2);
			}
			else
			{
				if(!getulong(type))
					return;

				switch (type)
				{
				case 0x66696C00:
					file(level2);
					break;
				case 0x64697200:
					dir(level2);
					break;
				case 0x73747200:
					str(level2);
					break;
				case 0x73746500:
					ste(level2);
					break;
				case 0x73726700:
					srg(level2);
					break;
				default:
					level2->write(IDS_UNKNOWN,type);
					break;
				}
			}
		}
	}
}
void DSMCCOCbase::context(writer *level)
{
	unsigned char count;
	unsigned short len;
	unsigned long ulong_data;

	if(!getbyte(count))
		return;
	level->write(IDS_SERVCONTLICOUNT, count);

	while (count--)
	{
		if(!getulong(ulong_data))
			return;
		level->write(IDS_CONTID, ulong_data);

		if(!getushort(len))
			return;
		level->write(IDS_LENGTH, len);

		writeblock(IDS_SERVCONT,len,level);
	}
}


void OCModule::file(writer *level)
{
	writer *level2;
	unsigned short objinflen;
	unsigned long Len,ulong_data;

	level2 = level->write(IDS_FILE);

	if(!getushort(objinflen))
		return;

	level2->write(IDS_OBJINFLEN,objinflen);

	writeblock(IDS_FILECONSIZ,8,level2);

	writeblock(IDS_OBJINFO, objinflen - 8, level2);

	context(level2);

	if(!getulong(ulong_data))
		return;
	level2->write(IDS_MESSBODLEN, ulong_data);

	if(!getulong(Len))
		return;

	level2->write(IDS_CONLEN, Len);

	writeblock(IDS_CONTENT,Len,level2);

}

void OCModule::dir(writer *level)
{
	writer *level2,*level3;
	unsigned short len,bindingscount,objinflen;
	unsigned long Mblen;
	unsigned char uchar_data;

	level2 = level->write(IDS_DIR);

	if(!getushort(len))
		return;

	writeblock(IDS_DIR,len,level2);

	context(level2);

	if(!getulong(Mblen))
		return;
	level2->write(IDS_MESSBODLEN, Mblen);

	if(!getushort(bindingscount))
		return;
	level3 = level2->write(IDS_BINDCOUNT, bindingscount);

	while(bindingscount--)
	{
		BIOPName(level3);

		if(!getbyte(uchar_data))
			return;
		level3->write(IDS_BINDTYPE, uchar_data);

		IOR(level3);

		if(!getushort(objinflen))
			return;

		level3->write(IDS_OBJINFLEN, objinflen);

		writeblock(IDS_OBJINFO, objinflen,level3);

	}
}
void OCModule::BIOPName(writer *level)
{
	writer *level2;
	unsigned char componentscount;
	unsigned char Len;

	if(!getbyte(componentscount))
		return;

	level2 = level->write(IDS_BIOPNAME);

	level2->write(IDS_COMPCOUNT, componentscount);

	while (componentscount--)
	{
		if(!getbyte(Len))
			return;
		level2->write(IDS_IDLEN, Len);

		writeblock(IDS_ID,Len,level2);

		if(!getbyte(Len))
			return;
		level2->write(IDS_OBJKINDLEN, Len);

		writeblock(IDS_OBJKIND,Len,level2);
	}
}

void OCModule::str(writer *level)
{
	level->write(IDS_STR);
}

void OCModule::ste(writer *level)
{
	level->write(IDS_STE);
}

void OCModule::srg(writer *level)
{
	writer *level2,*level3;
	unsigned short len,bindingscount,objinflen;
	unsigned long Mblen;

	level2 = level->write(IDS_SRG);

	if(!getushort(len))
		return;

	writeblock(IDS_SRG,len,level2);

	context(level2);

	if(!getulong(Mblen))
		return;
	level2->write(IDS_MESSBODLEN, Mblen);

	if(!getushort(bindingscount))
		return;
	level3 = level2->write(IDS_BINDCOUNT, bindingscount);

	while(bindingscount--)
	{
		unsigned char uchar_data;
		BIOPName(level3);

		if(!getbyte(uchar_data))
			return;
		level3->write(IDS_BINDTYPE, uchar_data);

		IOR(level3);

		if(!getushort(objinflen))
			return;

		level3->write(IDS_OBJINFLEN, objinflen);

		writeblock(IDS_OBJINFO, objinflen,level3);

	}
}

void OCDSI::ServiceGateway(writer *level)
{
	writer *level2,*level3;
	unsigned short len;
	unsigned char count;

	level2 = level->write(IDS_SERVGATE);

	IOR(level2);

	if(!getbyte(count))
		return;
	level2->write(IDS_TAPSC,count);

	while (count--)
		Taps(level2);

	context(level2);

	if(!getushort(len))
		return;
	level2->write(IDS_USEINFLEN,len);

	writeblock(IDS_USEINFO,len,level2);

}


