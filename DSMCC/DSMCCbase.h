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

	bool getdata(long length, unsigned char *buffer, bool inc = false){return dsmccreader->getdata(length,buffer,inc);};

	bool finddata(bool fromstart, long len){return dsmccreader->finddata(fromstart,len);};

	bool getulong(unsigned long& data, bool inc = true){return dsmccreader->getulong(data,inc);};

	bool getbyte(unsigned char& data, bool inc = true){return dsmccreader->getbyte(data,inc);};

	bool getushort(unsigned short& data, bool inc = true){return dsmccreader->getushort(data,inc);};

	bool get12bits(unsigned short& data, bool inc = true){return dsmccreader->get12bits(data,inc);};

	bool writeblock( component_ids ID, unsigned long len, writer* level);

	unsigned int Len(){return dsmccreader->Len();};
	virtual bool Write(writer *level)=0;
protected:
	reader *dsmccreader;
};
#endif

