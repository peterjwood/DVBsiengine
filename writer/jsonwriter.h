// writer.h: interface for the writer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(JSONWRITER_H__INCLUDED_)
#define JSONWRITER_H__INCLUDED_
#include "writer.h"
#include <stdio.h>

class jsonwriter  : public writer
{

private:
	FILE *output;
	short indent;
	bool firstitem;
	bool lastitem;
	int lastlistcount;
	bool writetime(component_ids id,unsigned char *data, unsigned int len);
	bool writeduration(component_ids id,unsigned char *data, unsigned int len);
public:
	virtual bool init(void) {
		jsonwriter();
		return true;
	};
	jsonwriter():writer()
	{
		output = stdout;
		indent = 0;
		firstitem=true;
		lastitem=false;
		lastlistcount=-1;
	};
	virtual ~jsonwriter(){};

	void setoutput(FILE *f) {output=f;};

	virtual bool ProcessData(char *message);
	virtual bool write(char *message);
	virtual bool write(component_ids id,unsigned int val);
	virtual bool write(component_ids id);
	virtual bool bindata(component_ids id,unsigned char *data, unsigned int len);
	virtual bool chardata(component_ids id,char *data, unsigned int len);
	virtual writer *child(void);
	virtual void removechild(writer* child);
	virtual void startlist(component_ids id);
	virtual void listitem(void);
	virtual void endlist(void);
	virtual void enditem(void);

};

#endif // !defined(JSONWRITER_H__INCLUDED_)
