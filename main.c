#include "gl.h"

Biobuf* infile;

char*
usage(void)
{
	return "usage: gl [file]";
}


void
main(int argc, char* argv[])
{
	if(argc < 2)
		sysfatal(usage());

	if((infile = Bopen(argv[1], OREAD)) == nil)
		sysfatal("failed to open file for compilation: %r");
	mktokenstream();
	Bterm(infile);
}