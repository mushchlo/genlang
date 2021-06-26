#include "gl.h"
#include "tok.h"
#include "ast.h"

Biobuf* infile;

char*
usage(void)
{
	return "usage: gl [file]";
}


void
main(int argc, char* argv[])
{
	Token *tokstrm;
	AST *tocompile;

	if(argc < 2)
		sysfatal(usage());
	Tokenfmtinstall();
	ASTfmtinstall();

	if((infile = Bopen(argv[1], OREAD)) == nil)
		sysfatal("failed to open file for compilation: %r");
	tokstrm = mktokenstream();
	tocompile = parse_toplevel(tokstrm);

	print("%A\n", tocompile->val.block[0]->val.binary.right->val.lambda.body);
	print("%s", compile(tocompile));

	Bterm(infile);
}