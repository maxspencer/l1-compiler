#include <stdio.h>
#include <stdlib.h>

char *flavours[] = {";", "while do", "if then else", ":=", "+", ">=", "!", 
		    "()", "var", "number", "bool", "skip"};
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

Expr *mkseq(Expr *e1, Expr *e2) {
  Expr *r = (Expr *) malloc(sizeof(Expr));
  r->flavour = E_seq;
  r->u.diad.left = e1;
  r->u.diad.right = e2;
  return r;
}

Expr *mkwhile(Expr *cond, Expr *e) {
  Expr *r = (Expr *) malloc(sizeof(Expr));
  r->flavour = E_while;
  r->u.diad.left = cond;
  r->u.diad.right = e;
  return r;
}

Expr *mkif(Expr *cond, Expr *e1, Expr *e2) {
  Expr *r = (Expr *) malloc(sizeof(Expr));
  r->flavour = E_if;
  r->u.triad.left = cond;
  r->u.triad.middle = e1;
  r->u.triad.right = e2;
  return r;
}

Expr *mkassign(Expr *var, Expr *val) {
  Expr *r = (Expr *) malloc(sizeof(Expr));
  r->flavour = E_assign;
  r->u.diad.left = var;
  r->u.diad.right = val;
  return r;
}

Expr *mkplus(Expr *a, Expr *b) {
  Expr *r = (Expr *) malloc(sizeof(Expr));
  r->flavour = E_plus;
  r->u.diad.left = a;
  r->u.diad.right = b;
  return r;
}

Expr *mkgteq(Expr *a, Expr *b) {
  Expr *r = (Expr *) malloc(sizeof(Expr));
  r->flavour = E_gteq;
  r->u.diad.left = a;
  r->u.diad.right = b;
  return r;
}

Expr *mkaccess(Expr *var) {
  Expr *r = (Expr *) malloc(sizeof(Expr));
  r->flavour = E_access;
  r->u.monad.child = var;
  return r;
}

Expr *mkbracket(Expr *e) {
  Expr *r = (Expr *) malloc(sizeof(Expr));
  r->flavour = E_bracket;
  r->u.monad.child = e;
  return r;
}

Expr *mknumber(int n) {
  Expr *r = (Expr *) malloc(sizeof(Expr));
  r->flavour = E_number;
  r->u.number = n;
  return r;
}

Expr *mkbool(int b) {
  Expr *r = (Expr *) malloc(sizeof(Expr));
  r->flavour = E_bool;
  r->u.bool = b;
  return r;
}

Expr *mkvar(char *var) {
  Expr *r = (Expr *) malloc(sizeof(Expr));
  r->flavour = E_var;
  r->u.var = var;
  return r;
}

Expr *mkskip(void) {
  Expr *r = (Expr *) malloc(sizeof(Expr));
  r->flavour = E_skip;
  return r;
}

void printe(Expr *tree) {
  /* Print an Expr tree in a human-readable format to stdout.
  */
  printf("--- %p ---\n  %s\n", tree, flavours[tree->flavour]);
  switch(tree->flavour) {
  case E_if:
    printf("  left = %p\n  middle = %p\n  right = %p\n",
	   tree->u.triad.left, tree->u.triad.middle, tree->u.triad.right);
    printe(tree->u.triad.left);
    printe(tree->u.triad.middle);
    printe(tree->u.triad.right);
    break;
  case E_seq: case E_while: case E_assign: case E_plus: case E_gteq:
    printf("  left = %p\n  right = %p\n",
	   tree->u.diad.left, tree->u.diad.right);
    printe(tree->u.diad.left);
    printe(tree->u.diad.right);
    break;
  case E_access: case E_bracket:
    printf("  child = %p\n", tree->u.monad.child);
    printe(tree->u.monad.child);
    break;
  case E_number:
    printf("  %d\n", tree->u.number);
    break;
  case E_bool:
    printf("  %s\n", tree->u.bool ? "true" : "false");
    break;
  case E_var:
    printf("  %s\n", tree->u.var);
    break;
  default:
    // E_skip, maybe others later.
    break;
  }
}

void fdote(FILE *fp, Expr *e, int *i) {
  /* Print the parse tree of an expression in DOT format to the given file, so
     that it can be drawn with graphviz tools. 'i' is a pointer to an int
     allocated as the index for the nodes of the graph, when i is 0, that node
     is considered the root of the graph and the appropriate markup wrappers
     will be added.
  */
  int root = *i ? 0 : 1;
  int j = (*i)++;

  if (root)
    fprintf(fp, "graph G {\n  graph [labelloc = \"t\", label =\"Parse Tree\"];\n");
  
  // Node:
  switch (e->flavour) {
  case E_number:
    fprintf(fp, "  node_%d [label=\"%s (%d)\"];\n",
	   j, flavours[e->flavour], e->u.number);
    break;
  case E_bool:
    fprintf(fp, "  node_%d [label=\"%s (%s)\"];\n",
	   j, flavours[e->flavour], e->u.var ? "true" : "false");
    break;
  case E_var:
    // Different format of node id used here so all references to one variable
    // name refer to the same node.
    fprintf(fp, "  node_%s [label=\"%s (%s)\"];\n",
	   e->u.var, flavours[e->flavour], e->u.var);
    break;
  default:
    fprintf(fp, "  node_%d [label=\"%s\"];\n", j, flavours[e->flavour]);
  }

  // Edges:
  switch(e->flavour) {
  case E_if:
    fprintf(fp, "  node_%d -- node_%d [label=\"left\"];\n", j, *i);
    fdote(fp, e->u.triad.left, i);
    fprintf(fp, "  node_%d -- node_%d [label=\"middle\"];\n", j, *i);
    fdote(fp, e->u.triad.middle, i);
    fprintf(fp, "  node_%d -- node_%d [label=\"right\"];\n", j, *i);
    fdote(fp, e->u.triad.right, i);
    break;
  case E_seq: case E_while: case E_plus: case E_gteq:
    fprintf(fp, "  node_%d -- node_%d [label=\"left\"];\n", j, *i);
    fdote(fp, e->u.diad.left, i);
    fprintf(fp, "  node_%d -- node_%d [label=\"right\"];\n", j, *i);
    fdote(fp, e->u.diad.right, i);
    break;
  case E_assign:
    fprintf(fp, "  node_%d -- node_%s [label=\"left\"];\n",
	   j, e->u.diad.left->u.var);
    fdote(fp, e->u.diad.left, i);
    fprintf(fp, "  node_%d -- node_%d [label=\"right\"];\n", j, *i);
    fdote(fp, e->u.diad.right, i);
    break;
  case E_bracket:
    fprintf(fp, "  node_%d -- node_%d [label=\"child\"];\n", j, *i);
    fdote(fp, e->u.monad.child, i);
    break;
  case E_access:
    fprintf(fp, "  node_%d -- node_%s [label=\"child\"];\n",
	   j, e->u.monad.child->u.var);
    fdote(fp, e->u.monad.child, i);
    break;
  default:
    // All other nodes have no edges.
    break;
  }

  if (root)
    fprintf(fp, "}");
}
