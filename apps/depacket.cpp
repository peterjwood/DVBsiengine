#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "SITable.h"
#include "Packet.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "generaltools.h"
#include "pthread.h"

int PacketLoadingTask();

char filename[1024];
char fullfilename[1024];
char outfilename[1024];
int activePID = -1;

int outfile=-1;

int main (int argc, char *argv[] )
{
	int option;

	filename[0] = 0;
	outfilename[0] = 0;

	while ((option = getopt(argc,argv,"f:p:o:")) != -1)
	{
		switch (option)
		{
		case 'f':
			strncpy(filename,optarg,1024);
			printf("Filename = %s\n",filename);
		break;
		case 'p':
		{
			int pid = strtol(optarg,NULL,0);
			printf ("pid 0x%X\n",pid);
			if (pid < 8192)
			{
				activePID = pid;
			}
			else
				printf("Invalid value\n");
		}	
		break;
		case 'o':
			strncpy(outfilename,optarg,1024);
			printf("Output Filename = %s\n",outfilename);
		break;
		default:
			fprintf(stderr,"Error unknown option \n");
		break;
		}
	}

	if ((filename[0] != 0) && (activePID != -1) && (outfilename[0] != 0))
	{
		return PacketLoadingTask();
	}
	return 1;
}

RawSection *Sect;
unsigned char cont;
unsigned char basebuffer[256];

int ProcessSection(RawSection *Sect)
{
	if ( Sect != NULL )
	{
		if ( Sect->complete() == 1)
		{
			write(outfile,(unsigned char*)(*Sect), Sect->FullSectionLength());
			return 1;
		}
	}
	return 0;
}


int PacketLoadingTask()
{
	unsigned short basepacketsize,basestartpos=0;
	unsigned long amountread;
	unsigned long bufferpos = 0;
	unsigned long pnum = 0;
	int i;
	int f=-1;

	// initialise the local structures
	sprintf(fullfilename, "%s.0x%X", filename,activePID);
	printf("Fifo name %s\n",fullfilename);
	mkfifo(fullfilename,0666);
	f = open(fullfilename,O_RDONLY);

	if (f == -1)
		return -1;

	outfile = open(outfilename,O_RDWR|O_CREAT|O_TRUNC,0666);
	if (outfile == -1)
	{
		close(f);
		unlink(fullfilename);
		return -2;
	}

	basepacketsize = 188;

	cont = 0xFF;
	Sect = NULL;

	while (amountread = read(f,basebuffer,basepacketsize))
	{
		packet p; // define here so that it gets reset when we read from the file

		if (!p.getnext(basepacketsize,basebuffer,pnum++))
		{
			continue;
		}

		if ((p.cont == cont)  || !p.datacontent())
		{
			continue;
		}

		cont = p.cont;

		if ( (Sect == NULL) && !p.payload_start())
			continue;

		do
		{
			if(!p.dataremaining() && (Sect != NULL))
			{
				//missed some data for the section somewhere so delete it
				delete Sect;
				Sect = NULL;
			}
			if ((Sect == NULL))
			{
				Sect = RawSection::Allocate(p);
			}
			else
			{
				Sect->PacketAdd(p);
			}

			if (Sect != NULL)
			{
				if (Sect->complete() == 1)
				{
					ProcessSection(Sect);
					delete Sect;
					Sect = NULL;
				}
				else if (Sect->complete() == -1)
				{
					// Checksum error
					delete Sect;
					Sect = NULL;
				}
			}
		}while (!p.checkdataend());
	}

	close(f);
	close(outfile);
	unlink(fullfilename);
	if (Sect != NULL)
	{
		delete Sect;
	}

	return 0;
}

