// OCDSMCC.h: interface for the Object carousel DSMCC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_OCDSMCC_H_INCLUDED_)
#define _OCDSMCC_H_INCLUDED_

#include "DSMCCbase.h"

class DSMCCOCbase: public DSMCCbase
{
public:
	DSMCCOCbase(reader *r):DSMCCbase(r){
	};
	~DSMCCOCbase(){};

	virtual void compatibilitydesc(writer *level);
	void IOR(writer *level);
	void taggedprofiles(writer *level);
	void ObjectLocation(writer *level);
	void ConnBinder(writer *level);
	void Taps(writer *level);
	void context(writer *level);

};

class OCDII : public DSMCCOCbase
{
public:
	OCDII(reader *r):DSMCCOCbase(r){};
	virtual ~OCDII(){};

	virtual bool Write(writer *level);	
};


class OCModule : public DSMCCOCbase
{
public:
	OCModule(reader *r):DSMCCOCbase(r){};

	virtual ~OCModule(){};

	virtual bool Write(writer *level);

	void BIOPName(writer *level);
	void srg(writer *level);
	void ste(writer *level);
	void str(writer *level);
	void dir(writer *level);
	void file(writer *level);
	void BIOPmessage(writer *level);
};


class OCDSI : public DSMCCOCbase
{
public:
	OCDSI(reader *r):DSMCCOCbase(r){};
	virtual ~OCDSI(){};

	void ServiceGateway(writer *level);
	virtual bool Write(writer *level);	
};

#endif 
