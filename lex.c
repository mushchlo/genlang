#include "gl.h"
#include "tok.h"


Token Errtok = { .t = (Tokkind) -1, .val = { .Strlit = nil } };

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

Token
read_punc(void)
{
	Tokval v = { .Kwopid = map_kwop(smprint("%C", (Rune)next())) };
	return (Token){ Punc, v };
}

Token
read_op(void)
{
	int i, opnum;
	Rune c[4];

	for(i = 0; !eof() && isopchar(peek()) && i < 3; i++)
		c[i] = next();
	c[i] = '\0';

	opnum = map_kwop(smprint("%S", c));
	if(opnum < 0)
		sysfatal("invalid operator '%S'", c);

	Tokval v = { .Kwopid = kwopdict[opnum].id };
	return (Token){ kwopdict[opnum].kind, v };
}

Token
read_str(void)
{
	Token got;

	Tokval v = { .Strlit = Brdstr(infile, '"', 1) };
	got = (Token){ Strlit, v };

	return got;
}

Token
read_flt(void)
{
	double d;

	if(Bgetd(infile, &d) < 0)
		sysfatal("fucking uh-oh");

	Tokval t = { .Fltlit = d };
	return (Token){ Fltlit, t };
}

Token
read_int(void)
{
	int i;

	i = 0;
	while(isdigit(peek())){
		i *= 10;
		i += next() - '0';
	}

	Tokval t = { .Intlit = i };
	return (Token){ Intlit, t };
}

Token
read_id(void)
{
	char *ident;
	Rune buf[1000];
	Tokkind kind;
	Tokval tokenized;
	int i, kwnum, id;

	for(i = 0; i < 999 && !eof() && !isspace(peek()) && !isspecial(peek()); i++)
		buf[i] = next();
	buf[i] = 0;
	ident = smprint("%S", buf);

	if((kwnum = map_kwop(ident)) >= 0){
		kind = kwopdict[kwnum].kind;
		id = kwopdict[kwnum].id;
		if(id == Tok_True || id == Tok_False){
			tokenized.Boollit = id==Tok_True;
			return (Token){ Boollit, tokenized };
		}
		tokenized.Kwopid = kwopdict[kwnum].id;
		return (Token){ kind, tokenized };
	}else{
		kind = Ident;
		tokenized.Name = ident;
		return (Token){ kind, tokenized };
	}
}

Token
next_token(void)
{
	Rune c;

	skip_whitespace();
	c = peek();
	if(eof()){
		Tokval v = { .Strlit = nil };
		return (Token){ (Tokkind) -1, v };
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
	case Intlit:	return "Intlit";
	case Fltlit:	return "Fltlit";
	case Strlit:	return "Strlit";
	default:	return "";
	}
}

void
print_tok(Token tok)
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
			if(kwopdict[i].id == tok.val.Kwopid){
				val = kwopdict[i].key;
				goto Out;
			}
		break;
	case Ident:
		val = smprint("%s", tok.val.Name);
		break;
	case Strlit:
		val = smprint("%s", tok.val.Strlit);
		break;
	case Intlit:
		val = smprint("%d", tok.val.Intlit);
		break;
	case Fltlit:
		val = smprint("%g", tok.val.Fltlit);
		break;
	}
	Out:

	if(strcmp(val, "\n") == 0)
		val = "\\n";
	if(tok.t == Strlit || tok.t == Punc)
		val = smprint("'%s'", val);

	print("{ %s: %s }\n", nameoftokkind(tok.t), val);
}

Token*
mktokenstream(void)
{
	Token tok, *tokstream;
	int maxtok, i;

	tokstream = malloc(0);
	for(i = 0, maxtok = 1; !eof(); i++){
		tok = next_token();
//		print_tok(tok);
		if(i+1 >= maxtok){
			maxtok *= 2;
			tokstream = realloc(tokstream, maxtok * sizeof(Token));
		}
		tokstream[i] = tok;
		if(tok.t < 0)
			break;
	}

	return tokstream;
}


