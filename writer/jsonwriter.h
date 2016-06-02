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
	writer *writetime(component_ids id,unsigned char *data, unsigned int len);
	writer *writeduration(component_ids id,unsigned char *data, unsigned int len);
public:
	virtual bool init(void) {
		jsonwriter();
		return true;
	};
	jsonwriter()
	{
		output = stdout;
		indent = 0;
	};
	virtual ~jsonwriter(){};

	void setoutput(FILE *f) {output=f;};

	virtual void doindent();
	virtual writer *ProcessData(char *message);
	virtual writer *write(char *message);
	virtual writer *write(component_ids id,unsigned int val);
	virtual writer *write(component_ids id);
	virtual writer *bindata(component_ids id,unsigned char *data, unsigned int len);
	virtual writer *chardata(component_ids id,char *data, unsigned int len);
};

#endif // !defined(JSONWRITER_H__INCLUDED_)
