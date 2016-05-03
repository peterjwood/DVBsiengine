#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "filewriter.h"
#include "OCDSMCC.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "generaltools.h"
#include <stdlib.h>
#include "readerio.h"

static int SectionLoadingTask();

char filename[1024];
char outfilename[1024];
int type= -1;

int main (int argc, char *argv[] )
{
	int option;

	filename[0] = 0;
	outfilename[0] = 0;

	while ((option = getopt(argc,argv,"t:f:o:")) != -1)
	{
		switch (option)
		{
		case 't':
			type = strtol(optarg,NULL,0);
			break;
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

int SectionLoadingTask()
{
	DSMCCOCbase *Current = NULL;
	readerio dsmccio(filename);

	filewriter w;

	int f=-1;
	FILE *outfile=NULL;

	if (type == -1)
	{
		printf("No type selected\n");
		return -1;
	}

	switch (type)
	{
		case 1:
			Current = new OCDII(&dsmccio);
			break;
		case 2:
			Current = new OCDSI(&dsmccio);
			break;
		case 3:
			Current = new OCModule(&dsmccio);
			break;
		default:
			return -2;
	}

	if (Current == NULL)
		return -3;

	if (outfilename[0] != 0)
	{
		outfile = fopen(outfilename,"w+");
		if (outfile)
			w.setoutput(outfile);
	}

	Current->Write(&w);

	delete Current;

	if (outfile)
		fclose(outfile);

	return 0;
}

