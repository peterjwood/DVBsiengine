#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "SITable.h"
#include "Packet.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "generaltools.h"
#include "pthread.h"

int PacketLoadingTask();

char filename[1024];
char outfilename[1024];

int of[8192];
int loopcount =1;
bool activePIDS[8192];

volatile sig_atomic_t g_abort           = false;

void sig_handler(int s)
{
  if (s==SIGINT && !g_abort)
  {
     signal(SIGINT, SIG_DFL);
     g_abort = true;
     return;
  }
  signal(SIGABRT, SIG_DFL);
  signal(SIGSEGV, SIG_DFL);
  signal(SIGFPE, SIG_DFL);
}


int main (int argc, char *argv[] )
{
	int option;
	bool packetdecode = false;

  	signal(SIGSEGV, sig_handler);
  	signal(SIGABRT, sig_handler);
  	signal(SIGFPE, sig_handler);
  	signal(SIGINT, sig_handler);
	memset(activePIDS,0,sizeof(activePIDS));

	filename[0] = 0;
	outfilename[0] = 0;

	while ((option = getopt(argc,argv,"p:f:o:l:")) != -1)
	{
		switch (option)
		{
		case 'p':
		{
			int pid = strtol(optarg,NULL,0);
			printf ("Adding pid 0x%X\n",pid);
			printf ("arg was %s\n",optarg);
			if (pid < 8192)
			{
				activePIDS[pid] = true;
			}
			else
				printf("Invalid value\n");
		}	
		break;

		case 'l':
			loopcount = strtol(optarg,NULL,0);
			printf ("Loop %d times through file\n",loopcount);
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
		return PacketLoadingTask();
	}
	return 1;
}

#define PACKETBUFFERSIZE 1024*1024
unsigned char basebuffer[PACKETBUFFERSIZE];
char outname[1024];
int PacketLoadingTask()
{
	unsigned short basepacketsize,basestartpos=0;
	long amountread,leftover;
	unsigned long bufferpos = 0;
	unsigned long pnum = 0;
	int i;
	int f=-1;

	signal(SIGPIPE,SIG_IGN);

	// initialise the local structures
	f = open(filename,O_RDONLY|O_LARGEFILE);

	if (f == -1)
		return -1;

	//amountread = read(f,basebuffer,512);

	basepacketsize = 188;//getpacketsize(basebuffer,&basestartpos);

	if (basepacketsize == 0xFFFF)
	{
		close(f);
		return -2;
	}

	for (i = 0; i <8192; i++)
	{
		of[i] = -1;
		
	}

	
	bufferpos = 0;
	while (!g_abort)
	{
	while (!g_abort && ((amountread = read(f,&basebuffer[bufferpos],(PACKETBUFFERSIZE/basepacketsize)*basepacketsize))>basepacketsize))
	{
		packet p; // define here so that it gets reset when we read from the file
		if (amountread < 0)
			continue;
		leftover = amountread % basepacketsize;

		while(!g_abort && (bufferpos <= amountread-basepacketsize))
		{
			if (!p.getnext(basepacketsize,&basebuffer[bufferpos],pnum++))
			{
				bufferpos += basepacketsize;
				continue;
			}
			bufferpos += basepacketsize;
			if (activePIDS[p.pid()] && of[p.pid()] == -1)
			{
				if (outfilename[0] != 0)
					sprintf(outname, "%s.0x%X", outfilename,p.pid());
				else
					sprintf(outname, "%s.0x%X", filename,p.pid());


					of[p.pid()] = open(outname,O_WRONLY);
			}
			if (of[0x1fff] != -1)
			{
				if(write(of[0x1fff],(unsigned char*)p,basepacketsize) != basepacketsize)
				{
					close(of[0x1fff]);
					of[0x1fff] = -1;
				}
			}
			if ((p.pid()!=0x1fff) && (of[p.pid()] != -1))
			{
				if(write(of[p.pid()],(unsigned char*)p,basepacketsize) != basepacketsize)
				{
					close(of[p.pid()]);
					of[p.pid()] = -1;
				}
			}

		}
		if (leftover)
			memcpy(basebuffer,&basebuffer[bufferpos],leftover);
		bufferpos = leftover;
		
	}
	}
	close(f);

	for (i = 0; i <8192; i++)
	{
		if (of[i] != -1)
		{
			close(of[i]);
		}
	}

	return 0;
}

