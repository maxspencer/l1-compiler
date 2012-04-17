l1c: lex.yy.c y.tab.c y.tab.h syn.c syn.h tab.c tab.h trans.c trans.h
	gcc lex.yy.c y.tab.c syn.c syn.h tab.c tab.h trans.c trans.h -lfl -o $@

lex.yy.c: l1.lex y.tab.c y.tab.h
	lex $<

y.tab.c y.tab.h: l1.yacc
	byacc -d $<
