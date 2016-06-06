#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "filewriter.h"
#include "SITable.h"
#include "Packet.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "generaltools.h"


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


class filterwriter : public filewriter
{
private:
	component_ids lastid;
public:
	virtual bool write(char *message){return this;};
	virtual bool write(component_ids id,unsigned int val){
		if (id == IDS_MODID)
			filewriter::write(id,val);
		if (lastid == IDS_MODID)
		{
			//get the len next and then the version
			filewriter::write(id,val);
			if (id == IDS_SIZE) // expect the MODID then the len then the version
				return this;
		}
		lastid = id;
		return this;
	};
	virtual bool write(component_ids id){return this;};
	virtual bool bindata(component_ids id,unsigned char *data, unsigned int len){return this;};
	virtual bool chardata(component_ids id,char *data, unsigned int len){return this;};

	
	filterwriter(){lastid = IDS_UNKNOWN;};
	~filterwriter(){};
};

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

char filename[1024];
char outfilename[1024];
bool ExcludeDup = false; 

bool multistartpacket=false;

int main (int argc, char *argv[] )
{
	int option;
	bool packetdecode = false;

  signal(SIGSEGV, sig_handler);
  signal(SIGABRT, sig_handler);
  signal(SIGFPE, sig_handler);
  signal(SIGINT, sig_handler);

	filename[0] = 0;
	outfilename[0] = 0;

	while ((option = getopt(argc,argv,"f:xo:m")) != -1)
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
		case 'x':
			ExcludeDup = true;
			printf("Excluding duplicate tables\n");
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

RawSection *Sect;
unsigned char cont;
#define PACKETBUFFERSIZE 1024*1024
unsigned char basebuffer[PACKETBUFFERSIZE];
TableList *TList=NULL;

int ProcessSection(RawSection *Sect,writer *level)
{
	SITable *TMPTable=NULL;

	if ( Sect != NULL )
	{
		if ( Sect->complete() == 1)
		{
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
					TMPTable->Write(level);

					if (!ExcludeDup)
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
	filewriter w;
	writer* level1;
	int f=-1;
	FILE *outfile=NULL;

	// initialise the local structures

	mkfifo(filename,0666);
	f = open(filename,O_RDONLY);

	if (f == -1)
		return -1;

	if (outfilename[0] != 0)
	{
		outfile = fopen(outfilename,"w+");
		if (outfile)
			w.setoutput(outfile);
	}

	basepacketsize = 188;

	
	w.write((char*)"Tables");
	level1 = w.child();
	
	Sect = NULL;
	cont = 0xFF;

	while (!g_abort)
	{
	while (!g_abort && ((amountread = read(f,&basebuffer[bufferpos],(PACKETBUFFERSIZE/basepacketsize)*basepacketsize))>basepacketsize))
	{
		packet p; // define here so that it gets reset when we read from the file
		if (amountread < 0)
			continue;
		leftover = amountread % basepacketsize;

		while(bufferpos <= amountread-basepacketsize)
		{
			if (!p.getnext(basepacketsize,&basebuffer[bufferpos],pnum++))
			{
				bufferpos += basepacketsize;
				continue;
			}
			bufferpos += basepacketsize;

			if ((p.cont == cont)  || !p.datacontent())
			{
				continue;
			}

			cont = p.cont;

			if ( (Sect == NULL) && !p.payload_start())
				continue;


			while (!p.checkdataend())
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
						int processval;
						if ((processval = ProcessSection(Sect,level1)) == 1)
						{
							goto exitpoint;
							//lseek(f,basestartpos,0);
							amountread = basepacketsize;
							delete Sect;
							Sect = NULL;
							break;
						}
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
			}
		}
		bufferpos = 0;
		p.initialise();
	}
	if (!g_abort) usleep(50000);
	}
	if (Sect != NULL)
	{
		delete Sect;
		Sect = NULL;
	}

	if (TList)
	{
		TList->clean(true);
		delete TList;
		TList = NULL;
	}	

exitpoint:
	w.removechild(level1);
	close(f);
	unlink(filename);
	if (outfile)
		fclose(outfile);

	if (Sect != NULL)
	{
		delete Sect;
		Sect = NULL;
	}

	if (TList)
	{
		TList->clean(true);
		delete TList;
	}	

	return 0;
}

