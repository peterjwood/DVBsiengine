// SITable.h: interface for the SITable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SITABLE_H__F90FB7F1_21FC_11D6_8E5B_0000865F93FD__INCLUDED_)
#define AFX_SITABLE_H__F90FB7F1_21FC_11D6_8E5B_0000865F93FD__INCLUDED_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "SISection.h"
#include "writer.h"

#define MAX_SECTIONS 65536

class SITable  
{
public:
	SITable(RawSection *sect = NULL);
	virtual ~SITable();

	virtual bool complete();
	unsigned short Version();
	unsigned short PID() {return SectionList[0]->PID();};
	bool current();
	unsigned short TableType();
	unsigned short TableID();

	virtual bool getPIDs(bool *List, bool AddPAT, bool AddPMT, bool AddDSMCC=false, bool AddSubs=false, bool AddTTxt=false, bool AddVid=false, bool AddAudio=false)
	{
		bool retval = false;

		for (int i = 0; i < MAX_SECTIONS; i++)
		{
			if (SectionList[i] != NULL)
			{
				if (SectionList[i]->getPIDs(List,AddPAT,AddPMT,AddDSMCC,AddSubs,AddTTxt,AddVid,AddAudio))
					retval = true;
			}
			else
				break;
		}

		return retval;
	};

	virtual bool Write(writer* level);

	virtual bool AddSection(RawSection *sect);

	virtual bool operator==(SITable &comp)
	{
		if ( (TableType() == comp.TableType()) &&
				(TableID() == comp.TableID()) &&
				(Version() == comp.Version()) &&
				(current() == comp.current()) )
				return true;
		return false;
	};

	virtual bool operator==(SITable *comp)
	{
		if ( (TableType() == comp->TableType()) &&
				(TableID() == comp->TableID()) &&
				(Version() == comp->Version()) &&
				(current() == comp->current()) )
				return true;
		return false;
	};
	virtual bool operator==(RawSection *comp)
	{
		if ( (TableType() == ((SISection*)comp)->TableType()) &&
				(TableID() == ((SISection*)comp)->ID()) &&
				(Version() == ((SISection*)comp)->TableVersion()) &&
				(current() == ((SISection*)comp)->CurrentNext()) )
				return true;
		return false;
	};
	virtual bool operator==(RawSection &comp)
	{
		return operator==(&comp);
	};
	virtual void operator=(SITable *toCopy)
	{
		TableLength = toCopy->TableLength;
		numSections = toCopy->numSections;
		for (int i=0; i<MAX_SECTIONS; i++)
			SectionList[i] = toCopy->SectionList[i];
	};
	virtual void operator=(SITable &toCopy)
	{
		TableLength = toCopy.TableLength;
		numSections = toCopy.numSections;
		for (int i=0; i<MAX_SECTIONS; i++)
			SectionList[i] = toCopy.SectionList[i];
	};

private:
	unsigned long TableLength;
	unsigned int numSections;
	RawSection *SectionList[MAX_SECTIONS];

};

#endif // !defined(AFX_SITABLE_H__F90FB7F1_21FC_11D6_8E5B_0000865F93FD__INCLUDED_)
