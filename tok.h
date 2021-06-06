typedef enum {
	Unop,
	Binop,
	Assgn,
	Keywd,
	Ident,
	Punc,

/* constant types */
	Int,
	Flt,
	Str,
} Tokkind;

typedef enum {
/* mathematical operators */
	Add,
	Sub,
	Mul,
	Div,

/* truthiness operators*/
	Gt,
	Lt,
	Gteq,
	Lteq,
	Doeq,
	Noteq,
	Not,
	And,
	Or,
	Xor,

/* misc. operator */
	Concat,

/* assignment operators */
	Eq,
	Addeq,
	Subeq,
	Muleq,
	Diveq,

/* keywords */
	Ret,
	If,
	Else,
	λ,
	True,
	False,

/* punctuation */
	Lparen,
	Rparen,
	Lcurly,
	Rcurly,
	Lbrack,
	Rbrack,
	Comma,
	Semicol,
	Newln,
} Tokid;

typedef union Tokval Tokval;
union Tokval {
	Tokid	Id;
	int		Int;
	double	Flt;
	char*	Str;
};

typedef struct TokenEl TokenEl;
struct TokenEl {
	Tokkind		t;
	Tokval		val;
	TokenEl*	next;
};

typedef struct Dict Dict;
struct Dict {
	char*	key;
	Tokid	id;
	Tokkind	kind;
};

Dict kwopdict[] = {
	"+",		Add,	Binop,
	"-",		Sub,	Binop,
	"*",		Mul,	Binop,
	"/",		Div,	Binop,
	">",		Gt,		Binop,
	"<",		Lt,		Binop,
	">=",		Gteq,	Binop,
	"<=",		Lteq,	Binop,
	"==",		Doeq,	Binop,
	"!=",		Noteq,	Binop,
	"~",		Concat,	Binop,

	":=",		Eq,		Assgn,
	"+=",		Addeq,	Assgn,
	"-=",		Subeq,	Assgn,
	"*=",		Muleq,	Assgn,
	"/=",		Diveq,	Assgn,

	"!",		Not,	Unop,
	"and",		And,	Binop,
	"or",		Or,		Binop,
	"xor",		Xor,	Binop,

	"return",	Ret,	Keywd,
	"if",		If,		Keywd,
	"else",		Else,	Keywd,
	"λ",		λ,		Keywd,
	"true",		True,	Keywd,
	"false",	False,	Keywd,

	"(",		Lparen,	Punc,
	")",		Rparen,	Punc,
	"{",		Lcurly,	Punc,
	"}",		Rcurly,	Punc,
	"[",		Lbrack,	Punc,
	"]",		Rbrack,	Punc,
	",",		Comma,	Punc,
	";",		Semicol,	Punc,
	"\n",		Newln,	Punc,

	nil,		-1,		-1
};