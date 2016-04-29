// SIdescriptor.cpp: implementation of the SIdescriptor class.
//
//////////////////////////////////////////////////////////////////////

#include "SIdescriptor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static bool allocators_initialised= false;
static desc_allocator allocators[256];

SIdescriptor* SIdescriptor::allocate(unsigned char *data)
{
		
	if (data == NULL)
		return NULL;

	if (!allocators_initialised)
		initialise_allocators();

	if (allocators[data[0]] != NULL)
		return (*allocators[data[0]])(data);

	return new SIdescriptor(data);
}

void SIdescriptor::set_allocator(unsigned char descid, desc_allocator allocator)
{
	if (!allocators_initialised)
		initialise_allocators();

	allocators[descid] = allocator;
}

void SIdescriptor::initialise_allocators(void)
{
	unsigned short i;

	allocators_initialised = true;

	for (i = 0; i <= 0xFF; i++)
	{
	switch (i)
	{
	case 0x02:// Video Stream Descriptor
		allocators[i] = &VSdesc::allocate;
	break;
	case 0x03:// Audio Stream Descriptor
		allocators[i] = &ASdesc::allocate;
	break;
	case 0x04:// Hierachy Descriptor
		allocators[i] = &Hiedesc::allocate;
	break;
	case 0x05:// Registration Descriptor
		allocators[i] = &Regdesc::allocate;
	break;
	case 0x06:// Data Stream Alignment Descriptor
		allocators[i] = &DSAlignmentdesc::allocate;
	break;
	case 0x07: //Target background grid descriptor
		allocators[i] = &TGBdesc::allocate;
	break;

	case 0x08:// Video Window Descriptor
		allocators[i] = &VidWindesc::allocate;
	break;
	case 0x09:// CA Descriptor
		allocators[i] = &CAdesc::allocate;
	break;
	case 0x0A: // ISO language descriptor
		allocators[i] = &ISOLangdesc::allocate;
	break;

	case 0x0B:// System Clock Descriptor
		allocators[i] = &SysClkdesc::allocate;
	break;

	case 0x0C:// Multiplex Buffer Utilisation Descriptor
		allocators[i] = &MxBuffUtildesc::allocate;
	break;

	case 0x0D:// Copyright Descriptor
		allocators[i] = &Copydesc::allocate;
	break;

	case 0x0E:// Maximum Bitrate Descriptor
		allocators[i] = &MaxBRdesc::allocate;
	break;

	case 0x0F:// Private Data Indicator Descriptor
		allocators[i] = &PDIdesc::allocate;
	break;

	case 0x10:// Smoothing Buffer Descriptor
		allocators[i] = &SmoothBufdesc::allocate;
	break;

	case 0x11:
		allocators[i] = &STDdesc::allocate;
	break;

	case 0x12:// IBP Descriptor
		allocators[i] = &IBPdesc::allocate;
	break;

	case 0x13: // Carousel identifier descriptor
		allocators[i] = &CarouselIDdesc::allocate;
	break;

	case 0x14: // association tag descriptor
		allocators[i] = &AssociationTagdesc::allocate;
	break;

	case 0x15: // deferred association tags descriptor
		allocators[i] = &DefAssTagdesc::allocate;
	break;

	case 0x40: // network name descriptor
		allocators[i] = &NetNamedesc::allocate;
	break;

	case 0x41: //service list descriptor
		allocators[i] = &SLdesc::allocate;
	break;

	case 0x42: // stuffing descriptor
		allocators[i] = &Stuffdesc::allocate;
	break;

	case 0x43: // satellite delivery system descriptor
		allocators[i] = &SatDSdesc::allocate;
	break;

	case 0x44: // cable delivery system descriptor
		allocators[i] = &CabDSdesc::allocate;
	break;

	case 0x48: // service descriptor
		allocators[i] = &Servdesc::allocate;
	break;

	case 0x49: // Country availability descriptor
		allocators[i] = &CountryAvaildesc::allocate;
	break;

	case 0x4A: //Linkage descriptor
		allocators[i] = &Linkdesc::allocate;
	break;

	case 0x4D:  // Short event descriptor
		allocators[i] = &ShortEvdesc::allocate;
	break;

	case 0x4E:// Extended event descriptor
		allocators[i] = &ExtEventdesc::allocate;
	break;

	case 0x50:  // Component descriptor
		allocators[i] = &Compondesc::allocate;
	break;

	case 0x52: // stream identifier descriptor
		allocators[i] = &StreamIDdesc::allocate;
	break;

	case 0x53: // CA ID descriptor
		allocators[i] = &CAIDdesc::allocate;
	break;

	case 0x54: // Content descriptor
		allocators[i] = &Contentdesc::allocate;
	break;

	case 0x55:  // parental rating
		allocators[i] = &ParRatdesc::allocate;
	break;

	case 0x59: // Subtitiling Descriptor
		allocators[i] = &Subdesc::allocate;
	break;

	case 0x5A:
		allocators[i] = &TerrDelivSysdesc::allocate;
	break;
	case 0x5E: //Multilingual component descriptor
		allocators[i] = &MultiCompdesc::allocate;
	break;

	case 0x5F:
		allocators[i] = &PrivDatSpecdesc::allocate;
	break;

	case 0x62:
		allocators[i] = &FreqListdesc::allocate;
	break;
	case 0x64: // Data broadcast descriptor
		allocators[i] = &DataBroaddesc::allocate;
	break;

	case 0x66:  // data broadcast ID
		allocators[i] = &DataBroadIDdesc::allocate;
	break;

	case 0xD0:  //free_satellite_tunnelled_data_descriptor
		allocators[i] = &FSTunneldesc::allocate;
	break;

	case 0xD1:  //free_satellite_alt_tunnelled_data_descriptor
		allocators[i] = &FSAltTunneldesc::allocate;
	break;
	case 0xD2:  //free_satellite_linkage_descriptor
		allocators[i] = &FSLinkdesc::allocate;
	break;
	case 0xD4:// freesat region_name_descriptor
		allocators[i] = &FSRNamedesc::allocate;
	break;
	default:
		allocators[i] = NULL;
	break;
	}
	}
}
unsigned char SIdescriptor::DescriptorLength()
{
	if (descriptordata != NULL)
		return descriptordata[1];
	else
		return 0;

}

unsigned char SIdescriptor::DescriptorType()
{

	if (descriptordata != NULL)
		return descriptordata[0];
	else
		return 0xFF;
}

bool SIdescriptor::decode(writer *level)
{
	writer *desclevel;

	desclevel = level->write(IDS_UKNDESC, DescriptorType());

	desclevel->write(IDS_LENGTH, DescriptorLength());
	desclevel->bindata(IDS_DATA,&descriptordata[2],DescriptorLength());

	return true;
}

