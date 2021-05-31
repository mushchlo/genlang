typedef struct Dict Dict;
struct Dict {
	char* key;
	int id;
};

Dict opdict[] = {
	"+", Add,
	"-", Sub,
	"*", Mul,
	"/", Div,
	">", Gt,
	"<", Lt,
	">=", Gteq,
	"<=", Lteq,
	"==", Doeq,
	"!=", Noteq,
	"~", Concat,
	"!", Not,

	":=", Eq,
	"+=", Addeq,
	"-=", Subeq,
	"*=", Muleq,
	"/=", Diveq,
	nil, -1
};