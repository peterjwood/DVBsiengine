// SISection.h: interface for the SISection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PESSECTION_H_INCLUDED_)
#define _PESSECTION_H_INCLUDED_

#include "RawSection.h"

class PESSect : public RawSection
{
public:
	static RawSection *Allocate(RawSection *sect){return new PESSect(sect);};
	PESSect(packet& p){};
	PESSect(packet *p){};
	PESSect(RawSection *sect=NULL):RawSection(sect){};
	virtual ~PESSect(){};
};

#endif
