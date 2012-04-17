%{

#include <stdlib.h>
#include <stdio.h>
#include "syn.h"
#include "tab.h"

Expr *parsetree;
Table *vartab;

%}

/* Define tokens */
%token SEMIC LBR RBR PLUS ACC ASS GTEQ IF THEN ELSE WHILE DO SKIP LCB RCB COMMA ARR

/* Define "yystype" */
%union {
  int nval;
  int bval;
  char *vval;
  Expr *e;
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

exprs:   expr SEMIC exprs { $$ = mkseq($1, $3); }
       | expr
       ;

expr:    WHILE expr DO expr { $$ = mkwhile($2, $4); }
       | IF expr THEN expr ELSE expr { $$ = mkif($2, $4, $6); }
       | VAR ASS expr { $$ = mkassign(mkvar($1), $3); }
       | val PLUS expr { $$ = mkplus($1, $3); }
       | val GTEQ val { $$ = mkgteq($1, $3); }
       | val
       ;

val:     NUMBER { $$ = mknumber($1); }
       | BOOL { $$ = mkbool($1); }
       | SKIP { $$ = mkskip(); }
       | ACC VAR { $$ = mkaccess(mkvar($2)); }
       | LBR exprs RBR { $$ = mkbracket($2); }
       ;

%%

#include <stdlib.h>

#define USAGETEXT "Usage: ./l1c [options] <target file> or ./l1c -h for help\n"
#define HELPTEXT "Options:\n-d <file>\tOutput syntax tree in DOT format to the given output file.\n-j <file>\tOutput the JVM Bytecode intermediate code to the given output file.\n-h\t\tSee this help text.\n"

// From lex:
extern int yylex();
extern int yyparse();
extern FILE *yyin;

// From syn:
extern Expr *parsetree;
extern void printe(Expr *);
extern void fdote(FILE *fp, Expr *e, int *i);

// From trans:
extern void transprogjvm(FILE *fp, Expr *e, Table *tab);

int main(int argc, char *argv[]) {
  FILE *dotfp = NULL;
  FILE *jvmfp = NULL;
  int i;
  char f;

  if(argc < 2) {
    printf("Wrong number of arguments. " USAGETEXT);
    return 1;
  }
  
  // Handle the options/arguments:
  for (--argc, ++argv; argc > 0 && (*argv)[0] == '-'; --argc, ++argv) {
    // Have flag arguments...
    for (i = 1; i > 0 && (f = (*argv)[i]); i++) {
      // Have flags in this argument...
      switch (f) {
	
      case 'd': // Do DOT output:
	if ((dotfp = fopen(argv[1], "w")) == 0) {
	  perror("Could not open DOT output file");
	  return 2;
	}
	i = -1; // Skip the rest of this arg.
	--argc, ++argv; // Skip next arg too.
	break;

      case 'j': // Do JVM Bytecode output:
	if ((jvmfp = fopen(argv[1], "w")) == 0) {
	  perror("Could not open JVM Bytecode output file");
	  return 2;
	}
	i = -1; // Skip the rest of this arg.
	--argc, ++argv; // Skip next arg.
	break;

      case 'h': // Show help text:
	printf(USAGETEXT HELPTEXT);
	return 0;
	
      default:
	printf("Invalid flag argument. " USAGETEXT);
	break;
      }
    }
  }

  // Implicitly, the last argument must be the target file for compilation...
  if ((yyin = fopen(*argv, "r")) == 0) {
    perror("Could not open target file");
    return 2;
  }

  // Do the parsing...
  do {
    yyparse();
  } while(!feof(yyin));

  if (dotfp != NULL) {
    int j = 0;
    fdote(dotfp, parsetree, &j);
  }

  if (jvmfp != NULL) {
    transprogjvm(jvmfp, parsetree, vartab);
  }
  
  return 0;
}

void yyerror(char *s) {
  printf("Error parsing L1 code: %s\n", s);
  exit(3);
}
