#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <zlib.h>

char filename[1024];
char outfilename[1024];
#define BUFFERSIZE 1024*1024
unsigned char basebuffer[BUFFERSIZE];
unsigned char inflatebuffer[BUFFERSIZE];

int main (int argc, char *argv[] )
{
	int option;
	long amountread;
	int f=-1;
	int outfile=-1;
	filename[0] = 0;
	outfilename[0] = 0;
	z_stream d_stream; /* decompression stream */
	int err;

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

	if ((filename[0] == 0) || (outfilename[0] == 0))
		return 1;

	f = open(filename,O_RDONLY|O_LARGEFILE);

	if (f == -1)
		return -1;

	outfile = open(outfilename,O_WRONLY|O_LARGEFILE|O_TRUNC|O_CREAT,0666);

	if (outfile == -1)
	{
		close(f);
		return -1;
	}

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	err = inflateInit(&d_stream);

	while ((amountread = read(f,basebuffer,BUFFERSIZE)) > 0)
	{
		d_stream.next_in  = basebuffer;
		d_stream.avail_in = amountread;

		do {
			d_stream.next_out = inflatebuffer;
			d_stream.avail_out = BUFFERSIZE;


			err = inflate(&d_stream, Z_NO_FLUSH);

			switch (err)
			{
				case Z_NEED_DICT:
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					goto errorexit;
				default: 
					break;
			}

			write(outfile,inflatebuffer,BUFFERSIZE - d_stream.avail_out);
		} while (d_stream.avail_out == 0);
	}

errorexit:
	err = inflateEnd(&d_stream);

	close(f);
	close(outfile);

	return 0;
}

