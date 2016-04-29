// DSMCC.h: interface for the DSMCC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSMCC_H__FD981445_251A_11D6_8E5F_0000865F93FD__INCLUDED_)
#define AFX_DSMCC_H__FD981445_251A_11D6_8E5F_0000865F93FD__INCLUDED_

#include "SISection.h"

class DSMCC : public SISection  
{
public:
	unsigned short DSMCCType(){return SectionPayload()[1]; };
	unsigned long TransID()
		{
			unsigned char *data = SectionPayload();
			return (data[4] <<24 ) + (data[5] <<16 ) + (data[6] <<8 ) + data[7];
		};
	unsigned short MessID()
		{
			unsigned char *data = SectionPayload();
			return (data[2] <<8 ) + data[3];
		};
	unsigned char headerAdaptLen(){return SectionPayload()[9]; };
	unsigned short messageLen()
		{
			unsigned char *data = SectionPayload();
			return (data[10] << 8) + data[11];
		};
	unsigned short DSMCCHeaderLen(){return headerAdaptLen() + 12;};

	bool WriteHeader(writer *level);

	virtual bool WriteGeneric(writer* parent);

	virtual bool Write(writer *level);

	DSMCC(RawSection *sect=NULL):SISection(sect) {};

	virtual ~DSMCC(){};

	static RawSection *Allocate(RawSection *sect);

	virtual bool operator==(RawSection *comp)
	{
		if (!SISection::operator==(comp) )
				return false;

		unsigned char *data = comp->SectionPayload();
		if ((DSMCCType() == data[1]) &&
			(MessID() == (data[2] <<8 ) + data[3]) &&
			(TransID() == (data[4] <<24 ) + (data[5] <<16 ) + (data[6] <<8 ) + data[7]))
			return true;

		return false;
	};
	virtual bool operator==(RawSection &comp)
	{
		return operator==(&comp);
	};
};

class UNConfigMessage : public DSMCC
{
public:
	UNConfigMessage(RawSection *sect=NULL):DSMCC(sect){};
	virtual ~UNConfigMessage(){};
	bool Write(writer *level);	
};

class UNSessionMessage : public DSMCC
{
public:
	UNSessionMessage(RawSection *sect=NULL):DSMCC(sect){};
	virtual ~UNSessionMessage(){};
	bool Write(writer *level);	
};

class DownloadMessage : public DSMCC
{
public:
	virtual bool CompSubDesc(writer *level);
	virtual void compatibilitydesc(writer *level);
	virtual bool WritePrivData(int len, writer *level);

	DownloadMessage(RawSection *sect=NULL):DSMCC(sect){};
	virtual ~DownloadMessage(){};
};

class DIRequest : public DownloadMessage
{
public:
	DIRequest(RawSection *sect=NULL):DownloadMessage(sect){};
	virtual ~DIRequest(){};

	virtual bool Write(writer *level);	
};

class DII : public DownloadMessage
{
public:
	virtual bool WriteModInfo(int len, writer *level);

	DII(RawSection *sect=NULL):DownloadMessage(sect){};
	virtual ~DII(){};

	virtual bool Write(writer *level);	
};

class DDB : public DownloadMessage
{
public:
	unsigned short ModuleID()
		{
			unsigned char *data = SectionPayload();
			return (data[DSMCCHeaderLen()]<<8) + data[DSMCCHeaderLen()+1];
		};
	unsigned int maxSection;

	DDB(RawSection *sect=NULL):DownloadMessage(sect){
		maxSection = 65535;
	};

	unsigned short BlockNum()
	{
		unsigned char *data = SectionPayload();
		int pos =12+data[9] +4;
		return (data[pos] << 8) + data[pos+1];
	};

	virtual unsigned int SectionNumber()
	{
		return BlockNum();
	};
	virtual unsigned int LastSectionNumber()
	{
		if (SISection::LastSectionNumber() < 0xFF)
			return SISection::LastSectionNumber();
		return maxSection;
	};

	virtual ~DDB(){};

	virtual bool Write(writer *level);
	void SetMaxSection(unsigned short value)
	{
		maxSection = value;
	};

};

class DDRequest : public DownloadMessage
{
public:
	DDRequest(RawSection *sect=NULL):DownloadMessage(sect){};
	virtual ~DDRequest(){};

	virtual bool Write(writer *level);	
};

class DLCancel : public DownloadMessage
{
public:
	DLCancel(RawSection *sect=NULL):DownloadMessage(sect){};
	virtual ~DLCancel(){};

	virtual bool Write(writer *level);	
};

class DSI : public DownloadMessage
{
public:
	DSI(RawSection *sect=NULL):DownloadMessage(sect){};
	virtual ~DSI(){};

	virtual bool Write(writer *level);	
};

class ChanChangeMessage : public DSMCC
{
public:
	ChanChangeMessage(RawSection *sect=NULL):DSMCC(sect){};
	virtual ~ChanChangeMessage(){};
	bool Write(writer *level);	
};


class UNPassMessage : public DSMCC
{
public:
	UNPassMessage(RawSection *sect=NULL):DSMCC(sect){};
	virtual ~UNPassMessage(){};
	bool Write(writer *level);	
};

#endif 
