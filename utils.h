
extern int yylineno; 
void yyerror(char *s, ...);
/*  table de symboles */
struct symbol { /* nom de variable  */
 char *name;
 double value;
 struct ast *func; /* stmt pour fonction */
 struct symlist *syms; /* list de dummy args */
};
#define NHASH 9997
struct symbol symtab[NHASH];
struct symbol *lookup(char*);
struct symlist {
 struct symbol *sym;
 struct symlist *next;
};
struct symlist *newsymlist(struct symbol *sym, struct symlist *next);
void symlistfree(struct symlist *sl);
/* 
 * + - * / |
 * 0-7 comparison ops,  04 egaux, 02 inf, 01 sup
 * M moins unaire
 * L expression de statement list
 * I If 
 * W Tantque 
 * N reference symbole 
 * := affectation
 * S list de symboles
 * F appel fonction
 * C appel fonction utilisateur
 */
enum bifs { 
   print = 1
};

struct ast {
 int nodetype;
 struct ast *l;
 struct ast *r;
};
struct fncall { 
 int nodetype; 
 struct ast *l;
 enum bifs functype;
};
struct ufncall { 
 int nodetype; 
 struct ast *l; 
 struct symbol *s;
};
struct flow {
int nodetype; 
 struct ast *cond;
 struct ast *tl; 
 struct ast *el; 
};
struct numval {
 int nodetype; 
 double number;
};
struct symref {
 int nodetype; 
 struct symbol *s;
};
struct symasgn {
 int nodetype; 
 struct symbol *s;
 struct ast *v; 
};
struct ast *creeast(int nodetype, struct ast *l, struct ast *r);
struct ast *creecomp(int cmptype, struct ast *l, struct ast *r);
struct ast *verifloop(int cmptype, struct ast *l, struct ast *r);
struct ast *fnprint(int functype, struct ast *l);
struct ast *reference(struct symbol *s);
struct ast *affectation(struct symbol *s, struct ast *v);
struct ast *numero(double d);
struct ast *cond_boucle(int nodetype, struct ast *cond, struct ast *tl, struct ast *tr);
double eval(struct ast *);
void clean(struct ast *);
extern int yylineno; 
void yyerror(char *s, ...);
