#include "parsetree.h"
#include "tab.h"

void transexprjvm(FILE *fp, struct E *e, Table *tab, int *li) {
  /* Translate expression 'e' with variable table 'tab' to JVM bytecodes and
     print to the given file.
  */
  switch (e->flavour) {
  case E_seq: {
    transexprjvm(fp, e->u.diad.left, tab, li);
    transexprjvm(fp, e->u.diad.right, tab, li);
    break;
  }
  case E_while: {
    int condl = (*li)++;
    int endl = (*li)++;
    fprintf(fp, "label%d:", condl);
    transexprjvm(fp, e->u.diad.left, tab, li);
    fprintf(fp, "\tifeq label%d\n", endl);
    transexprjvm(fp, e->u.diad.right, tab, li);
    fprintf(fp, "\tgoto label%d\n", condl);
    fprintf(fp, "label%d:", endl);
    break;
  }
  case E_if: {
    int elsel = (*li)++;
    int endl = (*li)++;
    transexprjvm(fp, e->u.triad.left, tab, li);
    fprintf(fp, "\tifeq label%d\n", elsel);
    transexprjvm(fp, e->u.triad.middle, tab, li);
    fprintf(fp, "\tgoto %d\n", endl);
    fprintf(fp, "label%d:", elsel);
    transexprjvm(fp, e->u.triad.right, tab, li);
    fprintf(fp, "label%d:", endl);
    break;
  }
  case E_assign: {
    int vi = indexfromkey(tab, e->u.diad.left->u.var);
    transexprjvm(fp, e->u.diad.right, tab, li);
    fprintf(fp, (vi < 4) ? "\tistore_%d\n" : "\tistore %d\n", vi);
    break;
  }
  case E_plus: {
    transexprjvm(fp, e->u.diad.left, tab, li);
    transexprjvm(fp, e->u.diad.right, tab, li);
    fprintf(fp, "\tiadd\n");
    break;
  }
  case E_gteq: {
    transexprjvm(fp, e->u.diad.left, tab, li);
    transexprjvm(fp, e->u.diad.right, tab, li);
    fprintf(fp, "\tif_icmpge label%d\n", *li);
    fprintf(fp, "\ticonst_0\n");
    fprintf(fp, "label%d:\ticonst_1\n", (*li)++);
    break;
  }
  case E_access: {
    int vi = indexfromkey(tab, e->u.monad.child->u.var);
    fprintf(fp, (vi < 4) ? "\tiload_%d\n" : "\tiload %d\n", vi);
    break;
  }
  case E_bracket: {
    transexprjvm(fp, e->u.monad.child, tab, li);
    break;
  }
  case E_number: {
    int n = e->u.number;
    fprintf(fp, (n < 6 && n >= 0) ? "\ticonst_%d\n" : "\tldc %d\n", n);
    break;
  }
  case E_bool: {
    fprintf(fp, e->u.bool ? "\ticonst_1\n" : "\ticonst_0\n");
    break;
  }
  default:
    // E_skip doesn't requier any translation.
    // E_var is always handled by its parent node.
    break;
  }
}

void transprogjvm(FILE *fp, struct E *prog, Table *tab) {
  // First put initial values of the variables into memory.
  int i, j, v;
  for (i = 0, v = -1; (v = valfromindex(tab, i)) > 0; i++) {
    fprintf(fp, (v < 6 && v >= 0) ? "\ticonst_%d\n" : "\tldc %d\n", v);    
    fprintf(fp, (i < 4) ? "\tistore_%d\n" : "\tistore %d\n", i);
  }

  // Now traverse the syntax tree, outputting JVM bytecodes for each construct.
  j = 0;
  transexprjvm(fp, prog, tab, &j);

  // Add an extra nop in case there's a trailing label with no instruction.
  fprintf(fp, "\tnop\n");
}
