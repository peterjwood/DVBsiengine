// reader.h: interface for the reader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(READER_H__INCLUDED_)
#define READER_H__INCLUDED_
#include <iostream>
#include <fstream>

class reader
{
public:
	reader(){};
	~reader(){};
	virtual unsigned long Len(){return 0;};
	virtual bool getdata(unsigned char *buffer, long len){return false;};
	virtual bool finddata(bool fromstart, long len){return true;};
};
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
	virtual bool getdata(unsigned char *buffer, long len){
		if (buffer == NULL)
			return false;
		is.read((char*)buffer,len);

		if (is)
			return true;

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

	
private:
	std::ifstream is;
	int len;

};


#endif
