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
    virtual bool init(void) =0;
	writer()
	{
	};
	virtual ~writer(){};

    virtual writer *ProcessData(char* message)=0;

    virtual writer *write(char *message)=0;
    virtual writer *write(component_ids id,unsigned int val)=0;
    virtual writer *write(component_ids id)=0;
    virtual writer *bindata(component_ids id,unsigned char *data, unsigned int len)=0;
    virtual writer *chardata(component_ids id,char *data, unsigned int len)=0;

};

#endif // !defined(WRITER_H__INCLUDED_)
