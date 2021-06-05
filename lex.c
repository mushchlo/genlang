#include "gl.h"
#include "tok.h"


/* all of these operate on
 * the global variable `infile` */

int
map_kwop(char* str)
{
	int i;

	for(i = 0; kwopdict[i].key != nil; i++)
		if(strcmp(kwopdict[i].key, str) == 0)
			return i;
	return -1;
}

TokenEl
read_punc(void)
{
	Tokval v = { .Id = map_kwop(smprint("%C", (Rune)next())) };
	return (TokenEl){ Punc, v, nil };
}

TokenEl
read_op(void)
{
	int i, opnum;
	Rune c[4];

	for(i = 0; !eof() && isopchar(peek()) && i < 3; i++)
		c[i] = next();
	c[i] = '\0';

	opnum = map_kwop(smprint("%S", c));
	if(opnum < 0)
		sysfatal("invalid operator %S", c);

	Tokval v = { .Id = kwopdict[opnum].id };
	return (TokenEl){ kwopdict[opnum].kind, v, nil };
}

TokenEl
read_str(void)
{
	TokenEl got;

	Tokval v = { .Str = Brdstr(infile, '"', 1) };
	got = (TokenEl){ Str, v, nil };

	return got;
}

TokenEl
read_flt(void)
{
	double d;

	if(Bgetd(infile, &d) < 0)
		sysfatal("fucking uh-oh");

	Tokval t = { .Flt = d };
	return (TokenEl){ Flt, t, nil };
}

TokenEl
read_int(void)
{
	int i;

	i = 0;
	while(isdigit(peek())){
		i *= 10;
		i += next() - '0';
	}

	Tokval t = { .Int = i };
	return (TokenEl){ Int, t, nil };
}

TokenEl
read_id(void)
{
	char *ident;
	Rune buf[1000];
	Tokkind kind;
	int i, kwnum;

	for(i = 0; i < 999 && !eof() && !isspace(peek()) && !isspecial(peek()); i++)
		buf[i] = next();
	buf[i] = 0;
	ident = smprint("%S", buf);

	if((kwnum = map_kwop(ident)) >= 0){
		kind = kwopdict[kwnum].kind;
		Tokval v = { .Id = kwopdict[kwnum].id };
		return (TokenEl){ kind, v, nil };
	}else{
		kind = Ident;
		Tokval v = { .Str = ident };
		return (TokenEl){ kind, v, nil };
	}
}

TokenEl
next_token(void)
{
	Rune c;

	skip_whitespace();
	c = peek();
	if(eof()){
		Tokval t = { .Str = nil };
		return (TokenEl){ (Tokkind) -1, t, nil };
	}

	switch(c){
	case L'«':
		skip_comment();
		return next_token();
	case '"':
		next();
		return read_str();
	case '`':
		next();
		return read_flt();
	}

	if(isdigit(c))
		return read_int();
	if(isopchar(c))
		return read_op();
	if(ispunc(c))
		return read_punc();

	/* assumed to be an keyword or identifier lol */
	return read_id();
}

char*
nameoftokkind(Tokkind k)
{
	switch(k){
	case Unop:	return "Unop";
	case Binop:	return "Binop";
	case Assgn:	return "Assgn";
	case Keywd:	return "Keywd";
	case Ident:	return "Ident";
	case Punc:	return "Punc";
	case Int:	return "Int";
	case Flt:	return "Flt";
	case Str:	return "Str";
	default:	return "";
	}
}

void
print_tok(TokenEl tok)
{
	char* val;
	int i;

	val = "";
	switch(tok.t){
	case Unop:
	case Binop:
	case Assgn:
	case Keywd:
	case Punc:
		for(i = 0; kwopdict[i].key != nil; i++)
			if(kwopdict[i].id == tok.val.Id){
				val = kwopdict[i].key;
				goto Out;
			}
		break;
	case Ident:
	case Str:
		val = smprint("%s", tok.val.Str);
		break;
	case Int:
		val = smprint("%d", tok.val.Int);
		break;
	case Flt:
		val = smprint("%g", tok.val.Flt);
		break;
	}
	Out:

	if(strcmp(val, "\n") == 0)
		val = "\\n";
	if(tok.t == Str || tok.t == Punc)
		val = smprint("'%s'", val);

	print("{ %s: %s }\n", nameoftokkind(tok.t), val);
}

void
mktokenstream(void)
{
	TokenEl tokhead, *currtok;

	tokhead = next_token();
	currtok = &tokhead;
	while(1){
		TokenEl *nexttok;

		nexttok = malloc(sizeof(TokenEl));
		*nexttok = next_token();
		currtok->next = nexttok;
		if(nexttok->t < 0){
			currtok->next = nil;
			break;
		}
		currtok = nexttok;
	}

	currtok = &tokhead;
	while(currtok != nil){
		print_tok(*currtok);
		currtok = currtok->next;
	}
}


