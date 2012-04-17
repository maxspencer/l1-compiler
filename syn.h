#include <stdio.h>
#include <stdlib.h>

typedef struct E {
  enum { E_seq, E_while, E_if, E_assign, E_plus, E_gteq, E_access, E_bracket,
	 E_var, E_number, E_bool, E_skip } flavour;
  union {
    /* E_if */
    struct { struct E *left, *middle, *right; } triad;
    /* E_seq, E_while, E_assign, E_plus, E_gteq */
    struct { struct E *left, *right; } diad;
    /* E_access, E_bracket */
    struct { struct E *child; } monad;
    /* E_number */
    int number;
    /* E_bool */
    int bool;
    /* E_var */
    char *var;
  } u;
} Expr;

Expr *mkseq(Expr *e1, Expr *e2);
Expr *mkwhile(Expr *cond, Expr *e);
Expr *mkif(Expr *cond, Expr *e1, Expr *e2);
Expr *mkassign(Expr *var, Expr *val);
Expr *mkplus(Expr *a, Expr *b);
Expr *mkgteq(Expr *a, Expr *b);
Expr *mkaccess(Expr *var);
Expr *mkbracket(Expr *e);
Expr *mknumber(int n);
Expr *mkbool(int b);
Expr *mkvar(char *var);
Expr *mkskip(void);

void printe(Expr *tree);
void fdote(FILE *fp, Expr *e, int *i);
