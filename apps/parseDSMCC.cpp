#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "filewriter.h"
#include "DSMCCsection.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "generaltools.h"

static int SectionLoadingTask();

char filename[1024];
char outfilename[1024];

int main (int argc, char *argv[] )
{
	int option;
	bool packetdecode = false;

	filename[0] = 0;
	outfilename[0] = 0;

	while ((option = getopt(argc,argv,"f:o:")) != -1)
	{
		switch (option)
		{
		case 'f':
			strncpy(filename,optarg,1024);
			printf("Filename = %s\n",filename);
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

	if (filename[0] != 0)
	{
		return SectionLoadingTask();
	}
	return 1;
}

#define PACKETBUFFERSIZE 1024*1024
unsigned char basebuffer[PACKETBUFFERSIZE];

int SectionLoadingTask()
{
	unsigned short basepacketsize = 4096,basestartpos = 0;
	long amountread;
	unsigned long bufferpos = 0;
	DSMCCdatasection *Current = NULL;

	filewriter w;
	writer* level1;
	int f=-1;
	FILE *outfile=NULL;

	printf("Section file decode selected\n");
	// initialise the local structures
	f = open(filename,O_RDONLY|O_LARGEFILE);

	if (f == -1)
		return -1;

#if 0
	if (outfilename[0] != 0)
	{
		outfile = fopen(outfilename,"w");
		if (outfile)
			w.setoutput(outfile);
	}
#endif
	basepacketsize = 4096;


	lseek(f,basestartpos,0);

	while ((amountread = read(f,&basebuffer[bufferpos],((PACKETBUFFERSIZE/basepacketsize)*basepacketsize)-bufferpos))>0)
	{
		amountread += bufferpos;
		bufferpos = 0;
		while(bufferpos <= amountread)
		{
			Current = DSMCCdatasection::Allocate(&basebuffer[bufferpos],amountread - bufferpos);
			if (Current == NULL)
				break;

			if (Current->Len() == 0)
			{
				delete Current;
				break;
			}

			bufferpos += Current->Len();

			if (outfilename[0] == 0)
				Current->Write(&w);
			else
				Current->Write(outfilename);

			delete Current;
		}
		if (amountread != bufferpos)
		{
			memcpy(basebuffer, &basebuffer[bufferpos], amountread-bufferpos);
			bufferpos = amountread-bufferpos;
		}
		else
			bufferpos = 0;
	}

	close(f);
	if (outfile)
		fclose(outfile);

	return 0;
}

