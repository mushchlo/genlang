#include "gl.h"
#include "tok.h"
#include "ast.h"


Token *Stream;
int StreamDepth;
Token errtok = { .t = (Tokkind) -1, .val = { .Strlit = nil } };
AST falseAST = { .t = Literal_Node, .val = { .literal = { .val = { .Boollit = 0 } } } };

struct {
	Tokid id;
	int prec;
} PRECEDENCE[] = {
	Tok_Or,		1,
	Tok_Xor,	2,
	Tok_And,	3,

	Tok_Gt,		7,
	Tok_Lt,		7,
	Tok_Gteq,	7,
	Tok_Lteq,	7,
	Tok_Doeq,	7,
	Tok_Noteq,	7,

	Tok_Add,	10,
	Tok_Sub,	10,

	Tok_Mul,	20,
	Tok_Div,	20,

	-1,		-1
};

AST* parse_expr(void);
AST* maybe_binary(AST* left, int prec);
AST* parse_atom(void);

int
precedence(Token tok)
{
	int i;

	switch(tok.t){
	case Assgn:
		return 0;
	case Binop:
		for(i = 0; PRECEDENCE[i].id != tok.val.Kwopid && PRECEDENCE[i].id > 0; i++)
			;
		return PRECEDENCE[i].prec;
	default:
		return -1;
	}
}

void
croak(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vfprint(2, fmt, args);
	va_end(args);
}

Token
peek_tok(void)
{
	return *Stream;
}

Token
next_tok(void)
{
	if(Stream->t < 0)
		return errtok;		// if we've reached the end, don't increment.
	return *Stream++;
}

int
eof_tok(void)
{
	Token peeked = peek_tok();
	return peeked.t < 0;
}

int
is_tokid(Tokid t)
{
	Token peeked;

	peeked = peek_tok();

	return t == peeked.val.Kwopid;
}

int
is_tokkind(Tokkind k)
{
	Token peeked;

	peeked = peek_tok();

	return k == peeked.t;
}

void
skip_tok(Tokid t)
{
	if(is_tokid(t))
		next_tok();
	else
		croak("Expecting token: '%T'", t);
}

/* calls malloc */
AST**
delimited(Tokid begin, Tokid end, Tokid delim, AST*(*parser)(void), int *len)
{
	AST **ret;
	int first, maxlen, i;

	skip_tok(begin);
	ret = malloc(sizeof(AST*));
	for(i = 0, first = maxlen = 1; !eof_tok() && !is_tokid(end); i++){
		if(first)
			first = 0;
		else
			skip_tok(delim);
		if(is_tokid(end))
			break;

		if(i+1 > maxlen){
			maxlen = (i + 1) * 2;
			ret = realloc(ret, maxlen * sizeof(AST*));
		}
		ret[i] = parser();
	}
	ret[i] = nil;
	skip_tok(end);
	if(len != nil)
		*len = i;

	return ret;
}

AST*
parse_const(void)
{
	AST *parsed;
	ASTval parsedval;
	Token curr;

	curr = next_tok();
	switch(curr.t){
	case Boollit:
	case Intlit:
	case Fltlit:
	case Strlit:
		parsedval.literal = curr;
		parsed = malloc(sizeof(AST));
		*parsed = (AST){ Literal_Node, parsedval };
		return parsed;
	default:
		croak("expected constant token\n");
		return nil;
	}	
}

AST*
parse_ident(void)
{
	AST *parsed;
	ASTval parsedval;
	Token curr;

	curr = next_tok();
	if(curr.t != Ident)
		croak("Expecting identifier");

	parsedval.ident = (curr.t!=Ident) ? nil : curr.val.Name;
	parsed = malloc(sizeof(AST));
	*parsed = (AST){ Identifier_Node, parsedval };

	return parsed;
}

AST*
parse_if(void)
{
	AST *parsed;
	ASTval parsedval;

	skip_tok(Tok_If);
	parsedval.IF.hasElse = 0;
	parsedval.IF.cond = parse_expr();
	parsedval.IF.then = parse_expr();

	if(is_tokid(Tok_Else)){
		next_tok();
		parsedval.IF.hasElse = 1;
		parsedval.IF.ELSE = parse_expr();
	}

	parsed = malloc(sizeof(AST));
	*parsed = (AST){ If_Node, parsedval };
	return parsed;
}

AST*
parse_block(void)
{
	AST *parsed;
	ASTval parsedval;
	int len;

	parsed = malloc(sizeof(AST));
	parsedval.block = delimited(Tok_Lcurly, Tok_Rcurly, Tok_Semicol, parse_expr, &len);
	switch(len){
	case 0:
		*parsed = falseAST;
		break;
	case 1:
		parsed = parsedval.block[0];
		break;
	default:
		*parsed = (AST){ Block_Node, parsedval };
		break;
	}

	return parsed;
}

AST*
parse_lambda(void)
{
	AST *parsed;
	ASTval parsedval;

	parsedval.lambda.params = delimited(Tok_Lparen, Tok_Rparen, Tok_Comma, parse_ident, nil);
	parsedval.lambda.body = parse_expr();

	parsed = malloc(sizeof(AST));
	*parsed = (AST){ Lambda_Node, parsedval };

	return parsed;
}

AST*
parse_toplevel(Token *tokstrm)
{
	int i, maxlen;
	AST **block, *parsed;
	ASTval tmp;


	Stream = tokstrm;
	block = malloc(0);
	for(i = 0, maxlen = 1; !eof_tok(); i++){
		if(i + 1 >= maxlen){
			maxlen *= 2;
			block = realloc(block, maxlen * sizeof(AST*));
		}
		block[i] = parse_expr();
		if(!eof_tok())
			skip_tok(Tok_Semicol);
	}
	block[i] = nil;

	tmp.block = block;
	parsed = malloc(sizeof(AST));
	*parsed = (AST){ Block_Node, tmp };

	return parsed;
}

AST*
parse_call(AST *a)
{
	AST *parsed;
	ASTval parsedval;

	parsed = malloc(sizeof(AST));
	parsedval.call.func = a;
	parsedval.call.args = delimited(Tok_Lparen, Tok_Rparen, Tok_Comma, parse_expr, nil);
	*parsed = (AST){ Call_Node, parsedval };

	return parsed;
}

AST*
maybe_call(AST*(get_ast)(void))
{
	AST *a;

	a = get_ast();

	if(is_tokid(Tok_Lparen))
		return parse_call(a);
	else
		return a;
}

AST*
parse_atomHelper(void)
{
	AST *expr;
	Token tok;

	tok = peek_tok();
	switch(tok.t){
	case Punc:
	case Keywd:
		switch(tok.val.Kwopid){
		case Tok_Lparen:
			next_tok();
			expr = parse_expr();
			skip_tok(Tok_Rparen);

			return expr;
		case Tok_Lcurly:
			return parse_block();
		case Tok_If:
			return parse_if();
		case Tok_λ:
			next_tok();
			return parse_lambda();
		}
	case Boollit:
	case Intlit:
	case Fltlit:
	case Strlit:
		return parse_const();
	case Ident:
		return parse_ident();
	default:
		croak("Unexpected token %T", peek_tok());
		return nil;
	}
}

AST*
parse_atom(void)
{
	return maybe_call(parse_atomHelper);
}


AST*
maybe_binary(AST *left, int prev_prec)
{
	if(is_tokkind(Binop) || is_tokkind(Assgn)){
		AST *parsed;
		ASTval parsedval;
		Token tok;
		int new_prec;

		tok = peek_tok();
		new_prec = precedence(tok);
		if(new_prec > prev_prec){
			next_tok();
			parsedval.binary.op		= tok.val.Kwopid;
			parsedval.binary.left	= left;
			parsedval.binary.right	= maybe_binary(parse_atom(), new_prec);
			parsed = malloc(sizeof(AST));
			*parsed = (AST){ (tok.t==Assgn) ? Assign_Node : Binary_Node, parsedval };

			return maybe_binary(parsed, prev_prec);
		}
	}

	return left;
}

AST*
parse_exprHelper(void)
{
	return maybe_binary(parse_atom(), -1);
}

AST*
parse_expr(void)
{
	return maybe_call(parse_exprHelper);
}