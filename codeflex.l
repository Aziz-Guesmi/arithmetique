%option noyywrap nodefault yylineno
%{
# include "utils.h"
# include "codebison.tab.h"
%}
EXP ([Ee][-+]?[0-9]+)
%%
"+" |
"-" |
"*" |
"/" |
"=" |
"|" |
"," |
";" |
"(" |
")" { return yytext[0]; }
">" { yylval.fn = 1; return CMP; }
"<" { yylval.fn = 2; return CMP; }
"<>" { yylval.fn = 3; return CMP; }
"==" { yylval.fn = 4; return CMP; }
">=" { yylval.fn = 5; return CMP; }
"<=" { yylval.fn = 6; return CMP; }
"si" { return IF; }
"alors" { return THEN; }
"sinon" { return ELSE; }
"tantque" { return WHILE; }
"finsi" { return ENDIF; }
"fintantque" { return ENDTQ; }
"faire" { return DO; } 
"print" { yylval.fn = print; return PRINT; }
":="  { return AFFECT;}
[a-zA-Z][a-zA-Z0-9]* { yylval.s = lookup(yytext); return NAME; }
[0-9]+"."[0-9]*{EXP}? |
"."?[0-9]+{EXP}? { yylval.d = atof(yytext); return NUMBER; }
"//".*
[ \t] 
\\\n { printf("c> "); }
\n { return EOL; }
. { yyerror("Mystery character %c\n", *yytext); }
%%