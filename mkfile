</$objtype/mkfile

BIN=$home/bin/$objtype

TARG=gl

OFILES=\
	main.$O\
	lex.$O\
	printasttok.$O\
	cradle.$O\
	parse.$O\
	compile.$O\


HFILES=\
	gl.h\
	ast.h\
	fns.h\
	tok.h

</sys/src/cmd/mkone