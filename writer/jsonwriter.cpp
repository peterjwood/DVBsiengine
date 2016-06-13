// writer.cpp: implementation of the writer class.
//
//////////////////////////////////////////////////////////////////////

#include "jsonwriter.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "JSONstringtable.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static jsonwriter NextLevel[256];

char writebuffer[64*1024];

bool jsonwriter::writetitle(component_ids id)
{
	if (id < IDS_MAX_STRING)
	{
		sprintf(writebuffer,"\"%s\" :",cSIJSONStringTable[id] );
	}
	else
		sprintf(writebuffer,"Invalid string_id %.2X ",id);
	return ProcessData(writebuffer);
}

bool jsonwriter::writetitle(char* message)
{
	sprintf(writebuffer,"\"%s\" :",message );
	return ProcessData(writebuffer);
}

bool jsonwriter::write(char* message)
{
	return ProcessData(message);
}
bool jsonwriter::ProcessData(char* message)
{	
	int i;

	for (i = 0; (i < indent); i++)
		fprintf(output,"\t");

	if (firstitem)
	{
		fprintf(output, "{%s",message);
		firstitem=false;
	}
	else if (lastitem)
	{
		fprintf(output, "%s",message);
		lastitem=false;
		firstitem=true;
	}
	else
		fprintf(output, ",%s",message);
	
	fprintf(output,"\n");
	return true;
}
void jsonwriter::startlist(component_ids id)
{
	if (id < IDS_MAX_STRING)
	{
		sprintf(writebuffer,"\"%s\" :[",cSIJSONStringTable[id] );
	}
	else
		sprintf(writebuffer,"Invalid string_id %.2X ",id);
	ProcessData(writebuffer);

	writer::startlist(id);
}
void jsonwriter::startlist(char *listname)
{
	sprintf(writebuffer,"\"%s\" :[", listname);
	ProcessData(writebuffer);

	writer::startlist(listname);
}

void jsonwriter::listitem()
{
	if (listcount)
	{
		for (int i = 0; (i < indent); i++)
			fprintf(output,"\t");
		fprintf(output,",\n");
	}
	writer::listitem();
}

void jsonwriter::endlist()
{
	for (int i = 0; (i < indent); i++)
		fprintf(output,"\t");

	fprintf(output, "]\n");

	writer::endlist();
}
void jsonwriter::enditem()
{
	lastitem = true;
	ProcessData("}");
}
writer *jsonwriter::child()
{
	NextLevel[indent].init();
	NextLevel[indent].indent = indent + 1;
	NextLevel[indent].output = output;
	return &NextLevel[indent];
}
void jsonwriter::removechild(writer *child)
{
	if (child == &NextLevel[indent])
	{
		if (NextLevel[indent].lastitem==false)
		{
			NextLevel[indent].lastitem=true;
		}
		if (NextLevel[indent].firstitem==false)
			child->ProcessData("}");
	}
	else
		printf("Trying to free wrong child\n");
}

bool jsonwriter::write(component_ids id, unsigned int val)
{
	if (id < IDS_MAX_STRING)
	{
		sprintf(writebuffer,"\"%s\" :\"0x%.2X\"",cSIJSONStringTable[id] ,val);
	}
	else
		sprintf(writebuffer,"Invalid string_id %.2X ",id);

	return ProcessData(writebuffer);
}
bool jsonwriter::write(component_ids id, component_ids id2)
{
	if ((id < IDS_MAX_STRING) && (id2 < IDS_MAX_STRING))
	{
		sprintf(writebuffer,"\"%s\" :\"%s\"",cSIJSONStringTable[id] ,cSIJSONStringTable[id2]);
	}
	else
		sprintf(writebuffer,"Invalid string_id %.2X or %.2X",id,id2);

	return ProcessData(writebuffer);
}

bool jsonwriter::write(component_ids id)
{
#if 0
	return true;
#else

	if (id < IDS_MAX_STRING)
	{
		sprintf(writebuffer,"\"%s\":\"true\"",cSIJSONStringTable[id]);
	}
	else
		sprintf(writebuffer,"Unknown ID %d",id);

	return ProcessData(writebuffer);
#endif
}

bool jsonwriter::writetime(component_ids id,unsigned char *data, unsigned int len)
{
	if ((id != IDS_STIME) || (len != 5))
		sprintf(writebuffer, "writetime called with wrong ID %d or len %d",id,len);
	else
	{
		unsigned short tmp,year,month,day;

		tmp = (data[0]<<8) + data[1];
		year = (tmp - 15078) / 365;
		month = ( tmp - 14956 - (year * 365) )/ 30;
		day = tmp - 14956 - (year * 365) - (month * 30 );

		if ((month == 14) || (month == 15))
			tmp = 1;
		else
			tmp = 0;

		year += tmp + 1900;
		month = month - 1 - (12 * tmp);
		sprintf(writebuffer,"\"%s\":\"%X:%.2X:%.2X %d/%d/%d\"",cSIJSONStringTable[id],data[2],data[3],data[4],day,month,year);
	}
	return ProcessData(writebuffer);
}
bool jsonwriter::writeduration(component_ids id,unsigned char *data, unsigned int len)
{
	if ((id != IDS_DUR) || (len != 3))
		sprintf(writebuffer, "writeduration called with wrong ID %d or len %d",id,len);
	else
	{
		sprintf(writebuffer,"\"%s\":\"%X:%.2X:%.2X\"",cSIJSONStringTable[id],data[0],data[1],data[2]);
	}
	return ProcessData(writebuffer);
}


bool jsonwriter::bindata(component_ids id,unsigned char *data, unsigned int len)
{
	unsigned int pos;

	if (id == IDS_STIME)
		return writetime(id,data,len);

	if (id == IDS_DUR)
		return writeduration(id,data,len);

	sprintf(writebuffer,"\"%s\":\"",cSIJSONStringTable[id]);

	if (data != NULL)
	{
		for (pos=0;pos < len; pos++)
		{
			sprintf(writebuffer,"%s%.2X", writebuffer,data[pos]);
		}
		strcat(writebuffer,"\"");
	}
	return ProcessData(writebuffer);
}

bool jsonwriter::chardata(component_ids id,char *data, unsigned int len)
{
	unsigned int pos;

	sprintf(writebuffer,"\"%s\":\"",cSIJSONStringTable[id]);

	for (pos=0;pos < len; pos++)
	{
		if (data[pos] > 0x1f) 
		{
			sprintf(writebuffer,"%s%c",writebuffer,data[pos]);
		}
		else
			strcat(writebuffer,".");

	}
	strcat(writebuffer,"\"");
	return ProcessData(writebuffer);
}
