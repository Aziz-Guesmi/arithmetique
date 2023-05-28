%{
# include <stdio.h>
# include <stdlib.h>
# include "utils.h"
%}
%union {
 struct ast *a;
 double d;
 struct symbol *s; 
 struct symlist *sl;
 int fn; 
}
%token <d> NUMBER
%token <s> NAME
%token <fn> PRINT
%token EOL
%token IF THEN ELSE WHILE DO ENDIF ENDTQ
%nonassoc <fn> CMP
%right AFFECT
%left '+' '-'
%left '*' '/'
%nonassoc '|' UMINUS
%type <a> exp stmt list explist loopexp
%type <sl> symlist
%start calclist
%%
stmt:
IF exp THEN list { printf("finsi missing"); exit(0); }
 | IF exp THEN list ELSE list  { printf("finsi missing"); exit(0); } 
 | IF exp THEN list ENDIF { $$ = cond_boucle('I', $2, $4, NULL); }
 | IF exp THEN list ELSE list ENDIF { $$ = cond_boucle('I', $2, $4, $6); }
 | DO  list WHILE  loopexp { $$ = cond_boucle('W', $4, $2, NULL); }
 | WHILE loopexp DO list  { printf("fintanque missing"); exit(0); }
 | WHILE loopexp DO list ENDTQ { $$ = cond_boucle('W', $2, $4, NULL); }
 | exp
;
list:  { $$ = NULL; }
 | stmt ';' list { if ($3 == NULL)
 $$ = $1;
 else
 $$ = creeast('L', $1, $3);
 }
 | stmt { $$ = $1; }
 ;
 loopexp :
      '(' loopexp ')' { $$ = $2; }
    | exp CMP exp {    $$ = verifloop($2, $1, $3); }
    | NUMBER { $$ = numero($1); }
    | NAME { $$ = reference($1); }
    
 exp: exp CMP exp { $$ = creecomp($2, $1, $3); }
 | exp '+' exp { $$ = creeast('+', $1,$3); }
 | exp '-' exp { $$ = creeast('-', $1,$3);}
 | exp '*' exp { $$ = creeast('*', $1,$3); }
 | exp '/' exp { $$ = creeast('/', $1,$3); }
 | '|' exp { $$ = creeast('|', $2, NULL); }
 | '(' exp ')' { $$ = $2; }
 | '-' exp %prec UMINUS { $$ = creeast('M', $2, NULL); }
 | NUMBER { $$ = numero($1); }
 | NAME { $$ = reference($1); }
 | NAME AFFECT exp { $$ = affectation($1, $3); }
 | PRINT '(' explist ')' { $$ = fnprint($1, $3); }
;
explist: exp
 | exp ',' explist { $$ = creeast('L', $1, $3); }
;
symlist: NAME { $$ = newsymlist($1, NULL); }
 | NAME ',' symlist { $$ = newsymlist($1, $3); }
;
calclist: 
 | calclist stmt EOL {
 printf("= %4.4g\n> ", eval($2));
 clean($2);
 }
;
 %%
 void yyerror(char *s, ...) {
   fprintf (stderr, "%s\n", s);
 }
int main()
{
 yyparse();
 getchar();
}