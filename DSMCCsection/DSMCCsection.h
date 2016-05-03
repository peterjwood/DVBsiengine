// DSMCC.h: interface for the DSMCC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DSMCCsection_H_INCLUDED_)
#define _DSMCCsection_H_INCLUDED_

#include <string.h>
#include "writer.h"
#include "reader.h"

class DSMCCdatasection  
{
public:
	enum DSMCCMessageType {
	DSMCC_UNConfigMessage,
	DSMCC_UNSessionMessage,
	DSMCC_DIRequest,
	DSMCC_DII,
	DSMCC_DDB,
	DSMCC_DDRequest,
	DSMCC_DLCancel,
	DSMCC_DSI,
	DSMCC_ChanChangeMessage,
	DSMCC_UNPassMessage,
	DSMCC_UNKnown} ;
	
	virtual DSMCCMessageType getMessageType(void) { return DSMCC_UNKnown;};

	unsigned short DSMCCType(){
		unsigned char acttype;
		dsmccdata->finddata(true,1);
		dsmccdata->getbyte(acttype);
		dsmccdata->finddata(true,currentpos);
		return (unsigned short)acttype;
	 };
	unsigned long TransID() {
		unsigned long data;
		dsmccdata->finddata(true,4);
		dsmccdata->getulong(data);
		dsmccdata->finddata(true,currentpos);
		return data;
		};
	unsigned short MessID()
		{
		unsigned short data; 
		dsmccdata->finddata(true,2);
		dsmccdata->getushort(data);
		dsmccdata->finddata(true,currentpos);
		return data;
		};
	unsigned char headerAdaptLen()
	{
		unsigned char data;
		dsmccdata->finddata(true,9);
		dsmccdata->getbyte(data);
		dsmccdata->finddata(true,currentpos);
		return data; 
	};
	unsigned short messageLen()
		{
		unsigned short data; 
		dsmccdata->finddata(true,10);
		dsmccdata->getushort(data);
		dsmccdata->finddata(true,currentpos);
		return data;
		};
	virtual unsigned short DSMCCHeaderLen(){return headerAdaptLen() + 12;};

	bool WriteHeader(writer *level);

	virtual bool Write(writer *level);
	virtual bool Write(char *name, char *ext=NULL);

	DSMCCdatasection(reader *r);

	virtual ~DSMCCdatasection(){};

	static DSMCCdatasection *Allocate(reader *r);

	virtual bool operator==(DSMCCdatasection *comp)
	{
		unsigned char data[8];
		comp->finddata(true,0);
		comp->getdata(8,data);

		if ((DSMCCType() == data[1]) &&
			(MessID() == (data[2] <<8 ) + data[3]) &&
			(TransID() == (data[4] <<24 ) + (data[5] <<16 ) + (data[6] <<8 ) + data[7]))
			return true;

		return false;
    }

	bool getdata(unsigned long length, unsigned char *buffer, bool inc = false);
	unsigned char *getrawdata(unsigned long length, bool inc=false);
	bool finddata(bool fromstart, long len);

	bool getulong(unsigned long& data, bool inc = true);

	bool getbyte(unsigned char& data, bool inc = true);

	bool getushort(unsigned short& data, bool inc = true);

	bool get12bits(unsigned short& data, bool inc = true);

	void writeblock( component_ids ID, unsigned long len, writer* level);

    unsigned int Len(){return DSMCCdatasection::messageLen()+DSMCCdatasection::DSMCCHeaderLen();}
protected:
	unsigned int currentpos;
	reader *dsmccdata;
};

class UNConfigMessageSect : public DSMCCdatasection
{
public:
    virtual DSMCCMessageType getMessageType(void) { return DSMCC_UNConfigMessage;}
        UNConfigMessageSect(reader *r):DSMCCdatasection(r){}
        virtual ~UNConfigMessageSect(){}
	bool Write(writer *level);	
    bool Write(char *name)
	{
		if (name == NULL)
			return false;

        return DSMCCdatasection::Write(name,(char*)"/UNConfigMessage");
	};
};

class UNSessionMessageSect : public DSMCCdatasection
{
public:
	virtual DSMCCMessageType getMessageType(void) { return DSMCC_UNSessionMessage;};
        UNSessionMessageSect(reader *r):DSMCCdatasection(r){};
        virtual ~UNSessionMessageSect(){};
	bool Write(writer *level);	
    bool Write(char *name)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,(char*)"/UNSessionMessage");
	};
};

class DownloadMessageSect : public DSMCCdatasection
{
public:
	virtual bool CompSubDesc(writer *level);
	virtual void compatibilitydesc(writer *level);
	virtual bool WritePrivData(int len, writer *level);

        DownloadMessageSect(reader *r):DSMCCdatasection(r){};
        virtual ~DownloadMessageSect(){};
};

class DIRequestSect : public DownloadMessageSect
{
public:
	virtual DSMCCMessageType getMessageType(void) { return DSMCC_DIRequest;};
        DIRequestSect(reader *r):DownloadMessageSect(r){};
        virtual ~DIRequestSect(){};

	virtual bool Write(writer *level);	
    bool Write(char *name)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,(char*)"/DIRequest");
	};
};

class DIISect : public DownloadMessageSect
{
public:
	virtual DSMCCMessageType getMessageType(void) { return DSMCC_DII;};
	virtual bool WriteModInfo(int len, writer *level);

        DIISect(reader *r):DownloadMessageSect(r){};
        virtual ~DIISect(){};

	virtual bool Write(writer *level);	
    bool Write(char *name)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,(char*)"/DII");
	};
};

class DDBSect : public DownloadMessageSect
{
public:
	virtual DSMCCMessageType getMessageType(void) { return DSMCC_DDB;};
	unsigned short ModuleID()
		{
		unsigned short data; 
		dsmccdata->finddata(true,DSMCCdatasection::DSMCCHeaderLen());
		dsmccdata->getushort(data);
		dsmccdata->finddata(true,currentpos);
			
		return data;
		};

	unsigned char DDBVersion(){
		unsigned short data; 
		dsmccdata->finddata(true,DSMCCdatasection::DSMCCHeaderLen()+2);
		dsmccdata->getushort(data);
		dsmccdata->finddata(true,currentpos);
			
		return data;
	};
		
	unsigned int maxBlock;

        DDBSect(reader *r):DownloadMessageSect(r){
		maxBlock = 65535;
	};

	unsigned short BlockNum()
	{
		unsigned short data; 
		dsmccdata->finddata(true,DSMCCdatasection::DSMCCHeaderLen()+4);
		dsmccdata->getushort(data);
		dsmccdata->finddata(true,currentpos);
			
		return data;
	};

	virtual unsigned short DSMCCHeaderLen(){return DSMCCdatasection::DSMCCHeaderLen() + 6;};
        virtual ~DDBSect(){};

	virtual bool Write(writer *level);
	void SetMaxSection(unsigned short value)
	{
		maxBlock = value;
	};
    bool Write(char *name)
	{
		char localext[256];

		if (name == NULL)
			return false;

        sprintf(localext,"/DDB.M%x.V%x.B%x", ModuleID(),DDBVersion(),BlockNum());
		return DSMCCdatasection::Write(name,localext);
	};
};

class DDRequestSect : public DownloadMessageSect
{
public:
	virtual DSMCCMessageType getMessageType(void) { return DSMCC_DDRequest;};
        DDRequestSect(reader *r):DownloadMessageSect(r){};
        virtual ~DDRequestSect(){};

	virtual bool Write(writer *level);	
    bool Write(char *name)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,(char*)"/DDRequest");
	};
};

class DLCancelSect : public DownloadMessageSect
{
public:
	virtual DSMCCMessageType getMessageType(void) { return DSMCC_DLCancel;};
        DLCancelSect(reader *r):DownloadMessageSect(r){};
        virtual ~DLCancelSect(){};

	virtual bool Write(writer *level);	
    bool Write(char *name)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,(char*)"/DLCancel");
	};
};

class DSISect : public DownloadMessageSect
{
public:
	virtual DSMCCMessageType getMessageType(void) { return DSMCC_DSI;};
        DSISect(reader *r):DownloadMessageSect(r){};
        virtual ~DSISect(){};

	virtual bool Write(writer *level);	
    bool Write(char *name)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,(char*)"/DSI");
	};
};

class ChanChangeMessageSect : public DSMCCdatasection
{
public:
	virtual DSMCCMessageType getMessageType(void) { return DSMCC_ChanChangeMessage;};
        ChanChangeMessageSect(reader *r):DSMCCdatasection(r){};
        virtual ~ChanChangeMessageSect(){};
	bool Write(writer *level);	
    bool Write(char *name)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,(char*)"/ChanChangeMessage");
	};
};


class UNPassMessageSect : public DSMCCdatasection
{
public:
	virtual DSMCCMessageType getMessageType(void) { return DSMCC_UNPassMessage;};
        UNPassMessageSect(reader *r):DSMCCdatasection(r){};
        virtual ~UNPassMessageSect(){};
	bool Write(writer *level);	
    bool Write(char *name)
	{
		if (name == NULL)
			return false;

		return DSMCCdatasection::Write(name,(char*)"/UNPassMessage");
	};
};

#endif 
