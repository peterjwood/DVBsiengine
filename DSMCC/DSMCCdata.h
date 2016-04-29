// DSMCC.h: interface for the DSMCC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DSMCCdata_H_INCLUDED_)
#define _DSMCCdata_H_INCLUDED_

#include "DSMCCbase.h"

class DSMCCdata: public DSMCCbase  
{
public:
	virtual bool Write(writer *level);

	DSMCCdata(reader *r):DSMCCbase(r){};

	virtual ~DSMCCdata(){};
};

class UNConfigMessage : public DSMCCdata
{
public:
	UNConfigMessage(reader *r):DSMCCdata(r){};
	virtual ~UNConfigMessage(){};
	bool Write(writer *level);	
};

class UNSessionMessage : public DSMCCdata
{
public:
	UNSessionMessage(reader *r):DSMCCdata(r){};
	virtual ~UNSessionMessage(){};
	bool Write(writer *level);	
};

class DownloadMessage : public DSMCCdata
{
public:
	virtual bool CompSubDesc(writer *level);
	virtual void compatibilitydesc(writer *level);
	virtual bool WritePrivData(int len, writer *level);
	virtual bool Write(writer *level);	

	DownloadMessage(reader *r):DSMCCdata(r){};
	virtual ~DownloadMessage(){};
};

class DIRequest : public DownloadMessage
{
public:
	DIRequest(reader *r):DownloadMessage(r){};
	virtual ~DIRequest(){};

	virtual bool Write(writer *level);	
};

class DII : public DownloadMessage
{
public:
	virtual bool WriteModInfo(int len, writer *level);

	DII(reader *r):DownloadMessage(r){};
	virtual ~DII(){};

	virtual bool Write(writer *level);	
};

class DataModule : public DownloadMessage
{
public:
	DataModule(reader *r):DownloadMessage(r){};

	virtual ~DataModule(){};

	virtual bool Write(writer *level);
};

class DDRequest : public DownloadMessage
{
public:
	DDRequest(reader *r):DownloadMessage(r){};
	virtual ~DDRequest(){};

	virtual bool Write(writer *level);	
};

class DLCancel : public DownloadMessage
{
public:
	DLCancel(reader *r):DownloadMessage(r){};
	virtual ~DLCancel(){};

	virtual bool Write(writer *level);	
};

class DSI : public DownloadMessage
{
public:
	DSI(reader *r):DownloadMessage(r){};
	virtual ~DSI(){};

	virtual bool Write(writer *level);	
};

class ChanChangeMessage : public DSMCCdata
{
public:
	ChanChangeMessage(reader *r):DSMCCdata(r){};
	virtual ~ChanChangeMessage(){};
	bool Write(writer *level);	
};


class UNPassMessage : public DSMCCdata
{
public:
	UNPassMessage(reader *r):DSMCCdata(r){};
	virtual ~UNPassMessage(){};
	bool Write(writer *level);	
};

#endif 
