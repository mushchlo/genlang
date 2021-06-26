#include "gl.h"
#include "tok.h"
#include "ast.h"

#pragma varargck type "T" Struct Token
#pragma varargck type "A" Struct AST*

char*
key_of_tokid(Tokid tid)
{
	int i;

	for(i = 0; kwopdict[i].id != tid && kwopdict[i].key != nil; i++)
		;
	return (kwopdict[i].key!=nil) ? kwopdict[i].key
									: "invalid token";
}

char*
tokval_str(Token tok)
{
	switch(tok.t){
	case Boollit:
		return (tok.val.Boollit!=0) ? "false" : "true";
	case Intlit:
		return smprint("%d", tok.val.Intlit);
	case Fltlit:
		return smprint("%f", tok.val.Fltlit);
	case Strlit:
		return smprint("\"%s\"", tok.val.Strlit);
	case Punc:
	case Binop:
	case Unop:
	case Keywd:
	case Assgn:
		return smprint("'%s'", key_of_tokid(tok.val.Kwopid));
	case Ident:
		return tok.val.Name;
	default:
		return "not a token";
	}
}

char*
toktype_str(Token tok)
{
	switch(tok.t){
	case Unop:
		return "Unary Operator";
	case Binop:
		return "Binary Operator";
	case Assgn:
		return "Assignment Operator";
	case Keywd:
		return "Keyword";
	case Ident:
		return "Identifier";
	case Punc:
		return "Punctuation mark";
	case Boollit:
	case Intlit:
	case Fltlit:
	case Strlit:
		return "Literal value";
	default:
		return "N/A";
	}
}

static int
printTok(Fmt *f, Token tok)
{
	if(tok.t >= Unop && tok.t <= Strlit)
		return fmtprint(f, "%s: %s",toktype_str(tok), tokval_str(tok));
	return 0;
}

int
Tokfmt(Fmt *f)
{
	Token tok;

	tok = va_arg(f->args, Token);
	return printTok(f, tok);
}

void
Tokenfmtinstall(void)
{
	fmtinstall('T', Tokfmt);
}

static int
printIndent(Fmt *f, int indent)
{
	int ret;

	for(ret = 0; indent-- > 0;)
		ret = fmtprint(f, "\t") || ret;
	return ret;
}

static int printAST(Fmt *f, AST *a, int indent, int flat);

static int
fmtindentprint(Fmt *f, int indent, char *fmt, ...)
{
	int ret;
	va_list args;

	ret = printIndent(f, indent);
	va_start(args, fmt);
	ret += fmtvprint(f, fmt, args);
	va_end(args);

	return ret;
}

static int
printASTarray(Fmt *f, AST **AA, int indent)
{
	int i, ret;

	ret = fmtprint(f, " [\n");
	for(i = 0; AA[i] != nil; i++){
		ret += printAST(f, AA[i], indent, 1);
		if(AA[i+1] != nil)	// next AST is not last
			ret += fmtprint(f, ",\n");
	}
	ret += fmtprint(f, "\n");
	ret += fmtindentprint(f, indent - 1, "]\n");

	return ret;
}

static int
printCall(Fmt *f, ASTval C, int indent)
{
	int ret;

	ret  = fmtindentprint(f, indent, "type: call,\n");
	ret += fmtindentprint(f, indent, "func:");
	ret += printAST(f, C.call.func, indent + 1, 0);
	ret += fmtindentprint(f, indent, "args:");
	ret += printASTarray(f, C.call.args, indent + 1);

	return ret;
}

static int
printLambda(Fmt *f, ASTval L, int indent)
{
	int ret;

	ret  = fmtindentprint(f, indent, "type: lambda,\n");
	ret += fmtindentprint(f, indent, "params:");
	ret += printASTarray(f, L.lambda.params, indent + 1);

	ret += fmtindentprint(f, indent, "body:");
	ret += printAST(f, L.lambda.body, indent + 1, 0);

	return ret;
}

static int
printIf(Fmt *f, ASTval I, int indent)
{
	int ret;

	ret  = fmtindentprint(f, indent, "type: if,\n");
	ret += fmtindentprint(f, indent, "cond:");
	ret += printAST(f, I.IF.cond, indent + 1, 0);

	ret += fmtindentprint(f, indent, "then:");
	ret += printAST(f, I.IF.then, indent + 1, 0);

	if(I.IF.hasElse){
		ret += fmtindentprint(f, indent, "else:");
		ret += printAST(f, I.IF.ELSE, indent + 1, 0);
	}

	return ret;
}

static int
printBinary(Fmt *f, AST *A, int indent)
{
	ASTval B;
	int ret;

	B = A->val;
	ret  = fmtindentprint(f, indent, "type: %s,\n", (A->t==Assign_Node) ? "assign" : "binary");
	ret += fmtindentprint(f, indent, "operator: '%s',\n", key_of_tokid(B.binary.op));

	ret += fmtindentprint(f, indent, "left:");
	ret += printAST(f, B.binary.left, indent + 1, 0);

	ret += fmtindentprint(f, indent, "right:");
	ret += printAST(f, B.binary.right, indent + 1, 0);

	return ret;
}

static int
printBlock(Fmt *f, ASTval B, int indent)
{
	int ret;

	ret  = fmtindentprint(f, indent, "block:");
	ret += printASTarray(f, B.block, indent + 1);

	return ret;
}

static int
printAST(Fmt *f, AST *a, int indent, int flat)
{
	int ret;

	ret = flat ? fmtindentprint(f, indent, "{\n") : fmtprint(f, " {\n");
	switch(a->t){
	case Literal_Node:
		ret += printIndent(f, indent + (flat ? 1 : 0));
		ret += printTok(f, a->val.literal);
		ret += fmtprint(f, "\n");
		break;
	case Identifier_Node:
		ret += fmtindentprint(f, indent + (flat ? 1 : 0), "Identifier: '%s'\n", a->val.ident);
		break;
	case Lambda_Node:
		ret += printLambda(f, a->val, indent);
		break;
	case Call_Node:
		ret += printCall(f, a->val,indent + 1);
		break;
	case If_Node:
		ret += printIf(f, a->val, indent + 1);
		break;
	case Assign_Node:
	case Binary_Node:
		ret += printBinary(f, a, indent + 1);
		break;
	case Block_Node:
		ret += printBlock(f, a->val, indent);
		break;
	default:
		print("UNEXPECTED NODE WTF %d\n", a->t);
		return 0;
	}
	ret += fmtindentprint(f, indent - 1 + (flat != 0), flat ? "}" : "}\n");

	return ret;
}

int
ASTfmt(Fmt *f)
{
	AST *a;

	a = va_arg(f->args, AST*);
	return printAST(f, a, 0, 0);
}

void
ASTfmtinstall(void)
{
	fmtinstall('A', ASTfmt);
}