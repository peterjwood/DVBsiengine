// PES.h: interface for the PES class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PES_H_INCLUDED_)
#define _PES_H_INCLUDED_

#include "PESSection.h"

class PES : public PESSect  
{
public:
	static RawSection* Allocate(RawSection *sect)
	{
		return (RawSection *)new PES(sect);
	}
	virtual int complete(){ return 1;};
	PES(RawSection *sect=NULL):PESSect(sect){};
	bool Write(writer *parent);
	virtual ~PES(){};
};

#endif
