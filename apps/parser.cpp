#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "jsonwriter.h"
#include "SITable.h"
#include "Packet.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "generaltools.h"

class TableList
{
private:
	TableList *next;
	SITable *tableptr;
public:
	bool Add(SITable *table)
	{
	TableList *current = this; 
	while (current->next != NULL) 
		current = current->next; 
	current->next = new TableList(table);
	return true;
	}
	TableList *Remove(SITable *table)
	{
		// Make sure that we don't remove the root element
		// if the table is in the root element then fiddle the list
		// This removes the element from the list but does not delete it
		// to finish off the job delete the returned pointer
		TableList *temp;
		if (tableptr == table)
		{
			if (next != NULL)
			{
				SITable *temptable = next->tableptr;
				temp = next;
				next = temp->next;
				temp->next = NULL;
				temp->tableptr = tableptr;
				tableptr = temptable;
				return temp;
			}
			else
				//only one element so return this
				return this;
		}
		temp = this;
		while (temp->next != NULL) 
		{
			if (temp->next->tableptr == table)
			{
				TableList *ret = temp->next;
				temp->next = temp->next->next;
				ret->next = NULL;
				return ret;
			}
			temp = temp->next;
		}
		// Not found so return NULL
		return NULL;
	}
	SITable *Find(RawSection *sect)
	{
		if (*tableptr == sect)
			return tableptr;
		else if (next != NULL)
			return next->Find(sect);
		return NULL;
	}
	TableList *Find(SITable *table)
	{
		if (*tableptr == table)
			return this;
		else if (next != NULL)
			return next->Find(table);
		return NULL;
	}
	int getCount(void)
	{
		int count = 0;
		TableList *current = this; 
		while (current->next != NULL)
		{
			count++;
			current = current->next;
		}
		return count;
	}
	TableList(SITable *table)
	{
		tableptr = table;
		next = NULL;
	}
	~TableList()
	{
	}
	void clean(bool safetodelete=false)
	{
		// we cannot delete the SI Tables since these might not have been allocated.
		if (next != NULL)
		{
			next->clean(safetodelete);
			delete next;
		}
		if (safetodelete)
			delete tableptr;
	}
	void deleteSITable()
	{
		delete tableptr;
	}
};


static int PacketLoadingTask();
static int SectionLoadingTask();

char filename[1024];
char outfilename[1024];
bool activePIDS[8192];
bool activeTables[257];
bool ExcludeDup = false; 
bool PATAdd = false;
bool PMTAdd = false;
bool DSMCCAdd = false;
bool SubsAdd = false;
bool TTxtAdd = false;
bool VidAdd = false;
bool AudAdd = false;
bool dostats = false;
float BitRate = 27000000.0;
int gettingpids = 0;
bool TableSet = false;
bool printallsects= false;
int duration=-1;

bool multistartpacket=false;

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
	bool packetdecode = true;

  signal(SIGSEGV, sig_handler);
  signal(SIGABRT, sig_handler);
  signal(SIGFPE, sig_handler);
  signal(SIGINT, sig_handler);
  signal(SIGPIPE, SIG_IGN);


	memset(activePIDS,0,sizeof(activePIDS));
	memset(activeTables,0,sizeof(activeTables));
	filename[0] = 0;
	outfilename[0] = 0;

	while ((option = getopt(argc,argv,"f:p:xPt:b:o:msT:ASFd:")) != -1)
	{
		switch (option)
		{
		case 'm':
			multistartpacket=true;
			break;
		case 'f':
			strncpy(filename,optarg,1024);
			printf("Filename = %s\n",filename);
		break;
		case 'A':
		{
			printf("All pids selected\n");
			for (int i=0; i<8192; i++)
				activePIDS[i] = true;
		}
		break;
		case 'd':
			duration = strtol(optarg,NULL,0);
			printf ("durarion = %d\n",duration);
		break;
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
		case 'T':
		{
			int Tab = strtol(optarg,NULL,0);
			printf ("Adding Table type 0x%X\n",Tab);
			printf ("arg was %s\n",optarg);
			if(Tab <257)
			{
				TableSet=true;
				activeTables[Tab] = true;
			}
			else
				printf("Invalid value\n");
		}	
		break;
		case 'x':
			ExcludeDup = true;
			printf("Excluding duplicate tables\n");
		break;
		case 't':
		{
			int adds = strtol(optarg,NULL,0);
			PMTAdd = true;
			gettingpids=1;
			if (adds & 1) DSMCCAdd = true;
			if (adds & 2) SubsAdd = true;
			if (adds & 4) TTxtAdd = true;
			//if (adds & 8) VidAdd = true;
			//if (adds & 16) AudAdd = true;
			printf("Adding pids from the PMT \n");
		}
		break;
		case 'P':
			PATAdd = true;
			gettingpids=1;
			printf("Adding pids from the PAT \n");
		break;
		case 'b':
		{
			float br = strtof(optarg,NULL);
			if (br != 0.0)
				BitRate = br;
			printf("Setting bitrate to %f\n",BitRate);
			
		}
		break;
		case 'o':
			strncpy(outfilename,optarg,1024);
			printf("Output Filename = %s\n",outfilename);
		break;
		case 'S':
			packetdecode=false;
		break;
		case 'F':
			printallsects=true;
		break;
		case 's':
			dostats = true;
			printf("Stats\n"); 
			break;
		default:
			fprintf(stderr,"Error unknown option \n");
		break;
		}
	}

	if (!TableSet)
		for (int i = 0; i <257; i++)
			activeTables[i] = true;

	if (filename[0] != 0)
	{
		if (packetdecode)
			return PacketLoadingTask();
		else
			return SectionLoadingTask();
	}
	return 1;
}

bool PidsUsed[8192];
RawSection *Sect[8192];
unsigned char cont[8192];
unsigned int count[8192];
#define PACKETBUFFERSIZE 16*1024*1024
unsigned char basebuffer[PACKETBUFFERSIZE];
TableList *TList=NULL;

int ProcessSection(RawSection *Sect, writer *parent)
{
	SITable *TMPTable=NULL;
	writer *level;

	if ( Sect != NULL )
	{
		if ( Sect->complete() == 1)
		{
                        if (printallsects)
                        {
                            RawSection *tmpsect = SISection::Allocate(Sect);
                            if ((tmpsect != NULL) && (level != NULL))
                            {
				level=parent->child();
				parent->listitem();
                                tmpsect->Write(level);
				parent->removechild(level);
                                delete tmpsect;
                            }
                            return -1;
                        }
			if (TList != NULL)
			{
				TMPTable = TList->Find(Sect);
			}

			if (TMPTable != NULL)
			{

				if (TMPTable->complete() || !TMPTable->AddSection(Sect))
				{
					return -1;
				}
			}
			else
			{
				TMPTable = new SITable(Sect);

				if (TMPTable != NULL)
					if (TList == NULL)
						TList = new TableList(TMPTable);
					else
						TList->Add(TMPTable);
			}
					
			if (TMPTable != NULL)
			{
				if (TMPTable->complete())
				{
					if (gettingpids != 1)
					{
						if (activeTables[TMPTable->TableType()])
						{
							level=parent->child();
							parent->listitem();
							TMPTable->Write(level);
							parent->removechild(level);
						}
					}
					else
					{
						if(TMPTable->getPIDs(activePIDS,PATAdd,PMTAdd,DSMCCAdd,SubsAdd,TTxtAdd,VidAdd,AudAdd))
						{
							return 1;
						}
					}
					if ((!ExcludeDup) && (gettingpids != 1)) // Don't do this when we are gathering pid info
					{
						TableList *TempTlist = TList->Remove(TMPTable);
						if (TempTlist == TList)
						{
							delete TList;
							TList = NULL;
						}
						else
							delete TempTlist;
						delete TMPTable;
						TMPTable = NULL;
					}
				}
			}
			else
			{
				// Something went wrong so delete the section
				return -1;
			}
		}
	}
	return 0;
}


int PacketLoadingTask()
{
	unsigned short basepacketsize,basestartpos=0;
	long amountread=0,leftover;
	unsigned long bufferpos = 0;
	unsigned long pnum = 0;
	int i;
	jsonwriter w;
	writer* level1;
	int f=-1;
	FILE *outfile=NULL;
	time_t start_time, time_now;

	printf("Packetised file decode selected\n");
	
	// initialise the local structures
	f = open(filename,O_RDONLY|O_LARGEFILE);

	if (f == -1)
		return -1;

	if (outfilename[0] != 0)
	{
		outfile = fopen(outfilename,"w+");
		if (outfile)
			w.setoutput(outfile);
	}

	amountread = read(f,basebuffer,188*3);

	basepacketsize = amountread > 188 ? getpacketsize(basebuffer,&basestartpos):188;

printf("Basestartpos = %d\n", basestartpos);
	if (basestartpos) read(f,basebuffer,basestartpos);

	if (basepacketsize == 0xFFFF)
	{
		close(f);
		if (outfile) fclose(outfile);
		return -2;
	}

	w.startlist("Tables");
	
	while (gettingpids > -1)
	{

	for (i = 0; i <8192; i++)
	{
		PidsUsed[i] = false;
		Sect[i] = NULL;
		cont[i] = 0xFF;
		count[i] = 0L;
	}

	if (duration != -1)
		time(&start_time);
		
	while (!g_abort)
	{
	while (!g_abort && ((amountread = read(f,&basebuffer[bufferpos],((PACKETBUFFERSIZE-bufferpos)/basepacketsize)*basepacketsize))>=basepacketsize))
	{
		packet p; // define here so that it gets reset when we read from the file
		if (amountread < 0)
			continue;
		leftover = amountread % basepacketsize;
		bufferpos = 0;

		while(bufferpos <= amountread-basepacketsize)
		{
			if (!p.getnext(basepacketsize,&basebuffer[bufferpos],pnum++))
			{
				bufferpos += basepacketsize;
				printf("error loading packet\n");
				continue;
			}
			bufferpos += basepacketsize;

			if (dostats)
			{
				count[p.pid()]++;

				PidsUsed[p.pid()] = true;
			}

			if (!activePIDS[p.pid()] )
				continue;

			if ((p.cont == cont[p.pid()])  || !p.datacontent())
			{
				printf("cont same? %d, %d or no data in packet\n",p.cont,cont[p.pid()]);
				continue;
			}
			if (p.cont != (cont[p.pid()]+1) %16)
				printf("cont skipped %d, %d\n",p.cont,cont[p.pid()]);

			cont[p.pid()] = p.cont;

			if ( (Sect[p.pid()] == NULL) && !p.payload_start())
				continue;


			while (!p.checkdataend())
			{
				if(!p.dataremaining() && (Sect[p.pid()] != NULL))
				{
					//missed some data for the section somewhere so delete it
					delete Sect[p.pid()];
					Sect[p.pid()] = NULL;
				}
				if ((Sect[p.pid()] == NULL))
				{
					Sect[p.pid()] = RawSection::Allocate(p);
				}
				else
				{
					Sect[p.pid()]->PacketAdd(p);
				}

				if (Sect[p.pid()] != NULL)
				{
					if (Sect[p.pid()]->complete() == 1)
					{
						int processval;
						if ((processval = ProcessSection(Sect[p.pid()], &w)) == 1)
						{
							//goto exitpoint;
							//lseek(f,basestartpos,0);
							//amountread = basepacketsize;
							//delete Sect[p.pid()];
							//Sect[p.pid()] = NULL;
							//break;
						}
						delete Sect[p.pid()];
						Sect[p.pid()] = NULL;
					}
					else if (Sect[p.pid()]->complete() == -1)
					{
						// Checksum error
						delete Sect[p.pid()];
						Sect[p.pid()] = NULL;
					}
				}
			}
		}
		if (leftover)
			memcpy(basebuffer,&basebuffer[bufferpos],leftover);
		bufferpos = leftover;

		p.initialise();
	
		if (duration != -1)
		{
			time(&time_now);
			if (duration < (int)difftime(time_now,start_time))
			{
				g_abort = true;
			}
		}
	}
	if (!g_abort) usleep(50000);
	}
	gettingpids--;
	for (i = 0; i <8192; i++)
	{
		if (Sect[i] != NULL)
		{
			delete Sect[i];
			Sect[i] = NULL;
		}
	}

	if (TList)
	{
		TList->clean(true);
		delete TList;
		TList = NULL;
	}	


	}
exitpoint:
	w.endlist();
	w.enditem();
	if (dostats)
	{
		unsigned long totalpackets = 0L;

		for (i = 0; i <8192; i++)
		{
			totalpackets += count[i];
		}

		w.startlist("PIDs");
		for (i = 0; i <8192; i++)
		{
			char buffer[256];
			float rate;
			if (PidsUsed[i])
			{
				level1 = w.child();
				w.listitem();
				rate = ((float)count[i] / (float)totalpackets) * (float)BitRate;

				sprintf(buffer, "0x%.4X - number of packets %lu - rate %1.8f bps",i,count[i],rate);
				level1->write(buffer);
				w.removechild(level1);
			}
		}
		w.endlist();
		w.enditem();
	}

	close(f);

	if (outfile)
		fclose(outfile);

	for (i = 0; i <8192; i++)
	{
		if (Sect[i] != NULL)
		{
			delete Sect[i];
			Sect[i] = NULL;
		}
	}

	if (TList)
	{
		TList->clean(true);
		delete TList;
	}	

	return 0;
}

int SectionLoadingTask()
{
	unsigned short basepacketsize = 4096,basestartpos = 0;
	unsigned long amountread;
	unsigned long bufferpos = 0;
	SISection *CurrentSect = NULL;

	jsonwriter w;
	writer* level1;
	int f=-1;
	FILE *outfile=NULL;

	printf("Section file decode selected\n");
	gettingpids = 0;
	// initialise the local structures
	f = open(filename,O_RDONLY|O_LARGEFILE);

	if (f == -1)
		return -1;

	if (outfilename[0] != 0)
	{
		outfile = fopen(outfilename,"w+");
		if (outfile)
			w.setoutput(outfile);
	}

	amountread = read(f,basebuffer,512);

	// Check for packets ... should not have any
	basepacketsize = getpacketsize(basebuffer,&basestartpos);

	if (basepacketsize != 0xFFFF)
	{
		close(f);
		if (outfile) fclose(outfile);
		return -2;
	}

	//reset packet size to default section size
	basepacketsize = 4096;


	w.startlist("Tables");
	level1 = w.child();

	lseek(f,basestartpos,0);

	while (((amountread = read(f,basebuffer,(PACKETBUFFERSIZE/basepacketsize)*basepacketsize))>basepacketsize))
	{
		while(bufferpos <= amountread-basepacketsize)
		{
			CurrentSect = new SISection(&basebuffer[bufferpos]);
			bufferpos += CurrentSect->FullSectionLength();;

			if ( CurrentSect->complete())
			{
				ProcessSection(CurrentSect,&w);

				delete CurrentSect;
				CurrentSect = NULL;
			}
		}
		bufferpos = 0;
	}

	close(f);
	if (outfile)
		fclose(outfile);

	if (TList)
	{
		TList->clean(true);
		delete TList;
	}
	w.removechild(level1);
	w.endlist();
	w.enditem();

	return 0;
}

