#include <u.h>
#include <libc.h>
#include <ctype.h>
#include <bio.h>

extern Biobuf* infile;

typedef enum {
	Unop,
	Binop,
	Assgn,
	Keywd,
	Id,
	Punc,

/* constant types */
	Int,
	Flt,
	Str
} Toktype;

typedef union Tokval Tokval;
union Tokval {
	int op;
	int i;
	double f;
	char* s;
};

typedef struct TokenEl TokenEl;
struct TokenEl {
	Toktype t;
	Tokval val;
	TokenEl* next;
};

typedef enum {
	Not
} Unoptype;

typedef enum {
	Add,
	Sub,
	Mul,
	Div,
	Gt,
	Lt,
	Gteq,
	Lteq,

	Doeq,
	Noteq,

	And,
	Or,
	Xor,

	Concat
} Binoptype;

typedef enum {
	Eq,
	Addeq,
	Subeq,
	Muleq,
	Diveq
} Assgntype;

typedef enum {
	Ret,
	If,
	Else,
	λ,
	True,
	False
} Keywdtype;

typedef enum {
	Lparen,
	Rparen,
	Lcurly,
	Rcurly,
	Semicol,
	Comma
} Punctype;

typedef struct Dict Dict;
struct Dict {
	Rune* key;
	int id;
};

Dict opdict[] = {
	L"+", Add,
	L"-", Sub,
	L"*", Mul,
	L"/", Div,
	L">", Gt,
	L"<", Lt,
	L">=", Gteq,
	L"<=", Lteq,
	L"==", Doeq,
	L"!=", Noteq,
	L"~", Concat,
	L"!", Not,

	L":=", Eq,
	L"+=", Addeq,
	L"-=", Subeq,
	L"*=", Muleq,
	L"/=", Diveq,
	nil, -1
};

#include "fns.h"