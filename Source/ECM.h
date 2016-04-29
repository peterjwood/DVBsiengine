// CAT.h: interface for the CAT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(ECM_H_INCLUDED_)
#define ECM_H_INCLUDED_

#include "SISection.h"

class ECM : public SISection  
{
public:
	static RawSection* Allocate(RawSection *sect)
	{
		return (RawSection *)new ECM(sect);
	}
	ECM(RawSection *sect=NULL):SISection(sect) {}
	virtual ~ECM(){}
	int complete() {return 1;}
	unsigned int LastSectionNumber(){
		// max section number for reference
		return 0;
	};
	unsigned int SectionNumber() {
		// current section number
		return 0;
	};
	
	bool Write(writer* parent);

};

#endif 
