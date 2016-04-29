// SISection.cpp: implementation of the SISection class.
//
//////////////////////////////////////////////////////////////////////

#include "SISection.h"
#include "SIdescriptor.h"
#include "PAT.h"
#include "NIT.h"
#include "CAT.h"
#include "ECM.h"
#include "BAT.h"
#include "PMT.h"
#include "EIT.h"
#include "SDT.h"
#include "PES.h"
#include "DSMCC.h"

// the following are for converting RAW sections to known types
// the Allocate function is called by SITable when the section is added to the table
// the allocator allocates a new section of the correct type if this is known 
// these are static functions so can be called at any time

// Flag to determine if the sectallocators array has been initialised
bool SISection::sectallocators_initialised= false;
// Table type is a byte so allow for 256 allocators
sect_allocator SISection::sectallocators[256];

// static so can be called at any time
RawSection *SISection::Allocate(RawSection *sect)
{
	// We need the data in sect to be able to determine the type
	// so if this is NULL it is an error
	if (sect == NULL)
		return NULL;

	// check the allocators have been set up and if not do it now
	if (!sectallocators_initialised)
		initialise_allocators();

	// this will use the Table Type in the section to call the right allocator if one has been set up
	// if sect is a pointer but there is no data then TableType returns 0xFF
	// when calling the allocator pass the current section so that the data in it can be copied out
	if (sectallocators[sect->TableType()] != NULL)
		return (*sectallocators[sect->TableType()])(sect);

	// fall back to an unknown section if the allocator has not been set up
	return (RawSection*)new SISection(sect);
}

// Function to allow override of an allocator id = TableType allocator is a function pointer
void SISection::set_allocator(unsigned char id, sect_allocator allocator)
{
	// Make sure the allocators have been initialised otherwise the new one would be removed in the function above
	if (!sectallocators_initialised)
		initialise_allocators();

	// override the allocator (NULL is valid so no need to check)
	sectallocators[id] = allocator;	
}

// Set up the default allocators
void SISection::initialise_allocators(void)
{
	int i;
 
	sectallocators_initialised= true;

	for (i = 0; i <= 0xFF; i++)
	{
	switch (i)
	{
	case 0x00:
	sectallocators[i] = PAT::Allocate;
	break;

	case 0x01:
	sectallocators[i] = CAT::Allocate;
	break;

	case 0x02:
	sectallocators[i] = PMT::Allocate;
	break;

	case 0x4A:
	sectallocators[i] = BAT::Allocate;
	break;

	case 0x3A:
	case 0x3B:
	case 0x3C:
	case 0x3D:
	case 0x3E:
	sectallocators[i] = DSMCC::Allocate;
	break;

	case 0x40:
	case 0x41:
	sectallocators[i] = NIT::Allocate;
	break;

	case 0x42:
	case 0x46:
	sectallocators[i] = SDT::Allocate;
	break;

	case 0x80:
	case 0x81:
	sectallocators[i] = ECM::Allocate;
	break;

	default:
	if ((( i >= 0x4E) && (i < 0x70)) )
		sectallocators[i] = EIT::Allocate;
	else
		sectallocators[i] = NULL;

	}

	}
}

// Constructor for a new section from a packet pointer
SISection::SISection(packet *p)
{
	// check not null
	if (p)
		PacketAdd(p);
}
// Constructor for a new section from a packet
SISection::SISection(packet& p)
{
	// call PacketAdd with the pointer to p
	PacketAdd(&p);
}

// Is the section finished or was there a CRC error
int SISection::complete()
{
	// check if there is enough data in the section to determine section length
	if (sectiondata.size() < 5)
		return 0;

	// check data is at least the size of the section 
	if (sectiondata.size() >= FullSectionLength()) 
	{
		// if the section syntax flag is set then check CRC32
		if (SectionSyntax())
			if (!CRCvalid())
				return -1;	// CRC failure
		return 1; //every thing OK
	}

	return 0;
}

unsigned short SISection::FullSectionLength()
{
	// check we have enough data to get the length
	if (sectiondata.size() < 3)
		return 0xFFFF;

	// section length is the number of bytes from the end of the field containing the length so add the 3 bytes at the start
	// to get the total number of bytes in the section
	return (unsigned short)((((unsigned short)sectiondata[1] & 15) << 8) + (unsigned short)sectiondata[2] + 3);
}
// Length of the section data without the CRC from the end of the length field
unsigned short SISection::SectionLength()
{
	// check we have enough data to get the length
	if (sectiondata.size() < 3)
		return 0xFFFF;

	// subtract the CRC field from the length
	return (unsigned short)((((unsigned short)sectiondata[1] & 15) << 8) + (unsigned short)sectiondata[2] )- (unsigned short)4;
}

// Function to add a packet to a section 
void SISection::PacketAdd(packet *p)
{
	unsigned short amount_to_copy = 0;

	// When adding data from a packet we have to take care not to read too much since a packet with the payload start flag
	// can have more than one section starting in it
	if (sectiondata.size() == 0)
	{
		//if we have no data then we need to make sure the packet is at the start of a new section
		p->readstart(&sectiondata,3);
		// if there is less data in the packet than we need then just grab the data and go
		if (sectiondata.size() < 3)
			return;
	}
	// check that we have enough and if not then get some more
	if (sectiondata.size() < 3 )
	{
		p->read(&sectiondata,3 - sectiondata.size());

		// check again that we have enough to read the length
		if (sectiondata.size() < 3 )
			return;
	}

	// see how much more we need to add
	amount_to_copy = FullSectionLength() - sectiondata.size();

	// this should never check but just in case
	if (amount_to_copy <= 0)
		return;

	// read as much as we can (sectiondata contains a count of the number of bytes in it)
	p->read(&sectiondata,amount_to_copy);

	// store the PID
	hPID = p->pid();
}
// Length of the payload for the section without the standard fields or CRC
unsigned short SISection::SectionPayloadLength()
{
	// subtract rest of the generic header from the count CRC is already removed by function
	return SectionLength() - 5;
}
// Get a pointer to the section payload for the next level parsing
unsigned char * SISection::SectionPayload()
{
	// return the raw data pointer to the start of the real data in the section
	// the data is dependent on the section type (BAT,EIT...) from this point on
	return &(sectiondata.data()[8]);

}
// CRC calculation array.
static unsigned int crctab[ 256 ] = 
{
  0x00000000l, 0x04C11DB7l, 0x09823B6El, 0x0D4326D9l, 0x130476DCl, 0x17C56B6Bl, 
  0x1A864DB2l, 0x1E475005l, 0x2608EDB8l, 0x22C9F00Fl, 0x2F8AD6D6l, 0x2B4BCB61l, 
  0x350C9B64l, 0x31CD86D3l, 0x3C8EA00Al, 0x384FBDBDl, 0x4C11DB70l, 0x48D0C6C7l, 
  0x4593E01El, 0x4152FDA9l, 0x5F15ADACl, 0x5BD4B01Bl, 0x569796C2l, 0x52568B75l, 
  0x6A1936C8l, 0x6ED82B7Fl, 0x639B0DA6l, 0x675A1011l, 0x791D4014l, 0x7DDC5DA3l, 
  0x709F7B7Al, 0x745E66CDl, 0x9823B6E0l, 0x9CE2AB57l, 0x91A18D8El, 0x95609039l, 
  0x8B27C03Cl, 0x8FE6DD8Bl, 0x82A5FB52l, 0x8664E6E5l, 0xBE2B5B58l, 0xBAEA46EFl, 
  0xB7A96036l, 0xB3687D81l, 0xAD2F2D84l, 0xA9EE3033l, 0xA4AD16EAl, 0xA06C0B5Dl, 
  0xD4326D90l, 0xD0F37027l, 0xDDB056FEl, 0xD9714B49l, 0xC7361B4Cl, 0xC3F706FBl, 
  0xCEB42022l, 0xCA753D95l, 0xF23A8028l, 0xF6FB9D9Fl, 0xFBB8BB46l, 0xFF79A6F1l, 
  0xE13EF6F4l, 0xE5FFEB43l, 0xE8BCCD9Al, 0xEC7DD02Dl, 0x34867077l, 0x30476DC0l, 
  0x3D044B19l, 0x39C556AEl, 0x278206ABl, 0x23431B1Cl, 0x2E003DC5l, 0x2AC12072l, 
  0x128E9DCFl, 0x164F8078l, 0x1B0CA6A1l, 0x1FCDBB16l, 0x018AEB13l, 0x054BF6A4l, 
  0x0808D07Dl, 0x0CC9CDCAl, 0x7897AB07l, 0x7C56B6B0l, 0x71159069l, 0x75D48DDEl, 
  0x6B93DDDBl, 0x6F52C06Cl, 0x6211E6B5l, 0x66D0FB02l, 0x5E9F46BFl, 0x5A5E5B08l, 
  0x571D7DD1l, 0x53DC6066l, 0x4D9B3063l, 0x495A2DD4l, 0x44190B0Dl, 0x40D816BAl, 
  0xACA5C697l, 0xA864DB20l, 0xA527FDF9l, 0xA1E6E04El, 0xBFA1B04Bl, 0xBB60ADFCl, 
  0xB6238B25l, 0xB2E29692l, 0x8AAD2B2Fl, 0x8E6C3698l, 0x832F1041l, 0x87EE0DF6l, 
  0x99A95DF3l, 0x9D684044l, 0x902B669Dl, 0x94EA7B2Al, 0xE0B41DE7l, 0xE4750050l, 
  0xE9362689l, 0xEDF73B3El, 0xF3B06B3Bl, 0xF771768Cl, 0xFA325055l, 0xFEF34DE2l, 
  0xC6BCF05Fl, 0xC27DEDE8l, 0xCF3ECB31l, 0xCBFFD686l, 0xD5B88683l, 0xD1799B34l, 
  0xDC3ABDEDl, 0xD8FBA05Al, 0x690CE0EEl, 0x6DCDFD59l, 0x608EDB80l, 0x644FC637l, 
  0x7A089632l, 0x7EC98B85l, 0x738AAD5Cl, 0x774BB0EBl, 0x4F040D56l, 0x4BC510E1l, 
  0x46863638l, 0x42472B8Fl, 0x5C007B8Al, 0x58C1663Dl, 0x558240E4l, 0x51435D53l, 
  0x251D3B9El, 0x21DC2629l, 0x2C9F00F0l, 0x285E1D47l, 0x36194D42l, 0x32D850F5l, 
  0x3F9B762Cl, 0x3B5A6B9Bl, 0x0315D626l, 0x07D4CB91l, 0x0A97ED48l, 0x0E56F0FFl, 
  0x1011A0FAl, 0x14D0BD4Dl, 0x19939B94l, 0x1D528623l, 0xF12F560El, 0xF5EE4BB9l, 
  0xF8AD6D60l, 0xFC6C70D7l, 0xE22B20D2l, 0xE6EA3D65l, 0xEBA91BBCl, 0xEF68060Bl, 
  0xD727BBB6l, 0xD3E6A601l, 0xDEA580D8l, 0xDA649D6Fl, 0xC423CD6Al, 0xC0E2D0DDl, 
  0xCDA1F604l, 0xC960EBB3l, 0xBD3E8D7El, 0xB9FF90C9l, 0xB4BCB610l, 0xB07DABA7l, 
  0xAE3AFBA2l, 0xAAFBE615l, 0xA7B8C0CCl, 0xA379DD7Bl, 0x9B3660C6l, 0x9FF77D71l, 
  0x92B45BA8l, 0x9675461Fl, 0x8832161Al, 0x8CF30BADl, 0x81B02D74l, 0x857130C3l, 
  0x5D8A9099l, 0x594B8D2El, 0x5408ABF7l, 0x50C9B640l, 0x4E8EE645l, 0x4A4FFBF2l, 
  0x470CDD2Bl, 0x43CDC09Cl, 0x7B827D21l, 0x7F436096l, 0x7200464Fl, 0x76C15BF8l, 
  0x68860BFDl, 0x6C47164Al, 0x61043093l, 0x65C52D24l, 0x119B4BE9l, 0x155A565El, 
  0x18197087l, 0x1CD86D30l, 0x029F3D35l, 0x065E2082l, 0x0B1D065Bl, 0x0FDC1BECl, 
  0x3793A651l, 0x3352BBE6l, 0x3E119D3Fl, 0x3AD08088l, 0x2497D08Dl, 0x2056CD3Al, 
  0x2D15EBE3l, 0x29D4F654l, 0xC5A92679l, 0xC1683BCEl, 0xCC2B1D17l, 0xC8EA00A0l, 
  0xD6AD50A5l, 0xD26C4D12l, 0xDF2F6BCBl, 0xDBEE767Cl, 0xE3A1CBC1l, 0xE760D676l, 
  0xEA23F0AFl, 0xEEE2ED18l, 0xF0A5BD1Dl, 0xF464A0AAl, 0xF9278673l, 0xFDE69BC4l, 
  0x89B8FD09l, 0x8D79E0BEl, 0x803AC667l, 0x84FBDBD0l, 0x9ABC8BD5l, 0x9E7D9662l, 
  0x933EB0BBl, 0x97FFAD0Cl, 0xAFB010B1l, 0xAB710D06l, 0xA6322BDFl, 0xA2F33668l, 
  0xBCB4666Dl, 0xB8757BDAl, 0xB5365D03l, 0xB1F740B4l, 
};

// Calculate a CRC32 on the section
unsigned int SISection::CRC32()
{
	unsigned short i;
	unsigned int crc = 0xFFFFFFFFL;

	for(i=0;i<SectionLength()+3;i++)
	{
		//crc = crctab[sectiondata[i] ^ (crc >> 24)] ^ (crc << 8);
		crc = crctab[((crc >> 24) ^ sectiondata[i]) & 0xff] ^ (crc << 8);

	}
	return (unsigned long)crc;
}

// calculate the CRC and check it against the CRC a the end of the section
bool SISection::CRCvalid()
{

	unsigned short CRCpos;

	CRCpos = SectionLength()+3;
	return CRC32() == (unsigned int)((sectiondata[CRCpos] << 24) + (sectiondata[CRCpos+1] << 16) + (sectiondata[CRCpos+2] << 8) + (sectiondata[CRCpos+3]));
}

// Function to write generic informatin about a section
// Called from derived classes
bool SISection::WriteGeneric(writer *parent)
{
	parent->write(IDS_PID,hPID);

	if (!IsPrivate())
	{
		parent->write(IDS_VER, TableVersion());
		parent->write(IDS_TIDEXT,ID());
		parent->write(IDS_LASTSEC,LastSectionNumber());
	}
	return true;
}

// When all else fails this is used to decode the section
bool SISection::Write(writer *level)
{

	writer *level1 = level->write(IDS_UNTABTYPE,sectiondata[0]);

	// go to the start of the buffer
	finddata(true, 0);
	WriteGeneric(level1);

	level1->write(IDS_CURSEC,SectionNumber());

	level1->write(IDS_LENGTH,SectionLength());

	// dump everything
	writeblock(IDS_DATA,SectionLength(),level1);

	return true;
}

// generic descriptor decode sections call this to decode descriptor loops
unsigned short SISection::DecodeDescriptor(writer *output, unsigned short maxsize)
{
	unsigned char buffer[maxsize];
	SIdescriptor *desc;
	unsigned short retval = 0;

	if (maxsize > SectionPayloadLength() - currentpos)
		maxsize = (unsigned short)(SectionPayloadLength() - currentpos);

	if ((maxsize > 0) && getdata(maxsize,buffer,false))
	{
		desc = SIdescriptor::allocate(buffer);

		if (desc == NULL)
			return retval;

		retval = desc->DescriptorLength() + 2;

		desc->decode(output);

		delete desc;
	}
	return retval;
}


