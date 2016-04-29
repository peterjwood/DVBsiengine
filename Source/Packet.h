// packet.h: interface for the packet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PACKET_H_INCLUDED_)
#define _PACKET_H_INCLUDED_

#include <vector>

class packet
{
public:
	packet();
	virtual ~packet();

	void initialise(void);

	unsigned short	pid(){return ((data[1] & 0x1f) << 8) + data[2];};

	unsigned char 	transport_scram_ctrl;
	bool 		TransportError(){return (data[1] & 0x80) != 0;};
	unsigned char 	cont;
	bool		payload_start(){return (data[1] & 0x40) != 0;};

	bool 		adaptation(){return (data[3] & 32) != 0;};
	bool 		datacontent() {return (data[3] & 16) != 0;};
	bool		isPES;

	unsigned long 	PacketNumber;

	unsigned short	read(::std::vector<unsigned char> *dest, unsigned short len);
	unsigned short	readstart(::std::vector<unsigned char> *dest, unsigned short len);
	bool 		checkdataend();
	bool		dataremaining();
	unsigned short 	datalength();
	short 		getnext(unsigned char s, unsigned char *buf, unsigned long PNum = 0);
	operator unsigned char *()
	{
		return data;
	};
private:
	unsigned char* 	payload();
	bool 		decode_data();

	bool 		new_section;

	unsigned char	size;

	unsigned char	data[204];


	unsigned short 	datalen;
	unsigned short 	prev_datalen;
	unsigned short	readpos;
};

#endif 
