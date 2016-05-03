// reader.h: interface for the reader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(BUFREADER_H__INCLUDED_)
#define BUFREADER_H__INCLUDED_
#include <vector>
#include <string.h>
#include "reader.h"

class bufreader : public reader
{
public:
	bufreader(unsigned char *buffer, int len)
	{
		currentpos = 0;
		bufdata.insert(bufdata.end(),buffer,buffer+len);
	};
	~bufreader(){};
	virtual unsigned long Len(){return bufdata.size();};
	virtual bool getdata(long len, unsigned char *buffer, bool inc=true)
	{
		if((currentpos + len) > Len())
			return false;

		memcpy(buffer, &bufdata.data()[currentpos], len);
		if (inc)
			currentpos += len;

		return true;
	};
	virtual bool finddata(bool fromstart, long len)
	{
		if (fromstart)
			currentpos = len;
		else
			currentpos += len;
		if (currentpos < Len())
			return true;
		return false;
	};
	virtual unsigned long CurrentPos(void){return currentpos;};
        bool operator ==(bufreader &comp)
        {
            return (comp.bufdata == bufdata);
        }

private:
	unsigned long currentpos;
	std::vector<unsigned char> bufdata;
};
#endif
