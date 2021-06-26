typedef enum {
	Literal_Node,

	Identifier_Node,
	Lambda_Node,
	Call_Node,
	If_Node,

	Assign_Node,
	Unary_Node,
	Binary_Node,
	Block_Node,

	Let_Node
} ASTtype;


typedef struct AST AST;
typedef union {
	Token literal;

	char *ident;

	struct {
		AST **params;
		AST *body;
	} lambda;

	struct {
		AST *cond;
		AST *then;
		int hasElse;
		AST *ELSE;
	} IF;

	struct {
		AST *func;
		AST **args;	
	} call;

	AST **block;

	struct {
		Tokid op;
		AST *left;
		AST *right;
	} binary;

	
} ASTval;

struct AST {
	ASTtype		t;
	ASTval		val;
};

AST* parse_toplevel(Token *tokstrm);
char* compile(AST *a);
