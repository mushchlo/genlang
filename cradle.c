#include "gl.h"

char* opchars = "!+-*/<>=:~";
char* puncchars = "(){}[],;";

long
next(void)
{
	return Bgetrune(infile);
}

long
peek(void)
{
	long c;

	c = Bgetrune(infile);
	if(c > 0)
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
isopchar(long c)
{
	return utfrune(opchars, c) != nil;
}

int
ispunc(long c)
{
	return utfrune(puncchars, c) != nil;
}

int
isspecial(long c)
{
	return isopchar(c) || ispunc(c) || c == L'«' ||
			c == '"' || c == '`';
}