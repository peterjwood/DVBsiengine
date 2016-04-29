// SISection.h: interface for the SISection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SISECTION_H_INCLUDED_)
#define _SISECTION_H_INCLUDED_

#include "RawSection.h"

typedef RawSection *(*sect_allocator)(RawSection *sect);

class SISection  : public RawSection
{
	friend class SITable;

public:
	static RawSection *Allocate(RawSection *sect);
	static RawSection *Allocate(unsigned char *sectdata, int len )
	{
		RawSection temp(sectdata,len);
		return Allocate(&temp);
	};
	static void set_allocator(unsigned char descid, sect_allocator allocator);
	static void initialise_allocators(void);
	static bool sectallocators_initialised;
	static sect_allocator sectallocators[256];


	SISection(unsigned char* buffer,int len=4096):RawSection(buffer,len){};
	SISection(packet& p);
	SISection(packet *p=NULL);
	SISection(RawSection *sect=NULL):RawSection(sect){};
	virtual ~SISection(){};

	virtual void PacketAdd(packet *p);

	virtual unsigned char TableType(){if (sectiondata.size() > 0) return sectiondata[0]; return 0xFF;};
	virtual bool CRCvalid();
	virtual unsigned char * SectionPayload();
	virtual unsigned short SectionPayloadLength();
	virtual unsigned short FullSectionLength();
	virtual unsigned short SectionLength();
	
	virtual unsigned int LastSectionNumber(){
		// max section number for reference
		return (unsigned int) sectiondata[7];
	};
	virtual unsigned int SectionNumber() {
		// current section number
		return (unsigned int) sectiondata[6];
	};
	virtual bool SectionSyntax() {
		// Flag defines type of CRC
		return (sectiondata[1] & 128) != 0;
	};

	unsigned char TableVersion() {
		// version number of the table is in 5 bits (1,2,3,4,5)
		return (sectiondata[5] & 62) >> 1;
	};
	bool CurrentNext(){
		// Field is in bit 0 
		return (sectiondata[5] & 1) == 1;
	};

	unsigned short ID(){
		// 2 bytes for the ID
		return (sectiondata[3] << 8) + sectiondata[4];
	};
	bool IsPrivate(void){
		// top bit indicates the table is private
		return (sectiondata[1] & 0x80 == 0);
	};
	virtual int complete();
	virtual bool WriteGeneric(writer* parent);
	virtual bool Write(writer* level);

	unsigned short DecodeDescriptor(writer *level, unsigned short maxsize);

private:
	unsigned int CRC32();
};

#endif
