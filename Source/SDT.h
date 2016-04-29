// SDT.h: interface for the SDT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SDT_H_INCLUDED_)
#define _SDT_H_INCLUDED_

#include "SISection.h"

class SDT : public SISection  
{
public:
	static RawSection* Allocate(RawSection *sect)
	{
		return (RawSection *)new SDT(sect);
	}
	SDT(RawSection *sect=NULL):SISection(sect) {};
	bool ServiceLoop(writer* parent);
	unsigned short TransportStreamID();
	bool Write(writer* parent);
	virtual ~SDT(){};

};

#endif
