// NIT.h: interface for the NIT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_NIT_H_INCLUDED_)
#define _NIT_H_INCLUDED_

#include "SISection.h"

class NIT : public SISection  
{
public:
	static RawSection* Allocate(RawSection *sect)
	{
		return (RawSection *)new NIT(sect);
	}
	NIT(RawSection *sect=NULL):SISection(sect) {};
	bool TSLoop(writer* parent);
	void NetworkDescriptorLoop(writer* parent);
	unsigned short NetworkID();
	bool Write(writer* parent);
	virtual ~NIT(){};

};

#endif 
