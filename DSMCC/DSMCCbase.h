#ifndef DSMCCBASE_H_INCLUDED
#define DSMCCBASE_H_INCLUDED

#include "writer.h"
#include "reader.h"

class DSMCCbase
{
public:
	DSMCCbase(reader *r){

	dsmccreader=r;

	};
	~DSMCCbase(){};

	virtual bool getdata(long length, unsigned char *buffer, bool inc = false);

	virtual bool finddata(bool fromstart, long len);

	virtual bool getulong(unsigned long& data, bool inc = true);

	virtual bool getbyte(unsigned char& data, bool inc = true);

	virtual bool getushort(unsigned short& data, bool inc = true);

	virtual bool get12bits(unsigned short& data, bool inc = true);

	virtual void writeblock( component_ids ID, unsigned long len, writer* level);

	unsigned int Len(){return dsmccreader->Len();};
	virtual bool Write(writer *level)=0;
protected:
	reader *dsmccreader;
};
#endif

