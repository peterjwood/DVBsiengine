// SIdescriptor.h: interface for the SIdescriptor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SIDESCRIPTOR_H_INCLUDED_)
#define _SIDESCRIPTOR_H_INCLUDED_

#include "writer.h"
#include <string.h>

class SIdescriptor;

typedef SIdescriptor *(*desc_allocator)(unsigned char *data);

class SIdescriptor  
{

public:
	static SIdescriptor *allocate(unsigned char *data = NULL);
	static void set_allocator(unsigned char descid, desc_allocator allocator);
	static void initialise_allocators(void);

	virtual bool decode(writer* level);
	unsigned char * descriptordata;
	unsigned char DescriptorType();
	unsigned char DescriptorLength();
	SIdescriptor(unsigned char *data = NULL){descriptordata = data;};
	virtual ~SIdescriptor(){};

};

// Video stream descriptor
class VSdesc : public SIdescriptor
{
public:

	VSdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~VSdesc(){};

	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new VSdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;

		desclevel = level->write(IDS_VSDESC);

		desclevel->write(IDS_MULTIFRATE,descriptordata[2] & 0x80);

		desclevel->write(IDS_FRCODE,(descriptordata[2] >> 3) & 0x0F);

		desclevel->write(IDS_MP1ONLY, (descriptordata[2] & 0x04));

		desclevel->write(IDS_CONPARAMF, descriptordata[2] & 0x02);

		desclevel->write(IDS_STILLPIC, descriptordata[2] & 0x01);

		if (!(descriptordata[2] & 0x04))
		{
			desclevel->write(IDS_PROFANDLEVEL,descriptordata[3]);

			desclevel->write(IDS_CRFORMAT,descriptordata[4] >> 6 );
			
			desclevel->write(IDS_FREXT,(descriptordata[4] >> 5) & 1);

		}
		return true;

	};
};

// Audio Stream descriptor
class ASdesc : public SIdescriptor
{
public:
	ASdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~ASdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new ASdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;

		desclevel = level->write(IDS_ASDESC);

		desclevel->write(IDS_FREEFORM, descriptordata[2] & 0x08);

		desclevel->write(IDS_IDFLAG, descriptordata[2] & 0x04);

		desclevel->write(IDS_LAYER,(descriptordata[2] >> 4) & 0x03);

		desclevel->write(IDS_VARRATE, descriptordata[2] & 0x08);

		return true;

	};
};

// Hierachy Descriptor
class Hiedesc : public SIdescriptor
{
public:
	Hiedesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~Hiedesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new Hiedesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;

		desclevel = level->write(IDS_HIEDESC);

		desclevel->write(IDS_TYPE,descriptordata[2] & 0x0F);

		desclevel->write(IDS_LAYIND,descriptordata[3] & 0x3F);

		desclevel->write(IDS_EMBLAYIND,descriptordata[4] & 0x3F);

		desclevel->write(IDS_HEICHAN,descriptordata[5] & 0x3F);

		return true;

	};
};
// Registration Descriptor
class Regdesc : public SIdescriptor
{
public:
	Regdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~Regdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new Regdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;

		desclevel = level->write(IDS_REGDESC);

		desclevel->write(IDS_FORMATID,(unsigned long)((descriptordata[2] << 24) + (descriptordata[3] << 16) + (descriptordata[4] << 8) + descriptordata[5]));

		desclevel->bindata(IDS_ADDITINFO,&descriptordata[6],descriptordata[1]-4);

		return true;

	};
};

// Data Stream Alignment Descriptor
class DSAlignmentdesc : public SIdescriptor
{
public:
	DSAlignmentdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~DSAlignmentdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new DSAlignmentdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;

		desclevel = level->write(IDS_DSALIGNMENTDESC);

		desclevel->write(IDS_TYPE,descriptordata[2]);
		return true;
	};
};

class TGBdesc : public SIdescriptor
{
public:
	TGBdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~TGBdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new TGBdesc(data);
	}
	bool decode(writer* level)
	{
		unsigned int val;
		writer *desclevel;
		desclevel = level->write(IDS_TBGD);

		val = (descriptordata[2] << 6) + (descriptordata[3] >> 2);
		desclevel->write(IDS_HORSIZ,val);

		val = ((descriptordata[3] & 3) << 8) + (descriptordata[4] >> 4);
		desclevel->write(IDS_VERTSIZ,val);

		desclevel->write(IDS_ASPECT,descriptordata[4] & 0x0F);
		return true;
	};
};


class VidWindesc : public SIdescriptor
{
public:
	VidWindesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~VidWindesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new VidWindesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;// Video Window Descriptor
		desclevel = level->write(IDS_VIDWINDESC);

		desclevel->write(IDS_HORIZOFF,(unsigned short)((descriptordata[2] << 6) + ((descriptordata[3] & 0xFC) >> 2)) );
		desclevel->write(IDS_VERTOFF,(unsigned short)( ((descriptordata[3] & 0x03)<< 12) + (descriptordata[4] << 4) + (descriptordata[5] >> 4)) );
		desclevel->write(IDS_PRIO,(descriptordata[5] & 0x0F));
		return true;
	};
};

class CAdesc : public SIdescriptor
{
public:
	CAdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~CAdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new CAdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;// CA Descriptor
		unsigned short len;
		desclevel = level->write(IDS_CADESC);
		desclevel->write(IDS_CASYSID,(unsigned short)((descriptordata[2] << 8) + descriptordata[3] ) );
		desclevel->write(IDS_CAPID,(unsigned short)(((descriptordata[4] & 0x1F) << 8) + descriptordata[5] ) );
		desclevel->bindata(IDS_CADATA, &descriptordata[6], descriptordata[1] - 4);

		return true;
	};
};

class ISOLangdesc : public SIdescriptor
{
public:
	ISOLangdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~ISOLangdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new ISOLangdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; // ISO language descriptor
		short len;
		desclevel = level->write(IDS_ISOLANGDESC);

		for (len = 0; len < DescriptorLength(); len += 4)
		{
			desclevel->chardata(IDS_ISOCODE,(char*)&descriptordata[len + 2],3);

			desclevel->write(IDS_AUDTYPE,descriptordata[len + 5]);
		}
		return true;
	};
};


class SysClkdesc : public SIdescriptor
{
public:
	SysClkdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~SysClkdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new SysClkdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;// System Clock Descriptor
		desclevel = level->write(IDS_SYSCLKDESC);

		desclevel->write(IDS_EXTCLKREF,descriptordata[2] & 0x08);
		desclevel->write(IDS_CLKACCINT,descriptordata[2] & 0x3F );

		desclevel->write(IDS_CLKACCEXP,descriptordata[3] >> 5 );
		return true;
	};
};

class MxBuffUtildesc : public SIdescriptor
{
public:
	MxBuffUtildesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~MxBuffUtildesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new MxBuffUtildesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;// Multiplex Buffer Utilisation Descriptor
		desclevel = level->write(IDS_MXBUFFUTILDESC);

		desclevel->write(IDS_BNDVALID,descriptordata[2] & 0x08);

		desclevel->write(IDS_LTWOFFLOW,(unsigned short)(((descriptordata[2] & 0x07) << 8) + descriptordata[3] ));
		desclevel->write(IDS_LTWOFFUPP,(unsigned short)(((descriptordata[4] & 0x07) << 8) + descriptordata[5] ));
		return true;
	};
};

class Copydesc : public SIdescriptor
{
public:
	Copydesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~Copydesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new Copydesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;// Copyright Descriptor
		unsigned short len;
		desclevel = level->write(IDS_COPYDESC);
		desclevel->write(IDS_COPYID,(unsigned long)((descriptordata[2] << 24) + (descriptordata[3] << 16) + (descriptordata[4] << 8) + descriptordata[5]) );
		
		desclevel->bindata(IDS_PRIVDAT,&descriptordata[6],descriptordata[1]-4);
		return true;
	};
};


class MaxBRdesc : public SIdescriptor
{
public:
	MaxBRdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~MaxBRdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new MaxBRdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;// Maximum Bitrate Descriptor
		desclevel = level->write(IDS_MAXBRDESC);
		desclevel->write(IDS_BR,(unsigned long)(((descriptordata[2] & 0x3F) << 16) + (descriptordata[3] << 8) + descriptordata[4] ) * 50 );
		return true;
	};
};


class PDIdesc : public SIdescriptor
{
public:
	PDIdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~PDIdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new PDIdesc(data);
	}
	bool decode(writer* level)
	{
		writer *desclevel;// Private Data Indicator Descriptor
		desclevel = level->bindata(IDS_PDIDESC,&descriptordata[2],DescriptorLength() );
		return true;
	};
};


class SmoothBufdesc : public SIdescriptor
{
public:
	SmoothBufdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~SmoothBufdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new SmoothBufdesc(data);
	}
	bool decode(writer* level)
	{
		writer *desclevel;// Smoothing Buffer Descriptor
		level->bindata(IDS_SMOOTHBUFDESC,&descriptordata[2],DescriptorLength());
		return true;
	};
};


class STDdesc : public SIdescriptor
{
public:
	STDdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~STDdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new STDdesc(data);
	}
	bool decode(writer* level)
	{
		writer *desclevel;
		desclevel = level->write(IDS_STDDESC);

		desclevel->write(IDS_LEAKVAL, descriptordata[2] & 1);
		return true;
	};
};


class IBPdesc : public SIdescriptor
{
public:
	IBPdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~IBPdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new IBPdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;// IBP Descriptor
		level->bindata(IDS_IBPDESC,&descriptordata[2],DescriptorLength());
		return true;
	};
};

class CarouselIDdesc : public SIdescriptor
{
public:
	CarouselIDdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~CarouselIDdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new CarouselIDdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;
		level->write(IDS_CAROUSELIDDESC,(descriptordata[2]<<24) + (descriptordata[3]<<16) + (descriptordata[4]<<8) + descriptordata[5]);
		if (DescriptorLength() > 4)
			level->bindata(IDS_PRIVDAT,&descriptordata[6],DescriptorLength()-4);
		return true;
	};
};

class AssociationTagdesc : public SIdescriptor
{
public:
	AssociationTagdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~AssociationTagdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new AssociationTagdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;
		level->bindata(IDS_ASSOCIATIONTAGDESC,&descriptordata[2],DescriptorLength());
		return true;
	};
};


class DefAssTagdesc : public SIdescriptor
{
public:
	DefAssTagdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~DefAssTagdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new DefAssTagdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;
		level->bindata(IDS_DEFASSTAGDESC,&descriptordata[2],DescriptorLength());
		return true;
	};
};

class NetNamedesc : public SIdescriptor
{
public:
	NetNamedesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~NetNamedesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new NetNamedesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; // network name descriptor
		level->chardata(IDS_NETNAME,(char*)&(descriptordata[2]),DescriptorLength());
		return true;
	};
};

class SLdesc : public SIdescriptor
{
public:
	SLdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~SLdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new SLdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; //service list descriptor
		desclevel = level->write(IDS_SLDESC);
		for (unsigned short i = 0; i < DescriptorLength(); i+=3)
		{
			desclevel->write(IDS_SERVICEID,(descriptordata[i+2]<<8)+descriptordata[i+3]);
			desclevel->write(IDS_SERVICETYPE,descriptordata[i+4]);
		}
		return true;
	};
};

class Stuffdesc : public SIdescriptor
{
public:
	Stuffdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~Stuffdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new Stuffdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; // stuffing descriptor
		desclevel = level->write(IDS_STUFFDESC);
		desclevel->write(IDS_LENGTH, DescriptorLength());
		return true;
	};
};

class SatDSdesc : public SIdescriptor
{
public:
	SatDSdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~SatDSdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new SatDSdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; // satellite delivery system descriptor
		desclevel = level->write(IDS_SATDSDESC);
		desclevel->write(IDS_SATFREQ,(descriptordata[2]<<24)+(descriptordata[3]<<16)+(descriptordata[4]<<8)+(descriptordata[5] ));
		desclevel->bindata(IDS_ORBPOS,&descriptordata[6],2);

		desclevel->write(IDS_EAST,(descriptordata[8] & 0x80));

		desclevel->write(IDS_POLAR, descriptordata[8] & 0x60);

		desclevel->write(IDS_MOD, descriptordata[8] & 0x1);

		desclevel->write(IDS_SYMBRATE,(descriptordata[9]<<24)+(descriptordata[10]<<16)+(descriptordata[11]<<8)+(descriptordata[12] & 0xf0));

		desclevel->write(IDS_FECINNER,descriptordata[12] & 0x07);
		return true;
	};
};

class CabDSdesc : public SIdescriptor
{
public:
	CabDSdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~CabDSdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new CabDSdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; // cable delivery system descriptor
		desclevel = level->write(IDS_CABDSDESC);
		desclevel->write(IDS_SATFREQ,(descriptordata[2]<<24)+(descriptordata[3]<<16)+(descriptordata[4]<<8)+(descriptordata[5] ));

		desclevel->write(IDS_FECOUTER,descriptordata[7] & 0x0f);

		desclevel->write(IDS_MOD, descriptordata[8]);

		desclevel->write(IDS_SYMBRATE,(descriptordata[9]<<24)+(descriptordata[10]<<16)+(descriptordata[11]<<8)+(descriptordata[12] & 0xf0));

		desclevel->write(IDS_FECINNER,descriptordata[12] & 0x0f);
		return true;
	};
};

class Servdesc : public SIdescriptor
{
public:
	Servdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~Servdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new Servdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; // service descriptor
		desclevel = level->write(IDS_SERVDESC);

		desclevel->write(IDS_SERVICETYPE2, descriptordata[2]);

		desclevel->chardata(IDS_SERVPROV,(char*)&(descriptordata[4]),descriptordata[3]);

		desclevel->chardata(IDS_SERVNAME,(char*)&(descriptordata[5 + descriptordata[3]]),descriptordata[4 + descriptordata[3]]);

		return true;
	};
};

class CountryAvaildesc : public SIdescriptor
{
public:
	CountryAvaildesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~CountryAvaildesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new CountryAvaildesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; // Country availability descriptor
		short len;
		component_ids id;

		desclevel = level->write(IDS_COUNTRYAVAIDESC);
		if (descriptordata[2] & 0x80)
			id = IDS_AVAIL;
		else
			id = IDS_NOTAVAIL;

		for (len = 1; len < descriptordata[1]; len += 3)
		{
			level->chardata(id,(char*)&(descriptordata[len+2]),3);
		}

		return true;
	};
};

class Linkdesc : public SIdescriptor
{
public:
	Linkdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~Linkdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new Linkdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel,*level2; //Linkage descriptor
		unsigned short i;
		desclevel = level->write(IDS_LINKDESC);

		desclevel->write(IDS_TSID2,(descriptordata[2]<<8) + descriptordata[3]);
		desclevel->write(IDS_ONID2,(descriptordata[4]<<8) + descriptordata[5]);
		desclevel->write(IDS_SERVICEID,(descriptordata[6]<<8)+descriptordata[7]);
		level2 = desclevel->write(IDS_LINKTYPE,descriptordata[8]);

		if ((descriptordata[8] == 0x09)||(descriptordata[8] == 0x90)||(descriptordata[8] == 0x91))
		{
			for (i = 9; i < DescriptorLength();)
			{
				level2->write(IDS_FSOUI,(descriptordata[i]<<16)+(descriptordata[i+1]<<8)+descriptordata[i+2]);
				level2->write(IDS_LENGTH,descriptordata[i+4]);
				i = descriptordata[i+3] +4 + i;
			}
		}
		else
		{
			desclevel->bindata(IDS_PRIVDAT, &descriptordata[11],DescriptorLength()-9);
		}
		return true;
	};
};


class ShortEvdesc : public SIdescriptor
{
public:
	ShortEvdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~ShortEvdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new ShortEvdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;  // Short event descriptor
		desclevel = level->write(IDS_SHORTEVDESC);
		desclevel->chardata(IDS_LANGCODE,(char*)&descriptordata[2],3);
		desclevel->write(IDS_EVNAMELEN,descriptordata[5]);

		desclevel->bindata(IDS_EVNAME,&(descriptordata[6]),descriptordata[5]);

		desclevel->write(IDS_EVTEXTLEN,descriptordata[6 + descriptordata[5]]);

		desclevel->bindata(IDS_EVTEXT,&(descriptordata[7 + descriptordata[5]]),descriptordata[6 + descriptordata[5]]);
		return true;
	};
};


class ExtEventdesc : public SIdescriptor
{
public:
	ExtEventdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~ExtEventdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new ExtEventdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;// Extended event descriptor
		short len = 0;

		desclevel = level->write(IDS_EXTEVENTDESC);
		desclevel->write(IDS_DESCNUMLDNUM,descriptordata[2]);
		desclevel->chardata(IDS_LANGCODE,(char*)&descriptordata[3],3);
		desclevel->write(IDS_LENGTH,descriptordata[6]);

		while (len < descriptordata[6])
		{
			desclevel->chardata(IDS_DATA,(char*)&(descriptordata[8+len]),descriptordata[7+len]);
			len += descriptordata[7+len] + 1;

			desclevel->chardata(IDS_DATA,(char*)&(descriptordata[9+len]),descriptordata[8+len]);
			len += descriptordata[7+len] + 1;
		}
		desclevel->chardata(IDS_DATA,(char*)&(descriptordata[8+descriptordata[6]]),descriptordata[7+descriptordata[6]]);

		return true;
	};
};

class Compondesc : public SIdescriptor
{
public:
	Compondesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~Compondesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new Compondesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;  // Component descriptor
		level->bindata(IDS_COMPONDESC,&descriptordata[2],DescriptorLength());
		return true;
	};
};


class StreamIDdesc : public SIdescriptor
{
public:
	StreamIDdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~StreamIDdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new StreamIDdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; // stream identifier descriptor
		level->write(IDS_STREAMIDDESC,descriptordata[2]);
		return true;
	};
};

class CAIDdesc : public SIdescriptor
{
public:
	CAIDdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~CAIDdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new CAIDdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; // CA ID descriptor
		level->write(IDS_CAIDDESC, (descriptordata[2]<<8) + descriptordata[3]);
		return true;
	};
};

class Contentdesc : public SIdescriptor
{
public:
	Contentdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~Contentdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new Contentdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; // Content descriptor
		level->bindata(IDS_CONTENTDESC,&descriptordata[2],descriptordata[1]);
		return true;
	};
};

class ParRatdesc : public SIdescriptor
{
public:
	ParRatdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~ParRatdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new ParRatdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;  // parental rating
		desclevel = level->write(IDS_PARRATDESC);

		for (short len = 0; len < DescriptorLength(); len += 4)
		{
			desclevel->chardata(IDS_COUNTRYCODE,(char*)&descriptordata[len + 2],3);
			desclevel->write(IDS_RATING,descriptordata[len + 5] + 3);
		}
		return true;
	};
};


class Subdesc : public SIdescriptor
{
public:
	Subdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~Subdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new Subdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; // Subtitiling Descriptor
		short len;
		desclevel = level->write(IDS_SUBDESC);
		for (len = 0; len < DescriptorLength(); len += 8)
		{
			desclevel->chardata(IDS_COUNTRYCODE,(char*)&descriptordata[len + 2], 3);

			desclevel->write(IDS_SUBTYPE,descriptordata[len + 5]);

			desclevel->write(IDS_COMPPAGEID,(descriptordata[len + 6]<<8) +descriptordata[len + 7]);

			desclevel->write(IDS_ANCPAGEID,(descriptordata[len + 8] << 8) +descriptordata[len + 9]);
		}
		return true;
	};
};

class TerrDelivSysdesc : public SIdescriptor
{
public:
	TerrDelivSysdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~TerrDelivSysdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new TerrDelivSysdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;
		desclevel = level->write(IDS_TERRDELIVSYSDESC);

		desclevel->write(IDS_CENTFREQ, ((unsigned long )(descriptordata[2] << 24) + (descriptordata[3] << 16) + (descriptordata[4] << 8) +(descriptordata[5]))*10);

		if ((descriptordata[6] & 0xE0) == 0x20)
			desclevel->write(IDS_7MBAND);
		else if ((descriptordata[6] & 0xE0) == 0)
			desclevel->write(IDS_8MBAND);
		else
			desclevel->write(IDS_RESBAND);

		

		switch (descriptordata[7] & 0xC0)
		{
		case 0x0:
			desclevel->write(IDS_QPSK);
			break;
		case 0x40:
			desclevel->write(IDS_16QAM);
			break;
		case 0x80:
			desclevel->write(IDS_64QAM);
			break;
		default:
			desclevel->write(IDS_INVALID);
			break;
		}

		switch (descriptordata[7] & 0x38)
		{
		case 0x0:
			desclevel->write(IDS_NONHIER);
			break;
		case 0x08:
			desclevel->write(IDS_a1);
			break;
		case 0x10:
			desclevel->write(IDS_a2);
			break;
		case 0x18:
			desclevel->write(IDS_a3);
			break;
		default:
			desclevel->write(IDS_INVALID);
			break;
		}

		desclevel->write(IDS_HPCR,descriptordata[7] & 0x07);
		desclevel->write(IDS_LPCR,(descriptordata[8] & 0xE0));
		desclevel->write(IDS_GUARDINT,descriptordata[8] & 0x18);

		if (descriptordata[8] & 0x06)
			desclevel->write(IDS_TRANS8K);
		else
			desclevel->write(IDS_TRANS2K);

		if (descriptordata[8] & 0x01)
			desclevel->write(IDS_OTHERFREQINUSE);
		else
			desclevel->write(IDS_NOOTHERFREQINUSE);

		return true;
	};
};

class MultiCompdesc : public SIdescriptor
{
public:
	MultiCompdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~MultiCompdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new MultiCompdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; //Multilingual component descriptor
		short len = 0;

		desclevel = level->write(IDS_MULTCOMPDESC);

		desclevel->write(IDS_COMPTAG,descriptordata[2]);

		while (len < descriptordata[1] - 1)
		{
			desclevel->chardata(IDS_LANGCODE,(char*)&descriptordata[len+3],3);
			desclevel->chardata(IDS_DESCRIPTION,(char*)&(descriptordata[len+7]),descriptordata[len+6]);
			len += 4 + descriptordata[len+6];
		}
		return true;
	};
};


class PrivDatSpecdesc : public SIdescriptor
{
public:
	PrivDatSpecdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~PrivDatSpecdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new PrivDatSpecdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;
		desclevel = level->bindata(IDS_PRIVDATSPECDESC,&descriptordata[2],DescriptorLength());
		return true;
	};
};


class FreqListdesc : public SIdescriptor
{
public:
	FreqListdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~FreqListdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new FreqListdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;
		desclevel = level->write(IDS_FREQLISTDESC);

		desclevel->write(IDS_CODETYPE,descriptordata[2] & 0x03);
		for (short i = 3; i< DescriptorLength() -1; i+=4)
		{
			desclevel->write(IDS_CENTFREQ, *((unsigned long *)&(descriptordata[i])));
		}

		return true;
	};
};

class DataBroaddesc : public SIdescriptor
{
public:
	DataBroaddesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~DataBroaddesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new DataBroaddesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel; // Data broadcast descriptor
		short i;
		desclevel = level->write(IDS_DATABROADDESC);
		desclevel->write(IDS_ID, (descriptordata[2]<<8) + descriptordata[3]);
		desclevel->write(IDS_COMPTAG, descriptordata[4]);
		desclevel->write(IDS_SELLEN, descriptordata[5]);

		desclevel->bindata(IDS_SELECTOR,&descriptordata[6],descriptordata[5]);

		desclevel->chardata(IDS_ISOCODE,(char*)&descriptordata[6 + descriptordata[5]],3);

		i = descriptordata[9 + descriptordata[5]];

		if (i)
		{
			desclevel->chardata(IDS_DATA,(char*)&(descriptordata[10 + descriptordata[5]]),i);
		}

		return true;
	};
};


class DataBroadIDdesc : public SIdescriptor
{
public:
	DataBroadIDdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~DataBroadIDdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new DataBroadIDdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;  // data broadcast ID
		short i,j;
		desclevel=level->write(IDS_DATABROADIDDESC, (descriptordata[2]<<8) + descriptordata[3]);
		desclevel->write(IDS_LENGTH, DescriptorLength());

		for (i = 4; i < DescriptorLength();)
		{
			desclevel->bindata(IDS_FSOUI,&descriptordata[i],3);
			i+=3;
			desclevel->write(IDS_UPDATETYPE,descriptordata[i]);
			i++;
			desclevel->write(IDS_UDVERSIONINGDATA,descriptordata[i]);
			i++;

			j = descriptordata[i];
			desclevel->chardata(IDS_SELECTOR,(char*)&descriptordata[i+1],j);
			i+= j + 1;
		}
		return true;
	};
};


class FSTunneldesc : public SIdescriptor
{
public:
	FSTunneldesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~FSTunneldesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new FSTunneldesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;  //free_satellite_tunnelled_data_descriptor
		short i;
		desclevel = level->write(IDS_FSTUNNELDESC);
		desclevel->write(IDS_LENGTH, DescriptorLength());
		for (i = 0; i < DescriptorLength(); i+=2)
		{
			if ((descriptordata[2+i] < 0x0d) && (descriptordata[2+i] != 0x08))
			{
				desclevel->write((component_ids)(IDS_FSEITPF + descriptordata[2+i]), descriptordata[3+i]);
			}
			else
			{
				desclevel->write(IDS_FSRES, (descriptordata[2+i]<<8)+ descriptordata[3+i]);
			}
		}
		return true;
	};
};


class FSAltTunneldesc : public SIdescriptor
{
public:
	FSAltTunneldesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~FSAltTunneldesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new FSAltTunneldesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel;  //free_satellite_alt_tunnelled_data_descriptor
		short i;
		desclevel = level->write(IDS_FSALTTUNNELDESC);
		desclevel->write(IDS_LENGTH, DescriptorLength());
		for (i = 0; i < DescriptorLength(); i++)
		{
			desclevel->write((component_ids)(IDS_FSEITPF2 + descriptordata[2+i]));
		}
		return true;
	};
};

class FSLinkdesc : public SIdescriptor
{
public:
	FSLinkdesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~FSLinkdesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new FSLinkdesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel,*level2;  //free_satellite_linkage_descriptor
		short i,j;
		desclevel = level->write(IDS_FSLINKDESC);
		desclevel->write(IDS_LENGTH, DescriptorLength());
		for (i = 2; i < DescriptorLength();)
		{
			desclevel->write(IDS_TSID2,(descriptordata[i]<<8) + descriptordata[i+1]);
			i+=2;
			desclevel->write(IDS_ONID2,(descriptordata[i]<<8) + descriptordata[i+1]);
			i+=2;
			desclevel->write(IDS_FSSTNDLL, descriptordata[i]);
			j = descriptordata[i];
			i++;
			if (j)
			{
				level2 = desclevel->write(IDS_FSLINKTYPE);
			}
			while (j > 0)
			{
				level2->write((component_ids)(IDS_FSEITPF2+descriptordata[i]));
				i++;
				j--;
			}
			desclevel->write(IDS_SERVICEID, (descriptordata[i]<<8) +  descriptordata[i+1]);
			i+=2;
			desclevel->write(IDS_FSTNDLL, descriptordata[i]);
			j = descriptordata[i];
			i++;
			if (j)
			{
				level2 = desclevel->write(IDS_FSLINKTYPE);
			}
			while (j > 0)
			{
				level2->write((component_ids)(IDS_FSEITPF2+descriptordata[i]));
				i++;
				j--;
			}

		}
		return true;
	};
};


class FSRNamedesc : public SIdescriptor
{
public:
	FSRNamedesc(unsigned char *data = NULL):SIdescriptor(data){};
	virtual ~FSRNamedesc(){};
	static SIdescriptor* allocate(unsigned char *data)
	{
		return (SIdescriptor*) new FSRNamedesc(data);
	}
	 bool decode(writer* level)
	{
		writer *desclevel, *level2;// freesat region_name_descriptor
		short i, j;
		desclevel = level->write(IDS_FSRNAMEDESC);
		desclevel->write(IDS_LENGTH, DescriptorLength());
		for (i = 2; i < DescriptorLength();) 
		{
			level2 = desclevel->write(IDS_FSREGIONID,(descriptordata[i]<<8) + descriptordata[i + 1]);
			i+= 2;
			level2->chardata(IDS_ISOCODE,(char*)&descriptordata[i],3);
			i+= 3;
			level2->write(IDS_LENGTH, descriptordata[i]);
			j = descriptordata[i];
			level2->chardata(IDS_DATA,(char*)&descriptordata[i+1],j);
			i+=j+1;
		}
		return true;
	};
};	
#endif
