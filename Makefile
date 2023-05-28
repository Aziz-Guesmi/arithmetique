flex :	
	flex codeflex.l
bison :	
	bison -d codebison.y

compile : 
	gcc -o prog codebison.tab.c lex.yy.c utils.c

exec : 
	./prog
