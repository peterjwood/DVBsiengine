// packet.cpp: implementation of the packet class.
//
//////////////////////////////////////////////////////////////////////

#include "Packet.h"
#include <string.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

packet::packet()
{
	initialise();
}

void packet::initialise()
{
	new_section = true;
	datalen = 0;
	prev_datalen = 0;
	readpos=0;
	isPES=false;
}

packet::~packet()
{
}

short packet::getnext(unsigned char s, unsigned char *buf, unsigned long PNum)
{

	initialise();

	memcpy(data,buf,s);

	size = s;
	PacketNumber = PNum;

	if (decode_data())
		return 1;
	else
	    return 0;
}

unsigned short packet::read(std::vector<unsigned char> *dest, unsigned short len)
{
	unsigned char *data;

	if ((dest == NULL) || (len == 0))
		return 0;
	
	data = payload();

	len = len > (datalength() - readpos) ? (datalength() - readpos):len;

	dest->insert(dest->end(),&data[readpos],&data[readpos+len]);

	readpos += len;

	return len;

}
unsigned short packet::readstart(std::vector<unsigned char> *dest, unsigned short len)
{
	unsigned char *data;

	if ((dest == NULL) || (len == 0))
		return 0;

	// check for payload start flag can only start a section if this flag is present
	if (!payload_start())
	{
		readpos = datalength();
		return 0;
	}

	// force read from new section area of packet
	if (!new_section)
	{
		new_section = true;
		readpos = 0;
	}

	if (datalength() - readpos < len)
	{
		len = datalength() - readpos;
	}

	data = payload();

	dest->insert(dest->end(),&data[readpos],&data[readpos+len]);

	readpos+= len;

	return len;

}

bool packet::checkdataend()
{
	// If we have a payload start marker and we are not looking at that 
	// area then we haven't finished with this packet
	if ( payload_start() && !new_section)
		return false;

	// check read pointer has end of packet
	if (readpos >= datalength())
		return true;

	// if the next table ID is 0xFF it is padding in the rest of the data
	if (payload()[readpos] == 0xFF)
		return true;

	return false;
}

bool packet::dataremaining()
{
	if (!new_section && (readpos == 0))
		return true;

	if ((readpos == 0) && payload_start() && new_section)
		return false;
		
	if ((new_section) &&(readpos < datalength()-1))
		return true;

	return false;
}
unsigned char* packet::payload()
{
	unsigned char *return_val;

	if (new_section)
	{
		if (adaptation())
		{
			if (payload_start())
				return_val = &data[6 + data[5 + data[4]] + data[4]];
			else
				return_val = &data[4 + data[4]];
		}
		else if (payload_start())
			return_val = &data[5 + data[4]];
		else
			return_val = &data[4];
	}
	else
	{
		if (adaptation())
		{
			if (payload_start())
				return_val = &data[6 + data[4]];
			else
				return_val = &data[4 + data[4]];
		}
		else if (payload_start())
			return_val = &data[5];
		else
			return_val = &data[4];
	}

	return return_val;
}

bool packet::decode_data()
{
	if ((data[0] == 0x47) || (data[0] == 0xb8))
	{
		// demod uncorrectable error
		if (TransportError())
			return false;

		if (adaptation() && (data[4] > 183))
		{
			//invalid adaptation len
			return false;
		}

		datalen = adaptation() ? 183 - data[4] : 184;

		new_section = true;
		prev_datalen = 0;
		isPES=false;

		if (payload_start())
		{
			if (adaptation())
			{
				if (data[5 + data[4]] > datalen)
				{
					// invalid length calculation
					return false;
				}
				else
				{
					if (data[5 + data[4]] != 0) 
					{
						new_section = false;
						prev_datalen = data[5 + data[4]];
					}
					else
					{
						if((data[6+ data[4]] == 0)&&(data[7+ data[4]]==1))
							isPES = true;
					}
					datalen -= prev_datalen + 1;
				}
			}
			else
			{
				if (data[4] > datalen)
				{
					// invalid length 
					return false;
				}
				else 
				{
					if (data[4] != 0) 
					{
						new_section = false;
						prev_datalen = data[4];
					}
					else 
					{
						if((data[5] == 0)&&(data[6]==1))
							isPES = true;
					}
					datalen -= prev_datalen + 1;
				}
			}
		}

		cont = data[3] & 0x0F;
	}
	else
	{
		// Invalid sync byte
		return false;
	}
	return true;
}

unsigned short packet::datalength()
{
	if (new_section)
		return datalen;
	else
		return prev_datalen;
}

