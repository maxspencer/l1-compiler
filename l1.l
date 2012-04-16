/* Definition Section */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
%}

NUMBER     -?[0-9]+
VAR        [a-zA-Z][a-zA-Z0-9_]*
WHITESPACE [ \t\n\r]

%%

";" { return SEMIC; }
"+" { return PLUS; }
"(" { return LBR; }
")" { return RBR; }
"!" { return ACC; }
"{" { return LCB; }
"}" { return RCB; }
"," { return COMMA; }
":=" { return ASS; }
">=" { return GTEQ; }
"->" { return ARR; }
"if" { return IF; }
"then" { return THEN; }
"else" { return ELSE; }
"while" { return WHILE; }
"do" { return DO; }
"skip" { return SKIP; }

"true" { yylval.bval = 1; return BOOL; }
"false" { yylval.bval = 0; return BOOL; }

{NUMBER} { yylval.nval = atoi(yytext); return NUMBER; }

{VAR} {
  char *t = (char *) malloc(strlen(yytext) + 1);
  yylval.vval = strcpy(t, yytext);
  return VAR;
}

{WHITESPACE} ; /* Ignore whitespace */

. ;

%%
