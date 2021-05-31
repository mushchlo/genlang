#include "gl.h"
//#include "tok.h"

char* opchars = "!+-*/<>=:~";

/* all of these operate on
 * the global variable `infile` */

long
next(void)
{
	return Bgetrune(infile);
}

long
peek(void)
{
	Rune c;

	c = Bgetrune(infile);
	Bungetrune(infile);

	return c;
}

int
eof(void)
{
	return peek() < 0;
}

void
skip_whitespace(void)
{
	while(isspace(peek()))
		next();
}

void
skip_comment(void)
{
	while(!eof() && next() != L'»')
		;
}

int
utfcmp(Rune* r₁, Rune* r₂)
{
	char *s₁, *s₂;

	s₁ = smprint("%S", r₁);
	s₂ = smprint("%S", r₂);

	return strcmp(s₁, s₂);
}

int
isopchar(long c)
{
	if(utfrune(opchars, c) != nil)
		return 1;
	return 0;
}

int
map_operator(Rune op[])
{
	int i;

	for(i = 0; opdict[i].key != nil; i++)
		if(utfcmp(op, opdict[i].key) == 0)
			return opdict[i].id;

	return -1;
}

TokenEl
read_op(void)
{
	int i, op;
	Rune c[3];

	for(i = 0; !eof() && isopchar(peek()) && i < 2; i++)
		c[i] = next();
	c[i] = '\0';
	op = map_operator(c);
	if(op < 0)
		sysfatal("invalid operator %S", c);

	Tokval t;
	t.op = op;
	return (TokenEl){ (op==Not) ? Unop : Binop, t, nil };
}

TokenEl
read_str(void)
{
	TokenEl got;

	Tokval t;
	t.s = Brdstr(infile, '"', 1);
	got = (TokenEl){ Str, t, nil };
	Bseek(infile, Blinelen(infile), 1);

	return got;
}

TokenEl
read_flt(void)
{
	double d;

	if(Bgetd(infile, &d) < 0)
		sysfatal("fucking uh-oh");

	Tokval t = { .f = d };
	return (TokenEl){ Flt, (t = { .f = d }), nil };
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

	Tokval t;
	t.op = i;
	return (TokenEl){ Int, t, nil };
}

TokenEl
next_token(void)
{
	Rune c;

	skip_whitespace();
	c = peek();
	if(eof())
		return void;

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
		return (TokenEl){ Punc, next(), nil };

	/* assumed to be an keyword or identifier lol */
	return read_id();
}

void
mktokenstream(void)
{
	TokenEl tokstream;

	while(!eof()){
	}
}


