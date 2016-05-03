// reader.h: interface for the reader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(READERIO_H__INCLUDED_)
#define READERIO_H__INCLUDED_
#include <iostream>
#include <fstream>
#include "reader.h"

class readerio : public reader
{
public:
    readerio(char *filename):is(filename,std::ifstream::binary){

        is.seekg(0,is.end);
        len = is.tellg();
        is.seekg(0,is.beg);
    };

    ~readerio(){
    };

    virtual unsigned long Len(){return len;};
    virtual bool getdata(long len, unsigned char *buffer, bool inc=true){

        if (buffer == NULL)
            return false;

        is.read((char*)buffer,len);

        if (is)
        {
            if (!inc) is.seekg(-len,is.cur);
            return true;
        }

        return false;
    };
    virtual bool finddata(bool fromstart, long len){
        if (fromstart)
            is.seekg(len, is.beg);
        else
            is.seekg(len,is.cur);

        if (is)
            return true;
        return false;
        };
    virtual unsigned long CurrentPos(void){return is.tellg();};

private:
    std::ifstream is;
    int len;

};

#endif
