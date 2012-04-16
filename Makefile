l1c: lex.yy.c y.tab.c y.tab.h parsetree.c parsetree.h tab.c tab.h trans.c trans.h
	gcc lex.yy.c y.tab.c parsetree.c parsetree.h tab.c tab.h trans.c trans.h -lfl -o $@

lex.yy.c: l1.l y.tab.c y.tab.h
	lex $<

y.tab.c y.tab.h: l1.y
	byacc -d $<
