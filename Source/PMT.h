// PMT.h: interface for the PMT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PMT_H_INCLUDED_)
#define _PMT_H_INCLUDED_

#include "SISection.h"

class PMT : public SISection  
{
public:
	static RawSection* Allocate(RawSection *sect)
	{
		return (RawSection *)new PMT(sect);
	}

	bool getPIDs(bool *List, bool AddPAT, bool AddPMT, bool AddDSMCC, bool AddSubs, bool AddTTxt, bool AddVid, bool AddAudio);
	PMT(RawSection *sect=NULL):SISection(sect) {};
	virtual ~PMT(){};

	bool StreamLoop(writer* parent);
	void ProgramInfoLoop(writer* parent);
	unsigned short ProgramNumber();
	bool Write(writer* parent);
};

#endif
