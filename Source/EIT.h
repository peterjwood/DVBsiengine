// EIT.h: interface for the EIT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(EIT_H_INCLUDED_)
#define EIT_H_INCLUDED_

#include "SISection.h"

class EIT : public SISection  
{
public:
	static RawSection* Allocate(RawSection *sect)
	{
		return (RawSection *)new EIT(sect);
	}
	EIT(RawSection *sect=NULL):SISection(sect) {};
	virtual ~EIT(){};

	void EventLoop(writer* parent);
	unsigned short ServiceID();
	bool Write(writer* parent);
};

#endif
