// PAT.h: interface for the PAT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PAT_H_INCLUDED_)
#define _PAT_H_INCLUDED_

#include "SISection.h"

class PAT : public SISection  
{
public:
	static RawSection* Allocate(RawSection *sect)
	{
		return (RawSection *)new PAT(sect);
	}
	bool getPIDs(bool *List, bool AddPAT, bool, bool , bool , bool , bool , bool );
	PAT(RawSection *sect=NULL):SISection(sect) {};
	unsigned short TransportStreamID();
	bool Write(writer *parent);
	virtual ~PAT(){};

};

#endif
