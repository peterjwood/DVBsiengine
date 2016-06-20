// SITable.cpp: implementation of the SITable class.
//
//////////////////////////////////////////////////////////////////////

#include "SITable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SITable::SITable(RawSection *sect)
{
	for (int i = 0; i < MAX_SECTIONS; i++)
		SectionList[i] = NULL;

	TableLength = 0;
	numSections = 0;

	if (sect != NULL)
	{
		AddSection(sect);
	}
}

SITable::~SITable()
{
	for (int i = 0; i< MAX_SECTIONS; i++)
	{
		if (SectionList[i] != NULL)
			delete SectionList[i];
	}
}

bool SITable::complete()
{
	if (SectionList[0] == NULL)
		return false;
	// DSMCC breaks the rules so check for rule breaking
	if (SectionList[0]->LastSectionNumber() < 0xFF)
	{
		if(numSections < SectionList[0]->LastSectionNumber())
			return false;
	}
	else
	{
		int i,lastsec;

		// if we have MAX_SECTIONS then return true
		if (numSections == MAX_SECTIONS)
			return true;

		// first the simple check we know that there have to be at least 0xFF sections
		//if (numSections < 0xFF)
		//	return false;

		// Find our last section
		i = MAX_SECTIONS-1;

		while (SectionList[i] == NULL)
			i--;

		// Check that the count and the last found section match so we know we have all the preceeding sections
		if (numSections < i)
			return false;

		// Final check that can be done without more input from another table is that DSMCC final section can be smaller than the rest
		// the rest are all same size so if the last section we have acquired is smaller than the others we have hit the end
		// if they are the same then we have a 1 in 4066 chance of being wrong
		if (SectionList[0]->SectionLength() == SectionList[i]->SectionLength())
			return false;

		// Add code here to allow setting of the max section for DSMCC
	}

	
	return true;
}


bool SITable::AddSection(RawSection *sect)
{
	RawSection *LocalSect = RawSection::Allocate(sect);

	if (LocalSect == NULL)
		return false;
	
	if (LocalSect->SectionNumber() > MAX_SECTIONS)
	{
		fprintf(stderr,"Section Number too big %s\n",__FUNCTION__);
		delete LocalSect;
		return false;
	}

	if (SectionList[LocalSect->SectionNumber()] == NULL)
	{
		SectionList[LocalSect->SectionNumber()] = LocalSect;
		TableLength += LocalSect->SectionPayloadLength();
		numSections++;
		return true;
	}
	else
	{
		// Check for a match which is OK
		if (LocalSect == SectionList[LocalSect->SectionNumber()])
		{
			delete LocalSect;
			return false;
		}
		// oops to get here we must have found a different section with the same number
		//fprintf(stderr,"found a mismatching section %s\n", __FUNCTION__);
	}

	delete LocalSect;
	return false;
}

unsigned short SITable::TableID()
{
	for (int i = 0; i < MAX_SECTIONS; i++)
		if (SectionList[i] != NULL)
			return ((SISection*)SectionList[i])->ID();

	return 0xFFFF;
}

unsigned short SITable::TableType()
{
	for (int i = 0; i < MAX_SECTIONS; i++)
		if (SectionList[i] != NULL)
			return SectionList[i]->TableType();

	return 0xFFFF;
}

unsigned short SITable::Version()
{
	for (int i = 0; i < MAX_SECTIONS; i++)
		if (SectionList[i] != NULL)
			return SectionList[i]->TableVersion();

	return 0xFFFF;
}

bool SITable::current()
{
	for (int i = 0; i < MAX_SECTIONS; i++)
		if (SectionList[i] != NULL)
			return ((SISection*)SectionList[i])->CurrentNext();

	return true;
}

bool SITable::Write(writer* level)
{
	writer *level1;
	int i;

	level->startlist(IDS_SECTION);
	for (i = 0; i < MAX_SECTIONS; i++)
	{
		if (SectionList[i] != NULL)
		{
			level1 = level->child();
			level->listitem();
			SectionList[i]->Write(level1);
			level->removechild(level1);
		}
	}
	level->endlist();
	return true;
}

