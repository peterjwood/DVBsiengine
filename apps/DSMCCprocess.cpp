#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "bufreader.h"
#include "filewriter.h"
#include "DSMCCsection.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "generaltools.h"

static int SectionLoadingTask();

char filename[1024];
char outfilename[1024];
char commandfilename[1024];

int main (int argc, char *argv[] )
{
	int option;

	filename[0] = 0;
	outfilename[0] = 0;
	commandfilename[0] = 0;

	while ((option = getopt(argc,argv,"f:o:c:")) != -1)
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
		case 'c':
			strncpy(commandfilename,optarg,1024);
			printf("Command Filename = %s\n",commandfilename);
		break;
		default:
			fprintf(stderr,"Error unknown option \n");
		break;
		}
	}

	if ((filename[0] != 0) || (commandfilename[0] != 0))
	{
		return SectionLoadingTask();
	}
	return 1;
}

#define PACKETBUFFERSIZE 4096


struct filterrequest {
	char match[16];
	char mask[16];
	char control;
};

struct filterrequest frDSI = { {0x3b,0,0,0,0,0,0,0,0x11,3,0x10,0x06,0,0,0,0},{0xFF,0,0,0,0,0,0,0,0xFF,0xFF,0xFF,0xFF,0,0,0,0}};
struct filterrequest frDII = { {0x3b,0,0,0,0,0,0,0,0x11,3,0x10,0x02,0,0,0,0},{0xFF,0,0,0,0,0,0,0,0xFF,0xFF,0xFF,0xFF,0,0,0,0}};
struct filterrequest frDDB = { {0x3c,0,0,0,0,0,0,0,0x11,3,0x10,0x03,0,0,0,0},{0xFF,0,0,0,0,0,0,0,0xFF,0xFF,0xFF,0xFF,0,0,0,0}};
struct filterrequest frClear = { {0xFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0xFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};


class filterreader : public reader
{
public:
	filterreader(char *infilename, char *cfilename):cf(-1),f(-1),br(NULL)
	{
		if (!mkfifo(infilename,0666))
		{
			if (!mkfifo(cfilename,0666))
			{
				f = open(infilename,O_RDONLY);

				if (f != -1)
				{
					cf = open(cfilename,O_WRONLY);

					if (cf == -1)
					{
						close(f);
						unlink(infilename);
						unlink(cfilename);
					}
				}
				else
				{
					unlink(infilename);
					unlink(cfilename);
				}
			}
			else
			{
				unlink(infilename);
			}
		}
	};
	~filterreader()
	{
		if (br!= NULL) 
			delete br; 
		if (f != -1)
			close(f);
		if (cf != -1)
			close(cf);
	};

	void query(struct filterrequest *fr)
	{
		int amountread;

		if (fr == NULL)
			return;

		write(cf,fr,sizeof(struct filterrequest));

		amountread = read(f,basebuffer,PACKETBUFFERSIZE);
		
		if (amountread > 0)
		{
			br = new bufreader(basebuffer,amountread);
		}
	};
	virtual unsigned long Len(){if (br!= NULL) return br->Len(); return 0;};
	virtual bool getdata(long len, unsigned char *buffer, bool inc=true){if (br!= NULL) return br->getdata(len,buffer,inc);return false;};
	virtual bool finddata(bool fromstart, long len){if (br!= NULL) return br->finddata(fromstart,len);return true;};
	virtual unsigned long CurrentPos(void){if (br!= NULL) return br->CurrentPos();return 0;};
private:
	bufreader *br;
	int cf;
	int f;
	unsigned char basebuffer[PACKETBUFFERSIZE];

};




int SectionLoadingTask()
{
	DSMCCdatasection *Current = NULL;

	filewriter w;
	writer* level1;

	filterreader filter(filename,commandfilename);

	filter.query(&frDSI);

	Current = DSMCCdatasection::Allocate(&filter);

	if (Current != NULL)
	{

		Current->Write(&w);

		delete Current;
	}

	filter.query(&frDII);

	Current = DSMCCdatasection::Allocate(&filter);

	if (Current != NULL)
	{

		Current->Write(&w);

		delete Current;
	}

	filter.query(&frDII);

	Current = DSMCCdatasection::Allocate(&filter);

	if (Current != NULL)
	{

		Current->Write(&w);

		delete Current;
	}
	filter.query(&frDII);

	Current = DSMCCdatasection::Allocate(&filter);

	if (Current != NULL)
	{

		Current->Write(&w);

		delete Current;
	}
	filter.query(&frDII);

	Current = DSMCCdatasection::Allocate(&filter);

	if (Current != NULL)
	{

		Current->Write(&w);

		delete Current;
	}


	unlink(filename);

	unlink(commandfilename);

	return 0;
}

