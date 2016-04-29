// BAT.h: interface for the BAT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(BAT_H_INCLUDED_)
#define BAT_H_INCLUDED_

#include "SISection.h"

class BAT : public SISection  
{
public:
	static RawSection* Allocate(RawSection *sect)
	{
		return (RawSection *)new BAT(sect);
	}
	BAT(RawSection *sect=NULL):SISection(sect) {};
	bool TSLoop(writer *output);
	void BouquetDescriptorLoop(writer *output);
	bool Write(writer *output);
	virtual ~BAT(){};

};

#endif 
