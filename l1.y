%{

#include <stdlib.h>
#include <stdio.h>
#include "parsetree.h"
#include "tab.h"

struct E *parsetree;
Table *vartab;

%}

/* Define tokens */
%token SEMIC LBR RBR PLUS ACC ASS GTEQ IF THEN ELSE WHILE DO SKIP LCB RCB COMMA ARR

/* Define "yystype" */
%union {
  int nval;
  int bval;
  char *vval;
  struct E *e;
}

/* Associate different types with tokens */
%token <nval> NUMBER
%token <bval> BOOL
%token <vval> VAR

%type <e> exprs expr val;

%%

prog:    store exprs { parsetree = $2; }
       ;

store:   LCB RCB
       | LCB inits RCB
       ;

inits:   init COMMA inits
       | init
       ;

init:    VAR ARR NUMBER { if (vartab == NULL) vartab = mktab(); insertkeyval(vartab, $1, $3); }
       ;

exprs:   expr SEMIC exprs { $$ = make_E_seq($1, $3); }
       | expr
       ;

expr:    WHILE expr DO expr { $$ = make_E_while($2, $4); }
       | IF expr THEN expr ELSE expr { $$ = make_E_if($2, $4, $6); }
       | VAR ASS expr { $$ = make_E_assign(make_E_var($1), $3); }
       | val PLUS expr { $$ = make_E_plus($1, $3); }
       | val GTEQ val { $$ = make_E_gteq($1, $3); }
       | val
       ;

val:     NUMBER { $$ = make_E_number($1); }
       | BOOL { $$ = make_E_bool($1); }
       | SKIP { $$ = make_E_skip(); }
       | ACC VAR { $$ = make_E_access(make_E_var($2)); }
       | LBR exprs RBR { $$ = make_E_bracket($2); }
       ;

%%

#include <stdlib.h>

/* Extern stuff from Lex that Yacc needs to know about: */
extern int yylex();
extern int yyparse();
extern FILE *yyin;

extern struct E *parsetree;
extern void print_E(struct E *);
extern void dotprint_E(FILE *fp, struct E *e, int *i);
extern void transprogjvm(FILE *fp, struct E *e, Table *tab);
       
int main(int argc, char *argv[]) {
  FILE *dotfp = NULL;
  FILE *jvmfp = fopen("bytecode.jvm", "w");
  int i;
  char f;

  if(argc < 2) {
    printf("Wrong number of arguments. Usage: ./l1c [options] <target file>\n");
    return 1;
  }
  
  for (--argc, ++argv; argc > 0 && (*argv)[0] == '-'; --argc, ++argv) {
    // Have flag arguments...
    for (i = 1; i > 0 && (f = (*argv)[i]); i++) {
      // Have flags in this argument...
      switch (f) {
      case 'd':
	if ((dotfp = fopen(argv[1], "w")) == 0) {
	  perror("Could not open DOT output file");
	  return 2;
	}
	i = -1; // Skip the rest of this arg.
	--argc, ++argv; // Skip next arg too.
	break;
      default:
	printf("Invalid flag argument.");
	break;
      }
    }
  }

  // Implicitly, the last argument must be the target file for compilation.
  if ((yyin = fopen(*argv, "r")) == 0) {
    perror("Could not open target file");
    return 2;
  }

  /* Do the parsing... */
  do {
    yyparse();
  } while(!feof(yyin));

  if (dotfp != NULL) {
    int j = 0;
    dotprint_E(dotfp, parsetree, &j);
  }

  if (jvmfp != NULL) {
    transprogjvm(jvmfp, parsetree, vartab);
  }
  
  return 0;
}

void yyerror(char *s) {
  printf("L1 parse error: %s\n", s);
  exit(3);
}
