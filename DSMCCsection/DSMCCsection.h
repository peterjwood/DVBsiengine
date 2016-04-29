// DSMCC.h: interface for the DSMCC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DSMCCsection_H_INCLUDED_)
#define _DSMCCsection_H_INCLUDED_

#include <vector>
#include <string.h>
#include "writer.h"

class DSMCCdatasection  
{
public:
	unsigned short DSMCCType(){return dsmccdata[1]; };
	unsigned long TransID()
		{
			unsigned char *data = dsmccdata.data();
			return (data[4] <<24 ) + (data[5] <<16 ) + (data[6] <<8 ) + data[7];
		};
	unsigned short MessID()
		{
			unsigned char *data = dsmccdata.data();
			return (data[2] <<8 ) + data[3];
		};
	unsigned char headerAdaptLen(){return dsmccdata[9]; };
	unsigned short messageLen()
		{
			unsigned char *data = dsmccdata.data();
			return (data[10] << 8) + data[11];
		};
	virtual unsigned short DSMCCHeaderLen(){return headerAdaptLen() + 12;};

	bool WriteHeader(writer *level);

	virtual bool Write(writer *level);
	virtual bool Write(char *name, char *ext=NULL);

	DSMCCdatasection(unsigned char *sect, int len);

	virtual ~DSMCCdatasection(){};

	static DSMCCdatasection *Allocate(unsigned char *data, int len);

	virtual bool operator==(DSMCCdatasection *comp)
	{
		unsigned char *data = comp->Payload();
		if ((DSMCCType() == data[1]) &&
			(MessID() == (data[2] <<8 ) + data[3]) &&
			(TransID() == (data[4] <<24 ) + (data[5] <<16 ) + (data[6] <<8 ) + data[7]))
			return true;

		return false;
	};

	bool getdata(unsigned long length, unsigned char *buffer, bool inc = false);
	unsigned char *getrawdata(unsigned long length, bool inc=false);
	bool finddata(bool fromstart, long len);

	bool getulong(unsigned long& data, bool inc = true);

	bool getbyte(unsigned char& data, bool inc = true);

	bool getushort(unsigned short& data, bool inc = true);

	bool get12bits(unsigned short& data, bool inc = true);

	void writeblock( component_ids ID, unsigned long len, writer* level);

	unsigned char *Payload(){return dsmccdata.data();};
	unsigned int Len(){return dsmccdata.size();};
protected:
	unsigned int currentpos;
	std::vector<unsigned char> dsmccdata;
};

class UNConfigMessage : public DSMCCdatasection
{
public:
	UNConfigMessage(unsigned char *data, int len):DSMCCdatasection(data,len){};
	virtual ~UNConfigMessage(){};
	bool Write(writer *level);	
	bool Write(char *name, char *ext=NULL)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,"/UNConfigMessage");
	};
};

class UNSessionMessage : public DSMCCdatasection
{
public:
	UNSessionMessage(unsigned char *data, int len):DSMCCdatasection(data,len){};
	virtual ~UNSessionMessage(){};
	bool Write(writer *level);	
	bool Write(char *name, char *ext=NULL)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,"/UNSessionMessage");
	};
};

class DownloadMessage : public DSMCCdatasection
{
public:
	virtual bool CompSubDesc(writer *level);
	virtual void compatibilitydesc(writer *level);
	virtual bool WritePrivData(int len, writer *level);

	DownloadMessage(unsigned char *data, int len):DSMCCdatasection(data,len){};
	virtual ~DownloadMessage(){};
	bool Write(char *name, char *ext=NULL)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,"/DownloadMessage");
	};
};

class DIRequest : public DownloadMessage
{
public:
	DIRequest(unsigned char *data, int len):DownloadMessage(data,len){};
	virtual ~DIRequest(){};

	virtual bool Write(writer *level);	
	bool Write(char *name, char *ext=NULL)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,"/DIRequest");
	};
};

class DII : public DownloadMessage
{
public:
	virtual bool WriteModInfo(int len, writer *level);

	DII(unsigned char *data, int len):DownloadMessage(data,len){};
	virtual ~DII(){};

	virtual bool Write(writer *level);	
	bool Write(char *name, char *ext=NULL)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,"/DII");
	};
};

class DDB : public DownloadMessage
{
public:
	unsigned short ModuleID()
		{
			unsigned char *data = dsmccdata.data();
			int pos =12+data[9];
			
			return (data[pos]<<8) + data[pos+1];
		};

	unsigned char DDBVersion(){
			int pos =12+dsmccdata[9]+2;
			return dsmccdata[pos];
	};
		
	unsigned int maxBlock;

	DDB(unsigned char *data, int len):DownloadMessage(data,len){
		maxBlock = 65535;
	};

	unsigned short BlockNum()
	{
		unsigned char *data = dsmccdata.data();
		int pos =12+data[9] +4;
		return (data[pos] << 8) + data[pos+1];
	};

	virtual unsigned short DSMCCHeaderLen(){return headerAdaptLen() + 12 + 6;};
	virtual ~DDB(){};

	virtual bool Write(writer *level);
	void SetMaxSection(unsigned short value)
	{
		maxBlock = value;
	};
	bool Write(char *name, char *ext=NULL)
	{
		char localext[256];

		if (name == NULL)
			return false;

		sprintf(localext,"/DDB.M%x.V%x.B%.8x", ModuleID(),DDBVersion(),BlockNum());
		return DSMCCdatasection::Write(name,localext);
	};
};

class DDRequest : public DownloadMessage
{
public:
	DDRequest(unsigned char *data, int len):DownloadMessage(data,len){};
	virtual ~DDRequest(){};

	virtual bool Write(writer *level);	
	bool Write(char *name, char *ext=NULL)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,"/DDRequest");
	};
};

class DLCancel : public DownloadMessage
{
public:
	DLCancel(unsigned char *data, int len):DownloadMessage(data,len){};
	virtual ~DLCancel(){};

	virtual bool Write(writer *level);	
	bool Write(char *name, char *ext=NULL)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,"/DLCancel");
	};
};

class DSI : public DownloadMessage
{
public:
	DSI(unsigned char *data, int len):DownloadMessage(data,len){};
	virtual ~DSI(){};

	virtual bool Write(writer *level);	
	bool Write(char *name, char *ext=NULL)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,"/DSI");
	};
};

class ChanChangeMessage : public DSMCCdatasection
{
public:
	ChanChangeMessage(unsigned char *data, int len):DSMCCdatasection(data,len){};
	virtual ~ChanChangeMessage(){};
	bool Write(writer *level);	
	bool Write(char *name, char *ext=NULL)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,"/ChanChangeMessage");
	};
};


class UNPassMessage : public DSMCCdatasection
{
public:
	UNPassMessage(unsigned char *data, int len):DSMCCdatasection(data, len){};
	virtual ~UNPassMessage(){};
	bool Write(writer *level);	
	bool Write(char *name, char *ext=NULL)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,"/UNPassMessage");
	};
};

#endif 
