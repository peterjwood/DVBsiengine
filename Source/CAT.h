// CAT.h: interface for the CAT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAT_H__FD981445_251A_11D6_8E5F_0000865F93FD__INCLUDED_)
#define AFX_CAT_H__FD981445_251A_11D6_8E5F_0000865F93FD__INCLUDED_

#include "SISection.h"

class CAT : public SISection  
{
public:
	static RawSection* Allocate(RawSection *sect)
	{
		return (RawSection *)new CAT(sect);
	}
	CAT(RawSection *sect=NULL):SISection(sect) {};
	bool Write(writer* parent);
	virtual ~CAT(){};

};

#endif // !defined(AFX_CAT_H__FD981445_251A_11D6_8E5F_0000865F93FD__INCLUDED_)
