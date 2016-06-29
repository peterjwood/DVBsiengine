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
		inlist=false;
		invisible=false;
		listcount = 0;
	};
	virtual ~writer(){};

    virtual writer *child(void)=0;
    virtual void removechild(writer *child){};
    virtual bool ProcessData(char* message)=0;
    virtual bool writetitle(char *message){return write(message);};
    virtual bool writetitle(component_ids id){return write(id);};
    virtual bool write(char *message)=0;
    virtual bool write(component_ids id,unsigned int val)=0;
    virtual bool write(component_ids id)=0;
    virtual bool write(component_ids id,component_ids id2){if (write(id)) return write(id2); return false;};
    virtual bool bindata(component_ids id,unsigned char *data, unsigned int len)=0;
    virtual bool chardata(component_ids id,char *data, unsigned int len)=0;
    virtual void startinvisiblelist(void){invisible=true;inlist=true;listcount = 0;};
    virtual void startlist(component_ids id){inlist=true;listcount = 0;};
    virtual void startlist(char *listname){inlist=true;listcount = 0;};
    virtual void listitem(void){listcount++;};
    virtual void endlist(void){inlist=false;};
    virtual void endinvisiblelist(void){inlist=false;invisible=false;};
    virtual void enditem(void)=0;

    int listcount;
    bool inlist;
    bool invisible;
};

#endif // !defined(WRITER_H__INCLUDED_)
