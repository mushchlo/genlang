#include "gl.h"
#include "tok.h"
#include "ast.h"

char*
condenseASTarr(AST **a, char *delim)
{
	int i;
	char *ret;

	for(i = 0, ret = ""; a[i] != nil; i++)
		ret = smprint("%s%s%s", ret, (i>0) ? delim : "", compile(a[i]));

	return ret;
}

char*
mk_Csafe(char *ident)
{
	return ident;	// lmao
}

char*
mk_ident(AST *id)
{
	return mk_Csafe(id->val.ident);
}

char*
mk_block(AST *b)
{
	char *lines;

	lines = condenseASTarr(b->val.block, ", ");

	return smprint("(%s)", lines);
}

char*
mk_call(AST *c)
{
	char *args;

	args = condenseASTarr(c->val.call.args, ", ");

	return smprint("%s(%s)", c->val.call.func->val.ident, args);
}

char*
mk_op(Tokid tid)
{
	typedef struct {
		Tokid id;
		char *compop;
	} D;

	int i;
	static D opdict[] = {
	Tok_Add,	"+",
	Tok_Sub,	"-",
	Tok_Mul,	"*",
	Tok_Div,	"/",
	Tok_Gt,		">",
	Tok_Lt,		"<",
	Tok_Gteq,	">=",
	Tok_Lteq,	"<=",
	Tok_Doeq,	"===",
	Tok_Noteq,	"!==",
	Tok_Concat,	"ajfsefj",

	Tok_Eq,		"=",
	Tok_Addeq,	"+=",
	Tok_Subeq,	"-=",
	Tok_Muleq,	"*=",
	Tok_Diveq,	"/=",

	Tok_Not,	"anrjbgemn",
	Tok_And,	"&&",
	Tok_Or,		"||",
	Tok_Xor,	"subafekr",

	-1,				nil
	};

	for(i = 0; opdict[i].id != tid && opdict[i].compop != nil; i++)
		;
	return opdict[i].compop;
}

char*
mk_binary(AST *b)
{
	return smprint("(%s %s %s)", compile(b->val.binary.left), mk_op(b->val.binary.op), compile(b->val.binary.right));
}

char*
mk_literal(AST *lit)
{
	switch(lit->val.literal.t){
	case Boollit:
		return smprint("%s", (lit->val.literal.val.Boollit!=0) ? "true" : "false");
	case Intlit:
		return smprint("%d", lit->val.literal.val.Intlit);
	case Fltlit:
		return smprint("%f", lit->val.literal.val.Fltlit);
	case Strlit:
		return smprint("\"%s\"", lit->val.literal.val.Strlit);
	default:
		sysfatal("wtf\n");
		return nil;
	}
}

char*
mk_if(AST *a)
{
	return smprint("(%s !== false ? %s : %s)", compile(a->val.IF.cond), compile(a->val.IF.then),
					a->val.IF.hasElse ? compile(a->val.IF.ELSE) : "false");
}

char*
mk_lambda(AST *l)
{
	char *code, *vars;

	vars = condenseASTarr(l->val.lambda.params, ", ");
	code = smprint("(function (%s) {return %s})", vars, compile(l->val.lambda.body));

	return code;
}

char*
compile(AST *expr)
{
	switch(expr->t){
	case Literal_Node:
		return mk_literal(expr);
	case Identifier_Node:
		return mk_ident(expr);
	case Assign_Node:
	case Binary_Node:
		return mk_binary(expr);
	case Lambda_Node:
		return mk_lambda(expr);
	case Call_Node:
		return mk_call(expr);
	case If_Node:
		return mk_if(expr);
	case Block_Node:
		return mk_block(expr);
	default:
		sysfatal("can't make AST %A\n", expr);
		return nil;
	}
}