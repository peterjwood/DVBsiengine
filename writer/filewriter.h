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
	bool writetime(component_ids id,unsigned char *data, unsigned int len);
	bool writeduration(component_ids id,unsigned char *data, unsigned int len);
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

	virtual void doindent();
	virtual writer *child(void);
	virtual bool ProcessData(char *message);
    	virtual bool writetitle(char *message){return write(message);};
	virtual bool write(char *message);
	virtual bool write(component_ids id,unsigned int val);
	virtual bool write(component_ids id);
	virtual bool bindata(component_ids id,unsigned char *data, unsigned int len);
	virtual bool chardata(component_ids id,char *data, unsigned int len);
	virtual void enditem(void);
};

#endif // !defined(WRITER_H__INCLUDED_)
