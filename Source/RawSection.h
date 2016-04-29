// SISection.h: interface for the SISection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_RAWSECTION_H_INCLUDED_)
#define _RAWSECTION_H_INCLUDED_

#include <vector>
#include "Packet.h"
#include <string.h>
#include "writer.h"

class RawSection
{
public:
	RawSection(RawSection* sect=NULL) 
	{
		if (sect != NULL)
		{
			// copy data from provided section
			sectiondata = sect->sectiondata;
			hPID = sect->hPID;
		}
	};
	RawSection(unsigned char *buffer,int len);
	virtual ~RawSection()
	{
	}

	static RawSection* Allocate(unsigned char *buffer,int len);
	static RawSection *Allocate(packet *p);
	static RawSection *Allocate(packet &p) {Allocate(&p);};

	// Used for allocating the correct section type from the RAW section passed in
	// called by SITable class when creating the table
	static RawSection *Allocate(RawSection *sect);

	virtual void PacketAdd(packet& p){PacketAdd(&p);};
	virtual void PacketAdd(packet *p){};

	
	virtual bool CRCvalid(){return true;};

	virtual unsigned char *SectionPayload(){return NULL;};
	virtual unsigned short SectionPayloadLength(){return 0;};
	virtual unsigned short FullSectionLength(){return sectiondata.size();};
	virtual unsigned short SectionLength(){return 0;};
	virtual unsigned int LastSectionNumber(){return 0;};
	virtual unsigned int SectionNumber(){return 0;};
	virtual int complete(){return 0;};
	virtual unsigned char TableType(){return 0xFF;};
	virtual unsigned char TableVersion(){return 0;}; 

	bool filtermatch(unsigned char *match, unsigned char *mask, int len)
	{
		for (int i = 0; i < len; i++)
		{
			if ((mask[i] & match[i]) != (mask[i] & sectiondata[i])) 
			{
				return false;
			}
		}
		return true;
	};

	operator unsigned char *()
	{
		if (sectiondata.size() > 0)
			return sectiondata.data();

		return NULL;
	};

	virtual bool operator==(RawSection *comp)
	{
		return memcmp(sectiondata.data(),comp->sectiondata.data(),6) == 0;
	};

	unsigned short PID(){return hPID;};

	virtual bool getPIDs(bool *List, bool AddPAT, bool AddPMT, bool AddDSMCC=false, bool AddSubs=false, bool AddTTxt=false, bool AddVid=false, bool AddAudio=false){return false;}

	virtual bool Write(writer* level);

	bool getdata(unsigned long length, unsigned char *buffer, bool inc = false);
	unsigned char *getrawdata(unsigned long length, bool inc=false);
	bool finddata(bool fromstart, long len);

	bool getulong(unsigned long& data, bool inc = true);

	bool getbyte(unsigned char& data, bool inc = true);

	bool getushort(unsigned short& data, bool inc = true);

	bool get12bits(unsigned short& data, bool inc = true);

	void writeblock( component_ids ID, unsigned long len, writer* level);

protected:
	unsigned int currentpos;
	unsigned short hPID;
	std::vector<unsigned char> sectiondata;
};

#endif
