typedef enum {
	Unop,
	Binop,
	Assgn,
	Keywd,
	Ident,
	Punc,

/* literal types */
	Boollit,
	Intlit,
	Fltlit,
	Strlit
} Tokkind;

typedef enum {
/* mathematical operators */
	Tok_Add,
	Tok_Sub,
	Tok_Mul,
	Tok_Div,

/* truthiness operators*/
	Tok_Gt,
	Tok_Lt,
	Tok_Gteq,
	Tok_Lteq,
	Tok_Doeq,
	Tok_Noteq,
	Tok_Not,
	Tok_And,
	Tok_Or,
	Tok_Xor,

/* misc. operator */
	Tok_Concat,

/* assignment operators */
	Tok_Eq,
	Tok_Addeq,
	Tok_Subeq,
	Tok_Muleq,
	Tok_Diveq,

/* keywords */
	Tok_Ret,
	Tok_If,
	Tok_Else,
	Tok_λ,
	Tok_True,
	Tok_False,

/* punctuation */
	Tok_Lparen,
	Tok_Rparen,
	Tok_Lcurly,
	Tok_Rcurly,
	Tok_Lbrack,
	Tok_Rbrack,
	Tok_Comma,
	Tok_Semicol
} Tokid;

typedef union {
	Tokid	Kwopid;
	char*	Name;

	int		Boollit;
	int		Intlit;
	double	Fltlit;
	char*	Strlit;
} Tokval;

typedef struct {
	Tokkind		t;
	Tokval		val;
} Token;

typedef struct {
	char*	key;
	Tokid	id;
	Tokkind	kind;
} Dict;

static Dict kwopdict[] = {
	"+",		Tok_Add,	Binop,
	"-",		Tok_Sub,	Binop,
	"*",		Tok_Mul,	Binop,
	"/",		Tok_Div,	Binop,
	">",		Tok_Gt,		Binop,
	"<",		Tok_Lt,		Binop,
	">=",		Tok_Gteq,	Binop,
	"<=",		Tok_Lteq,	Binop,
	"==",		Tok_Doeq,	Binop,
	"!=",		Tok_Noteq,	Binop,
	"~",		Tok_Concat,	Binop,

	":=",		Tok_Eq,		Assgn,
	"+=",		Tok_Addeq,	Assgn,
	"-=",		Tok_Subeq,	Assgn,
	"*=",		Tok_Muleq,	Assgn,
	"/=",		Tok_Diveq,	Assgn,

	"!",		Tok_Not,	Unop,
	"and",		Tok_And,	Binop,
	"or",		Tok_Or,		Binop,
	"xor",		Tok_Xor,	Binop,

	"return",	Tok_Ret,	Keywd,
	"if",		Tok_If,		Keywd,
	"else",		Tok_Else,	Keywd,
	"λ",		Tok_λ,		Keywd,
	"true",		Tok_True,	Keywd,
	"false",	Tok_False,	Keywd,

	"(",		Tok_Lparen,	Punc,
	")",		Tok_Rparen,	Punc,
	"{",		Tok_Lcurly,	Punc,
	"}",		Tok_Rcurly,	Punc,
	"[",		Tok_Lbrack,	Punc,
	"]",		Tok_Rbrack,	Punc,
	",",		Tok_Comma,	Punc,
	";",		Tok_Semicol,	Punc,

	nil,		-1,		-1
};

Token* mktokenstream(void);
void print_tok(Token t);
