#include <cstdio>
#include <cstdlib>

struct MEMFILE
{
	int length;
	int pos;
	void *data;
};


unsigned int memopen(char *name)
{
	MEMFILE *memfile;

	memfile = (MEMFILE*)calloc(sizeof(MEMFILE),1);

 	// load an external file and read it
	FILE *fp;
	fp = fopen(name, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		memfile->length = ftell(fp);
		memfile->data = calloc(memfile->length,1);
		memfile->pos = 0;

		fseek(fp, 0, SEEK_SET);
		fread(memfile->data, 1, memfile->length, fp);
		fclose(fp);
	}

	return (unsigned int)memfile;
}

void memclose(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;
	free(memfile->data);			// dont free it if it was initialized with LockResource
	free(memfile);
}

int memread(void *buffer, int size, unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (memfile->pos + size >= memfile->length)
		size = memfile->length - memfile->pos;

	memcpy(buffer, (char *)memfile->data+memfile->pos, size);
	memfile->pos += size;
	
	return size;
}

void memseek(unsigned int handle, int pos, signed char mode)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (mode == SEEK_SET) 
		memfile->pos = pos;
	else if (mode == SEEK_CUR) 
		memfile->pos += pos;
	else if (mode == SEEK_END)
		memfile->pos = memfile->length + pos;

	if (memfile->pos > memfile->length)
		memfile->pos = memfile->length;
}

int memtell(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;
	return memfile->pos;
}