#include <stdio.h>
#include <stdlib.h>

struct E {
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
};

struct E *make_E_seq(struct E *e1, struct E *e2);
struct E *make_E_while(struct E *cond, struct E *e);
struct E *make_E_if(struct E *cond, struct E *e1, struct E *e2);
struct E *make_E_assign(struct E *var, struct E *val);
struct E *make_E_plus(struct E *a, struct E *b);
struct E *make_E_gteq(struct E *a, struct E *b);
struct E *make_E_access(struct E *var);
struct E *make_E_bracket(struct E *e);
struct E *make_E_number(int n);
struct E *make_E_bool(int b);
struct E *make_E_var(char *var);
struct E *make_E_skip(void);

void print_E(struct E *tree);
void dotprint_E(FILE *fp, struct E *e, int *i);
