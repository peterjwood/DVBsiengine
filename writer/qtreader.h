// qtreader.h: interface for the reader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(QTREADER_H__INCLUDED_)
#define QTREADER_H__INCLUDED_
#ifdef USE_QT
#include <QFile>
class qtreaderio : public reader
{
public:
    qtreaderio(QString filename):qf(filename){
        qf.open(QIODevice::ReadOnly);
    }

    ~qtreaderio(){
    }

    virtual unsigned long Len(){return (unsigned long)qf.size();};
    virtual bool getdata(unsigned char *buffer, long len, bool inc=true){

        if (buffer == NULL)
            return false;

        if (qf.read((char*)buffer,len) != len)
            return false;

        if (!inc)
        {
            qf.seek(qf.pos() - len);
        }

        return true;
    };
    virtual bool finddata(bool fromstart, long len){
        if (fromstart)
        {
            return qf.seek(len);
        }
        long newpos = (long)qf.pos() + len;
        return qf.seek(newpos);
    };
    virtual unsigned long CurrentPos(void){return (unsigned long)qf.pos();}

private:
    QFile qf;

};
#endif
#endif

