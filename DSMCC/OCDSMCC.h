// OCDSMCC.h: interface for the Object carousel DSMCC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_OCDSMCC_H_INCLUDED_)
#define _OCDSMCC_H_INCLUDED_

#include "DSMCCbase.h"


struct tap{
    unsigned short id;
    unsigned short use;
    unsigned short asstag;
    unsigned short selectortype;
    unsigned long transid;
    unsigned long timeout;
};

struct modinfo {

    unsigned long mto;
    unsigned long bto;
    unsigned long mbt;
    unsigned char tapscount;
    struct tap *taps;
    bool compressed;
};

struct module {
    unsigned short ModuleID;
    unsigned long size;
    unsigned char ver;

    bool inf_active;

    struct modinfo inf;
};

struct subdesc {
    unsigned char type;
    unsigned char len;
    unsigned char *data;
};

struct desc{
    unsigned char type;
    unsigned char spectype;
    unsigned char specdata[3];
    unsigned short model;
    unsigned short ver;
    unsigned char subcount;
    struct subdesc *descs;
};

struct compat {
    unsigned short desccount;
    struct desc *descriptor;
};

struct OCDIIstruct {
    unsigned long dlid;
    unsigned short blocksize;
    unsigned char windowsize;
    unsigned char ack_period;
    unsigned long dlwindow;
    unsigned long dlscenario;
    struct compat compatibility;
    unsigned short modules;
    struct module *mods;
    unsigned short pdlen;
    unsigned char *pd;
};


struct ObjectLocationstruct{
    unsigned long carouselid;
    unsigned short moduleid;
    unsigned char majVer;
    unsigned char minVer;
    unsigned char keylen;
    unsigned char *key;
};
struct ConnBinderstruct{
    unsigned char tapscount;
    struct tap *Taps;
};


struct taggedprofile{
    unsigned long tag;
    unsigned char byteorder;
    struct ObjectLocationstruct oloc;
    struct ConnBinderstruct conbind;
};

struct IORstruct{
    unsigned long profileidlen;
    unsigned char *ProfileId;

    unsigned long profilecount;

    struct taggedprofile *tags;
};

struct servcontext{
    unsigned long ID;
    unsigned short len;
    unsigned char *context;
};

struct contextstruct{
    unsigned char count;
    struct servcontext *cont;
};

struct ServiceGatewaystruct{
    struct IORstruct ior;

    unsigned char tapscount;
    struct tap *taps;

    struct contextstruct context;

    unsigned short userlen;

    unsigned char *userinfo;

};

struct OCDSIstruct{
    unsigned char serverid[20];

    struct compat compatibility;
    struct ServiceGatewaystruct srg;
};

class DSMCCOCbase: public DSMCCbase
{
public:
    DSMCCOCbase(reader *r):DSMCCbase(r){
    };
    ~DSMCCOCbase(){};

    void compatibilitydesc(writer *level);
    bool parsecompatibilitydesc(struct compat *comp);
    void cleancompatibilitydesc(struct compat *comp);
    void IOR(writer *level);
    bool parseIOR(struct IORstruct *ior);
    void cleanIOR(struct IORstruct *ior);
    void taggedprofiles(writer *level);
    bool parsetaggedprofiles(struct taggedprofile *prof);
    void cleantaggedprofiles(struct taggedprofile *prof);
    void ObjectLocation(writer *level);
    bool parseObjectLocation(struct ObjectLocationstruct *ol);
    void cleanObjectLocation(struct ObjectLocationstruct *ol);
    void ConnBinder(writer *level);
    bool parseConnBinder(struct ConnBinderstruct* conb);
    void cleanConnBinder(struct ConnBinderstruct* conb);
    void Taps(writer *level);
    bool parseTaps(struct tap* tap);
    void cleanTaps(struct tap* tap);
    void context(writer *level);
    bool parsecontext(struct contextstruct *cont);
    void cleancontext(struct contextstruct *cont);
    bool skipcontext();

};

class OCDII : public DSMCCOCbase
{
public:
	OCDII(reader *r):DSMCCOCbase(r){};
	virtual ~OCDII(){};

    virtual bool Write(writer *level);
    bool parse(struct OCDIIstruct *dii);
    void clean(struct OCDIIstruct *dii);

};

struct DirItem{
    char name[256];
    unsigned long kind;
    struct IORstruct ior;

};

struct BIOPDirlist {
    unsigned short count;
    struct DirItem *dirlist;
};

class OCModule : public DSMCCOCbase
{
public:
	OCModule(reader *r):DSMCCOCbase(r){};

	virtual ~OCModule(){};

	virtual bool Write(writer *level);

	void BIOPName(writer *level);
	void srg(writer *level);
	void ste(writer *level);
	void str(writer *level);
	void dir(writer *level);
	void file(writer *level);
	void BIOPmessage(writer *level);

        bool extractDIR(unsigned char *key, unsigned char keylen, struct BIOPDirlist *dir );
        //bool extractFile(unsigned char *key, unsigned char keylen, QFile *dest);
        bool skipmessage();
        //bool extractfile(QFile *dest);
        bool extractdir(struct BIOPDirlist *dest);
        bool extractBIOPName(struct DirItem *dest);
};


class OCDSI : public DSMCCOCbase
{
public:
	OCDSI(reader *r):DSMCCOCbase(r){};
	virtual ~OCDSI(){};

    void ServiceGateway(writer *level);
    bool parseServiceGateway(struct ServiceGatewaystruct* sgw);
    void cleanServiceGateway(struct ServiceGatewaystruct* sgw);
    virtual bool Write(writer *level);
    bool parse(struct OCDSIstruct* dsi);
    void clean(struct OCDSIstruct* dsi);
};

#endif 
