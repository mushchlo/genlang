/* main.c */
char* usage(void);

/* printasttok.c */
void Tokenfmtinstall(void);
void ASTfmtinstall(void);

/* cradle.c */
long next(void);
long peek(void);
int eof(void);
void skip_whitespace(void);
void skip_comment(void);
int isopchar(long c);
int ispunc(long c);
int isspecial(long c);