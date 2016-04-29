// writer.h: interface for the writer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(FILEWRITER_H__INCLUDED_)
#define FILEWRITER_H__INCLUDED_
#include "writer.h"
#include <stdio.h>

class filewriter  : public writer
{

private:
	FILE *output;
	short indent;
	void doindent();
	writer *writetime(component_ids id,unsigned char *data, unsigned int len);
	writer *writeduration(component_ids id,unsigned char *data, unsigned int len);
public:
	virtual bool init(void) {
		filewriter();
		return true;
	};
	filewriter()
	{
		output = stdout;
		indent = 0;
	};
	virtual ~filewriter(){};

	void setoutput(FILE *f) {output=f;};

	virtual writer *ProcessData(char *message);
	virtual writer *write(char *message);
	virtual writer *write(component_ids id,unsigned int val);
	virtual writer *write(component_ids id);
	virtual writer *bindata(component_ids id,unsigned char *data, unsigned int len);
	virtual writer *chardata(component_ids id,char *data, unsigned int len);
};

#endif // !defined(WRITER_H__INCLUDED_)
