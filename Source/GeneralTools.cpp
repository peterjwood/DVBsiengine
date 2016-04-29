/* SI decode general tools */

unsigned short getpacketsize(unsigned char *buffer, unsigned short *startpos)
{
	short start = 0, count = 0, packet_size = 0;

	// figure out if the input file is 188 or 204 byte packets
	// read the first few bytes and find the sync bytes
	// find the first sync byte (must occur within the first 204 bytes)
	while ((buffer[count] != 0x47)&& (count < 205)) count++;

	if (count < 204)
	{
		start = count;

		do
		{

			while ( ((count - start) != 188) && ((count - start) != 204) && ((count - start) < 204) )
			{
				// found first packet sync byte so find the next
				count++;
		        while ( (buffer[count] != 0x47)&& (count < start + 205) ) count++;
			}

			if ((count - start) > 204)
			{
				start++;
				while ((buffer[start] != 0x47)&& (start < 204)) start++;
			}

		}while ( (start < 204) && ((count - start) != 188) && ((count - start) != 204) );
	}
	else
		return 0xFFFF;  // cant be a packetised file

	packet_size = count - start;

	if (start > 204 )
		return 0xFFFF; // cant be a packetised file


	if ((packet_size != 188) && (packet_size != 204))
		return 0xFFFF;  // cant be a packetised file

	*startpos = start;

	return packet_size;
}
