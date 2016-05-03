// DSMCC.cpp: implementation of the DSMCC class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include "OCDSMCC.h"

bool OCDII::Write(writer *level)
{
	writer *level2,*level3;
    unsigned short i;
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
bool OCDII::parse(struct OCDIIstruct *dii)
{
    unsigned short i;
    unsigned long ulong_data;
    unsigned char uchar_data;
    unsigned char useinflen;

    if (dii == NULL)
        return false;

    memset(dii,0,sizeof(struct OCDIIstruct));

    finddata(true,0);
    if (!getulong(dii->dlid))
        return false;

    if (!getushort(dii->blocksize))
        return false;

    if (!getbyte(dii->windowsize))
            return false;

    if (!getbyte(dii->ack_period))
        return false;

    if (!getulong(dii->dlwindow))
        return false;

    if (!getulong(dii->dlscenario))
        return false;

    if (!parsecompatibilitydesc(&dii->compatibility))
        return false;

    if(!getushort(dii->modules))
        return false;

    if (dii->modules)
    {
        dii->mods = new struct module[dii->modules];

        if (!dii->mods)
            return false;

        memset(dii->mods,0,dii->modules*sizeof(struct module));

        for (i = 0; i<dii->modules; i++)
        {
            unsigned char inflen;
            if(!getushort(dii->mods[i].ModuleID))
                return false;

            if (!getulong(dii->mods[i].size))
                return false;

            if (!getbyte(dii->mods[i].ver))
                return false;

            if(!getbyte(inflen))
                return false;

            if (inflen > 0)
            {
                dii->mods[i].inf_active = true;

                if (!getulong(dii->mods[i].inf.mto))
                    return false;

                if (!getulong(dii->mods[i].inf.bto))
                    return false;

                if (!getulong(dii->mods[i].inf.mbt))
                    return false;

                if(!getbyte(dii->mods[i].inf.tapscount))
                    return false;

                if (dii->mods[i].inf.tapscount)
                {
                    dii->mods[i].inf.taps = new struct tap[dii->mods[i].inf.tapscount];
                    if (!dii->mods[i].inf.taps)
                        return false;

                    memset(dii->mods[i].inf.taps,0,sizeof(struct tap)*dii->mods[i].inf.tapscount);

                    for(unsigned char tc=0; tc <dii->mods[i].inf.tapscount; tc++)
                    {
                        if (!parseTaps(&dii->mods[i].inf.taps[tc]))
                            return false;
                    }
                }
                dii->mods[i].inf.compressed = false;

                if(!getbyte(useinflen))
                    return false;

                while (useinflen > 0)
                {
                    unsigned char desclen;
                    unsigned char descID;
                    if(!getbyte(descID))
                        return false;
                    if(!getbyte(desclen))
                        return false;

                    if (descID == 0x09)
                    {

                        // add to a compressed modules list
                        if (!getbyte(uchar_data))
                            return false;
                        if (!getulong(ulong_data))
                            return false;
                        dii->mods[i].inf.compressed = true;
                        useinflen -= 7;

                    }
                    else
                    {
                        finddata(false,desclen);  // ignore length byte as len is 4

                        useinflen -= desclen + 2;

                    }
                }
            }
            else
                dii->mods[i].inf_active = false;

        }
    }
    if(!getushort(dii->pdlen))
        return false;

    if (dii->pdlen)
    {
        dii->pd = new unsigned char [dii->pdlen];

        return getdata(dii->pdlen,dii->pd,true);
    }

    return true;
}
void OCDII::clean(struct OCDIIstruct *dii)
{
    unsigned short i;

    if (dii == NULL)
        return;

    cleancompatibilitydesc(&dii->compatibility);

    if (dii->mods)
    {
        for (i = 0; i<dii->modules; i++)
        {
            if (dii->mods[i].inf.taps)
            {
                for(unsigned char tc=0; tc <dii->mods[i].inf.tapscount; tc++)
                {
                    cleanTaps(&dii->mods[i].inf.taps[tc]);
                }
                delete [] dii->mods[i].inf.taps;
                dii->mods[i].inf.taps = NULL;
            }

        }
        delete [] dii->mods;
    }

    if (dii->pd)
    {
        delete[] dii->pd;
        dii->pd = NULL;
    }

}

bool OCModule::Write(writer *level)
{
        if (!finddata(true,0))
            return false;

	BIOPmessage(level);

	return true;
}

bool OCDSI::Write(writer *level)
{
    writer *level2;
	unsigned short pdlen;

    if (!finddata(true,0))
        return false;

	level2 = level->write(IDS_DSI);

	writeblock(IDS_SERVID,20,level2);

	compatibilitydesc(level2);

	if(!getushort(pdlen))
		return false;
    level2->write(IDS_PDLEN, pdlen);

    ServiceGateway(level2);

	return true;
}

bool OCDSI::parse(struct OCDSIstruct *dsi)
{

    if (dsi ==NULL)
        return false;

    memset(dsi,0,sizeof(struct OCDSIstruct));

    if (!finddata(true,0))
            return false;

    if (!getdata(20,dsi->serverid,true))
        return false;

    if (!parsecompatibilitydesc(&dsi->compatibility))
        return false;

    if(!finddata(false,2))
        return false;
    if (!parseServiceGateway(&dsi->srg))
        return false;

    return true;
}
void OCDSI::clean(struct OCDSIstruct *dsi)
{

    if (dsi ==NULL)
        return;

    cleancompatibilitydesc(&dsi->compatibility);

    cleanServiceGateway(&dsi->srg);
}


void DSMCCOCbase::compatibilitydesc(writer *level)
{

    writer *level2,*level3, *level4;
    unsigned short count, len;
    unsigned char subcount;
    unsigned char uchar_data;
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
bool DSMCCOCbase::parsecompatibilitydesc(struct compat *comp)
{
    unsigned short count, len=0;
    unsigned char subcount;
    unsigned char uchar_data;

    if (!comp)
        return false;

    memset(comp, 0, sizeof(struct compat)) ;

    if(!getushort(len))
        return false;

    if(len == 0) // no compatibility descriptor so bye bye
    {
        return true; //not an error so continue
    }

    if(!getushort(comp->desccount))
        return false;

    if (comp->desccount)
        comp->descriptor = new struct desc[comp->desccount];

    memset(comp->descriptor, 0, sizeof(struct desc)*comp->desccount);

    for (count=0; count < comp->desccount; count++)
    {
        if (!getbyte(comp->descriptor[count].type))
            return false;

        if (!getbyte(uchar_data))
            return false;

        if (!getbyte(comp->descriptor[count].spectype))
            return false;

        if (!getdata(3,comp->descriptor[count].specdata,true))
            return false;

        if (!getushort(comp->descriptor[count].model))
            return false;

        if (!getushort(comp->descriptor[count].ver))
            return false;

        if(!getbyte(comp->descriptor[count].subcount))
            return false;

        subcount = comp->descriptor[count].subcount;
        comp->descriptor[count].descs = NULL;

        if (subcount)
        {
            comp->descriptor[count].descs = new struct subdesc [subcount];

            if (comp->descriptor[count].descs ==NULL)
                return false;

            memset(comp->descriptor[count].descs,0,sizeof(struct subdesc)*subcount);

            while(subcount--)
            {
                if (!getbyte(comp->descriptor[count].descs[subcount].type))
                    return false;

                if(!getbyte(comp->descriptor[count].descs[subcount].len))
                    return false;

                comp->descriptor[count].descs[subcount].data = NULL;
                if (comp->descriptor[count].descs[subcount].len)
                {
                    comp->descriptor[count].descs[subcount].data = new unsigned char [comp->descriptor[count].descs[subcount].len];
                    if (!getdata(comp->descriptor[count].descs[subcount].len,comp->descriptor[count].descs[subcount].data,true))
                        return false;
                }
            }
        }
    }
    return true;
}
void DSMCCOCbase::cleancompatibilitydesc(struct compat *comp)
{
    unsigned short count;
    unsigned char subcount;

    if (!comp)
        return;

    if(comp->desccount == 0 || !comp->descriptor)
    {
        return;
    }

    for (count=0; count < comp->desccount; count++)
    {
        subcount = comp->descriptor[count].subcount;

        if (subcount)
        {
            if (comp->descriptor[count].descs)
            {
                while(subcount--)
                {
                    if (comp->descriptor[count].descs[subcount].data)
                        delete [] comp->descriptor[count].descs[subcount].data;
                }
                delete [] comp->descriptor[count].descs;
            }
        }
    }
    delete [] comp->descriptor;
    comp->descriptor = NULL;
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
bool DSMCCOCbase::parseIOR(IORstruct *ior)
{
    unsigned long count;

    if (!ior)
        return false;

    memset(ior,0,sizeof(struct IORstruct));

    if(!getulong(ior->profileidlen))
        return false;

    if (ior->profileidlen)
    {
        ior->ProfileId = new unsigned char[ior->profileidlen];

        if (!getdata(ior->profileidlen,ior->ProfileId,true))
            return false;
    }

    if(!getulong(ior->profilecount))
        return false;

    if (!ior->profilecount)
        return true;

    ior->tags = new struct taggedprofile[ior->profilecount];
    memset(ior->tags,0,sizeof(struct taggedprofile)*ior->profilecount);

    if (!ior->tags)
        return false;

    for (count =0; count < ior->profilecount;count++)
        if (!parsetaggedprofiles(&ior->tags[count]))
            return false;

    return true;
}
void DSMCCOCbase::cleanIOR(IORstruct *ior)
{
    unsigned long count;

    if (!ior)
        return;

    if (ior->ProfileId)
    {
        delete [] ior->ProfileId;
        ior->ProfileId = NULL;
    }

    if(!ior->profilecount)
        return;

    for (count =0; ior->tags && (count < ior->profilecount);count++)
        cleantaggedprofiles(&ior->tags[count]);

    if (ior->tags)
        delete [] ior->tags;
    ior->tags = NULL;
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

bool DSMCCOCbase::parsetaggedprofiles(taggedprofile *prof)
{
    unsigned long len;
    unsigned char count;

    if (!prof)
        return false;

    memset(prof,0,sizeof(taggedprofile));

    if(!getulong(prof->tag))
        return false;

    if(!getulong(len))
        return false;

    if(!getbyte(prof->byteorder))
        return false;

    if(!getbyte(count))
        return false;

    while (count--)
    {
        unsigned long locType;
        if(! getulong(locType))
            return false;
        switch (locType)
        {
        case 0x49534f50:
            if (!parseObjectLocation(&prof->oloc))
                return false;
            break;
        case 0x49534f40:
            if (!parseConnBinder(&prof->conbind))
                return false;
            break;
        default:
        {
            //ignore the others
            unsigned char locLen;

            if(!getbyte(locLen))
                return false;
            if (!finddata(false,locLen))
                    return false;
        }
        break;
        }
    }
    return true;
}
void DSMCCOCbase::cleantaggedprofiles(taggedprofile *prof)
{

    if (!prof)
        return;

    cleanObjectLocation(&prof->oloc);
    cleanConnBinder(&prof->conbind);
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
bool DSMCCOCbase::parseObjectLocation(ObjectLocationstruct *ol)
{
    unsigned char uchar_data;

    if (!ol)
        return false;

    memset(ol,0,sizeof(ObjectLocationstruct));

    if(!getbyte(uchar_data))
        return false;

    if(!getulong(ol->carouselid))
        return false;

    if(!getushort(ol->moduleid))
        return false;

    if(!getbyte(ol->majVer))
        return false;

    if(!getbyte(ol->minVer))
        return false;

    if(!getbyte(ol->keylen))
        return false;

    if (!ol->keylen)
        return true;

    ol->key = new unsigned char [ol->keylen];
    if (!getdata(ol->keylen,ol->key,true))
        return false;
    return true;
}
void DSMCCOCbase::cleanObjectLocation(ObjectLocationstruct *ol)
{
    if (!ol)
        return;

    if (ol->key)
        delete [] ol->key;
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
bool DSMCCOCbase::parseConnBinder(ConnBinderstruct *conb)
{
    unsigned char uchar_data;

    if (!conb)
        return false;

    memset(conb,0,sizeof(ConnBinderstruct));

    if(!getbyte(uchar_data))
        return false;

    if(!getbyte(conb->tapscount))
        return false;

    if (!conb->tapscount)
        return true;

    conb->Taps = new struct tap[conb->tapscount];

    for(uchar_data=0;uchar_data<conb->tapscount;uchar_data++)
        if(!parseTaps(&conb->Taps[uchar_data]))
            return false;
    return true;
}

void DSMCCOCbase::cleanConnBinder(ConnBinderstruct *conb)
{
    unsigned char count;
    if ((!conb->tapscount) || (!conb->Taps))
        return ;

    for(count=0;count<conb->tapscount;count++)
        cleanTaps(&conb->Taps[count]);

    delete [] conb->Taps;
    memset(conb,0,sizeof(ConnBinderstruct));
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
bool DSMCCOCbase::parseTaps(struct tap *tap)
{
    unsigned char uchar_data;

    if (!tap)
        return false;

    memset(tap,0,sizeof (struct tap));

    if(!getushort(tap->id))
        return false;

    if(!getushort(tap->use))
        return false;

    if(!getushort(tap->asstag))
        return false;

    if(!getbyte(uchar_data))
        return false;

    if(!getushort(tap->selectortype))
        return false;

    if(!getulong(tap->transid))
        return false;

    if(!getulong(tap->timeout))
        return false;
    return true;
}
void DSMCCOCbase::cleanTaps(struct tap *tap)
{
    if(!tap)
        return;
}

void OCModule::BIOPmessage(writer *level)
{
	writer *level2;
	unsigned long len,type;
	unsigned char uchar_data;

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
bool DSMCCOCbase::skipcontext()
{
        unsigned char count;
        unsigned short len;
        unsigned long ulong_data;

        if(!getbyte(count))
                return false;

        while (count--)
        {
                if(!getulong(ulong_data))
                        return false;

                if(!getushort(len))
                        return false;

                if (!finddata(false,len))
                    return false;
        }
        return true;
}
bool DSMCCOCbase::parsecontext(struct contextstruct *cont)
{
    unsigned char count;

    if (!cont)
        return false;

    memset(cont,0,sizeof(struct contextstruct));

    if(!getbyte(cont->count))
        return false;

    count = cont->count;

    if ( count == 0)
        return true;

    cont->cont = new struct servcontext[count];

    if (cont->cont == NULL)
        return false;

    memset(cont->cont,0,sizeof(struct servcontext)*count);

    while (count--)
    {
        if(!getulong(cont->cont[count].ID))
            return false;

        if(!getushort(cont->cont[count].len))
            return false;

        cont->cont[count].context = new unsigned char [cont->cont[count].len];

        if (!getdata(cont->cont[count].len,cont->cont[count].context,true))
            return false;
    }
    return true;
}

void DSMCCOCbase::cleancontext(struct contextstruct *cont)
{
    unsigned char count;

    count = cont->count;

    if (!cont->cont)
        return;

    if ( count == 0)
        return;

    while (count--)
    {
        if(cont->cont[count].context)
            delete [] cont->cont[count].context;

    }
    delete[] cont->cont;
    cont->cont = NULL;
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
#if 0
bool OCModule::extractfile(QFile *dest)
{
    unsigned char buffer[1024];

    unsigned short objinflen;
    unsigned long Len,ulong_data;

    if(!getushort(objinflen))
        return false;

    if (!finddata(false,objinflen))
        return false;

    if (!skipcontext())
        return false;

    if(!getulong(ulong_data))
        return false;

    if(!getulong(Len))
        return false;

    while (Len >= 1024)
    {
        if (!getdata(1024,buffer,true))
            return false;
        dest->write((char*)buffer,1024);
    }
    if (Len)
    {
        if (!getdata(Len,buffer,true))
            return false;
        dest->write((char*)buffer,Len);
    }
    return true;
}
#endif


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
bool OCModule::extractdir(struct BIOPDirlist *dest)
{
        unsigned short len,bindingscount,objinflen;
        unsigned long Mblen;
        unsigned char uchar_data;

        if (dest == NULL)
            return false;

        memset(dest, 0, sizeof(struct BIOPDirlist));

        if(!getushort(len))
                return false;

        if (!finddata(false,len))
            return false;

        if (!skipcontext())
            return false;

        if(!getulong(Mblen))
                return false;

        if(!getushort(len))
                return false;

        dest->count = len;
        dest->dirlist = new struct DirItem[len];

        if (!dest->dirlist)
            return false;

        for(bindingscount = 0; bindingscount < len;bindingscount++)
        {
            extractBIOPName(&dest->dirlist[bindingscount]);

            if(!getbyte(uchar_data))
                  return false;

            if (!parseIOR(&dest->dirlist[bindingscount].ior))
                return false;

            if(!getushort(objinflen))
                return false;

            if (!finddata(false, objinflen))
                return false;

        }
        return true;
}

bool OCModule::skipmessage()
{
        unsigned short len;
        unsigned long Mblen;

        if(!getushort(len))
                return false;

        if (!finddata(false,len))
            return false;

        skipcontext();

        if(!getulong(Mblen))
                return false;

        return finddata(false,Mblen);
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
bool OCModule::extractBIOPName(struct DirItem *dest)
{
        unsigned char componentscount;
        unsigned char Len;

        if(!dest)
            return false;

        if(!getbyte(componentscount))
            return false;

        if (componentscount != 1)
            return false;

        if(!getbyte(Len))
            return false;

        if (!getdata(Len,(unsigned char*)dest->name,true))
            return false;

        if(!getbyte(Len))
            return false;

        if (Len != 4)
            return false;

        return getulong(dest->kind);
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

#if 0
bool OCModule::extractFile(unsigned char *key, unsigned char klen,QFile *dest)
{
    unsigned long len,type;
    unsigned char uchar_data;
    unsigned char keylen;
    unsigned char keydata[256];
    unsigned long kindlen;
    bool want = false;

    if (!dest || !key || !keylen)
        return false;

    if (!finddata(true,0))
        return false;

    while (1) // the failure to read data will drop out of the loop
    {
        unsigned long biopdata;

        if(!getulong(biopdata))
            return false;
        if (biopdata != 0x42494F50)
            return false;

        if(!getbyte(uchar_data))
            return false;

        if(!getbyte(uchar_data))
            return false;

        if(!getbyte(uchar_data))
            return false;

        if(!getbyte(uchar_data))
            return false;

        if(!getulong(len))
            return false;

        if(!getbyte(keylen))
            return false;

        if (!getdata(keylen,keydata,true))
            return false;

        if (keylen == klen)
            while(keylen--)
                if (keydata[keylen-1] != key[keylen-1])
                    break;

        if (!keylen)
            want = true;

        if (!getulong(kindlen))
            return false;

        if (kindlen != 4)
            return false;

        if(!getulong(type))
            return false;

        switch (type)
        {
        case 0x66696C00:
            if (want)
                return extractfile(dest);
            else
                if (!skipmessage())
                    return false;
            break;

        default:
            if (want)
                return false; // looking for a file so error
            else
                if (!skipmessage())
                    return false;
            break;
        }
    }
    return false;
}
#endif
bool OCModule::extractDIR(unsigned char *key, unsigned char klen, struct BIOPDirlist *Dir)
{
    unsigned long len,type;
    unsigned char uchar_data;
    unsigned char keylen;
    unsigned char keydata[256];
    unsigned long kindlen;
    bool want = false;

    if (!Dir || !key || !keylen)
        return false;

    if (!finddata(true,0))
        return false;

    while (1) // the failure to read data will drop out of the loop
    {
        unsigned long biopdata;

        if(!getulong(biopdata))
            return false;
        if (biopdata != 0x42494F50)
            return false;

        if(!getbyte(uchar_data))
            return false;

        if(!getbyte(uchar_data))
            return false;

        if(!getbyte(uchar_data))
            return false;

        if(!getbyte(uchar_data))
            return false;

        if(!getulong(len))
            return false;


        if(!getbyte(keylen))
            return false;

        if (!getdata(keylen,keydata,true))
            return false;

        if (keylen == klen)
            while(keylen--)
                if (keydata[keylen-1] != key[keylen-1])
                    break;

        if (keylen == 255)
            want = true;

        if (!getulong(kindlen))
            return false;

        if (kindlen != 4)
            return false;

        if(!getulong(type))
            return false;

        switch (type)
        {
        case 0x64697200:
        case 0x73726700:// SRG and Dir have same struct so delibarate fall through
            if (want)
                return extractdir(Dir);
            else
                if (!skipmessage())
                    return false;
            break;
        default:
            if (want)
                return false;
            else
                if (!skipmessage())
                    return false;
            break;
        }
    }
    return false;
}



void OCDSI::ServiceGateway(writer *level)
{
    writer *level2;
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

bool OCDSI::parseServiceGateway(struct ServiceGatewaystruct *sgw)
{
    unsigned char count;

    if (sgw ==NULL)
        return false;
    memset(sgw,0,sizeof(struct ServiceGatewaystruct));

    if (!parseIOR(&sgw->ior))
        return false;

    if(!getbyte(sgw->tapscount))
        return false;

    if (sgw->tapscount)
    {
        sgw->taps = new struct tap [sgw->tapscount];

        if (sgw->taps == NULL)
            return false;
        memset(sgw->taps, 0, sizeof (struct tap)*sgw->tapscount);

        for(count=0; count< sgw->tapscount;count++)
            if (!parseTaps(&sgw->taps[count]))
                return false;
    }
    if (!parsecontext(&sgw->context))
        return false;

    if(!getushort(sgw->userlen))
        return false;

    sgw->userinfo = NULL;

    if (sgw->userlen)
    {
        sgw->userinfo = new unsigned char[sgw->userlen];
        if (!getdata(sgw->userlen,sgw->userinfo,true))
            return false;
    }
    return true;
}
void OCDSI::cleanServiceGateway(struct ServiceGatewaystruct *sgw)
{
    unsigned char count;

    if (sgw ==NULL)
        return;

    cleanIOR(&sgw->ior);

    if (sgw->tapscount && sgw->taps)
    {
        for(count=0; count< sgw->tapscount;count++)
            cleanTaps(&sgw->taps[count]);
    }
    if (sgw->taps)
        delete [] sgw->taps;

    cleancontext(&sgw->context);

    if (sgw->userlen)
    {
        if(sgw->userinfo)
            delete [] sgw->userinfo;

        sgw->userinfo = NULL;
    }
}



