// writer.h: interface for the writer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(WRITER_H__INCLUDED_)
#define WRITER_H__INCLUDED_

#include "ComponentIds.h"
#include <stdio.h>

class writer  
{
public:
	virtual bool init(void) {
		return true;
	};
	writer()
	{
	};
	virtual ~writer(){};

	virtual writer *ProcessData(char* message){return this;};

	virtual writer *write(char *message){return this;};
	virtual writer *write(component_ids id,unsigned int val){return this;};
	virtual writer *write(component_ids id){return this;};
	virtual writer *bindata(component_ids id,unsigned char *data, unsigned int len){return this;};
	virtual writer *chardata(component_ids id,char *data, unsigned int len){return this;};

};

#endif // !defined(WRITER_H__INCLUDED_)
