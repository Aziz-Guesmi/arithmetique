# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <string.h>
# include <math.h>
# include "utils.h"

static unsigned
symhash(char *sym)
{
 unsigned int hash = 0;
 unsigned c;
 while(c = *sym++) hash = hash*9 ^ c;
 return hash;
}
struct symbol *
lookup(char* sym)
{
 struct symbol *sp = &symtab[symhash(sym)%NHASH];
 int scount = NHASH; 
 while(--scount >= 0) {
 if(sp->name && !strcmp(sp->name, sym)) { return sp; }
 if(!sp->name) { 
 sp->name = strdup(sym);
 sp->value = 0;
 sp->func = NULL;
 sp->syms = NULL;
 return sp;
 }
 if(++sp >= symtab+NHASH) sp = symtab; 
 }
 yyerror("symbol table overflow\n");
 abort(); 
}
struct ast *
creeast(int nodetype, struct ast *l, struct ast *r)
{
 struct ast *a = malloc(sizeof(struct ast));

 if(!a) {
 yyerror("out of space");
 exit(0);
 }
 a->nodetype = nodetype;
 a->l = l;
 a->r = r;
 return a;
}
struct ast *
numero(double d)
{
 struct numval *a = malloc(sizeof(struct numval));

 if(!a) {
 yyerror("out of space");
 exit(0);
 }
 a->nodetype = 'K';
 a->number = d;
 return (struct ast *)a;
}
struct ast *
creecomp(int cmptype, struct ast *l, struct ast *r)
{
 struct ast *a = malloc(sizeof(struct ast));

 if(!a) {
 yyerror("out of space");
 exit(0);
 }
 a->nodetype = '0' + cmptype;
 a->l = l;
 a->r = r;
 return a;
}
struct ast *
verifloop(int cmptype, struct ast *l, struct ast *r)
{
 struct ast *a = malloc(sizeof(struct ast));

 if(!a) {
 yyerror("out of space");
 exit(0);
 }
 a->nodetype = '0' + cmptype;
 a->l = l;
 a->r = r;
 
if ((l->nodetype == 'N' && r->nodetype == 'N')
             && ((struct symref *)l)->s->name == ((struct symref *)r)->s->name ) {
            struct symref  *s1=(struct symref *)l;
            struct symref *s2=(struct symref *)r;
            
        printf("boucle infinie ( %s == %s )",s1->s->name,s2->s->name);
        exit(0);
     }
     else {
        return creecomp(cmptype,l,r);
     }

 return a;
}
struct ast *
fnprint(int functype, struct ast *l)
{
 struct fncall *a = malloc(sizeof(struct fncall));

 if(!a) {
 yyerror("out of space");
 exit(0);
 }
 a->nodetype = 'F';
 a->l = l;
 a->functype = functype;
 return (struct ast *)a;
}
struct ast *
reference(struct symbol *s)
{
 struct symref *a = malloc(sizeof(struct symref));

 if(!a) {
 yyerror("out of space");
 exit(0);
 }
 a->nodetype = 'N';
 a->s = s;
 return (struct ast *)a;
}
struct ast *
affectation(struct symbol *s, struct ast *v)
{
 struct symasgn *a = malloc(sizeof(struct symasgn));
 if(!a) {
 yyerror("out of space");
 exit(0);
 }
 a->nodetype = '=';
 a->s = s;
 a->v = v;
 return (struct ast *)a;
}
struct ast *
cond_boucle(int nodetype, struct ast *cond, struct ast *tl, struct ast *el)
{
 struct flow *a = malloc(sizeof(struct flow));

 if(!a) {
 yyerror("out of space");
 exit(0);
 }
 a->nodetype = nodetype;
 a->cond = cond;
 a->tl = tl;
 a->el = el;
 return (struct ast *)a;
}
void
clean(struct ast *a)
{
 switch(a->nodetype) {
 case '+':
 case '-':
 case '*':
 case '/':
 case '1': case '2': case '3': case '4': case '5': case '6':
 case 'L':
 clean(a->r);
 case '|':
 case 'M': case 'C': case 'F':
 clean(a->l);
 case 'K': case 'N':
 break;
 case '=':
 free( ((struct symasgn *)a)->v);
 break;
 case 'I': case 'W':
 free( ((struct flow *)a)->cond);
 if( ((struct flow *)a)->tl) clean( ((struct flow *)a)->tl);
 if( ((struct flow *)a)->el) clean( ((struct flow *)a)->el);
 break;
 default: printf("erreur: free  %c\n", a->nodetype);
 }

 free(a);
}
struct symlist *
newsymlist(struct symbol *sym, struct symlist *next)
{
 struct symlist *sl = malloc(sizeof(struct symlist));

 if(!sl) {
 yyerror("out of space");
 exit(0);
 }
 sl->sym = sym;
 sl->next = next;
 return sl;
}
void
symlistfree(struct symlist *sl)
{
 struct symlist *nsl;
 while(sl) {
 nsl = sl->next;
 free(sl);
 sl = nsl;
 }
}
static double callbuiltin(struct fncall *);
static double calluser(struct ufncall *);

double eval(struct ast *a)
{
 double v;
 if(!a) {
 yyerror("erreur, null evaluation");
 return 0.0;
 }
 switch(a->nodetype) {
 case 'K': v = ((struct numval *)a)->number; break;
 case 'N': v = ((struct symref *)a)->s->value; break;
 case '=': v = ((struct symasgn *)a)->s->value =
 eval(((struct symasgn *)a)->v); break;
 case '+': v = eval(a->l) + eval(a->r); break;
 case '-': v = eval(a->l) - eval(a->r); break;
 case '*': v = eval(a->l) * eval(a->r); break;
 case '/': v = eval(a->l) / eval(a->r); break;
 case '|': v = fabs(eval(a->l)); break;
 case 'M': v = -eval(a->l); break;
 case '1': v = (eval(a->l) > eval(a->r))? 1 : 0; break;
 case '2': v = (eval(a->l) < eval(a->r))? 1 : 0; break;
 case '3': v = (eval(a->l) != eval(a->r))? 1 : 0; break;
 case '4':
     /*if(a->l->nodetype == 'N' &&a->r->nodetype == 'N') {


     }
     else {*/
      //  printf("%s\n\n",((struct symref *)(a->l))->s->name);
      v = (eval(a->l) == eval(a->r))? 1 : 0; break;
     //}
    
 case '5': v = (eval(a->l) >= eval(a->r))? 1 : 0; break;
 case '6': v = (eval(a->l) <= eval(a->r))? 1 : 0; break;
 /* si/alors/sinon */
 case 'I':
 if( eval( ((struct flow *)a)->cond) != 0) {  //verif condition
 if( ((struct flow *)a)->tl) { // vrai
 v = eval( ((struct flow *)a)->tl);
 } else
 v = 0.0; // par defaut 
 } else {
 if( ((struct flow *)a)->el) { // faux
 v = eval(((struct flow *)a)->el);
 } else
 v = 0.0; // a par defaut  
 }
 break;
 // tantque/faire
 case 'W':
 v = 0.0; 
 if( ((struct flow *)a)->tl) {
 while( eval(((struct flow *)a)->cond) != 0) // verif condition
 v = eval(((struct flow *)a)->tl); 
 }
 break; 


 case 'L': eval(a->l); v = eval(a->r); break;
 case 'F': v = callbuiltin((struct fncall *)a); break;
 case 'C': v = calluser((struct ufncall *)a); break;
 default: printf("erruer: mauvais noeud %c\n", a->nodetype);
 }
 return v;
}
static double
callbuiltin(struct fncall *f)
{
 enum bifs functype = f->functype;
 double v = eval(f->l);
 switch(functype) {
 case print:
 printf("= %4.4g\n", v);
 return v;
 default:
 yyerror("erreur fonction : essaiyez print('a')  %d", functype);
 return 0.0;
 }
}
static double
calluser(struct ufncall *f)
{
 struct symbol *fn = f->s; /*  nom */
 struct symlist *sl; /* arguments */
 struct ast *args = f->l; /*  arguments actuel */
 double *oldval, *newval; /* valeur arg  */
 double v;
 int nargs;
 int i;
 if(!fn->func) {
 yyerror("call to undefined function", fn->name);
 return 0;
 }
 sl = fn->syms;
 for(nargs = 0; sl; sl = sl->next)
 nargs++;
 oldval = (double *)malloc(nargs * sizeof(double));
 newval = (double *)malloc(nargs * sizeof(double));
 if(!oldval || !newval) {
 yyerror("Out of space in %s", fn->name); return 0.0;
 }

 for(i = 0; i < nargs; i++) {
 if(!args) {
 yyerror("too few args in call to %s", fn->name);
 free(oldval); free(newval);
 return 0.0;
 }
 if(args->nodetype == 'L') {
 newval[i] = eval(args->l);
 args = args->r;
 } else { 
 newval[i] = eval(args);
 args = NULL;
 }
 }

 sl = fn->syms;
 for(i = 0; i < nargs; i++) {
 struct symbol *s = sl->sym;
 oldval[i] = s->value;
 s->value = newval[i];
 sl = sl->next;
 }
 free(newval);
 v = eval(fn->func);
 sl = fn->syms;
 for(i = 0; i < nargs; i++) {
 struct symbol *s = sl->sym;
 s->value = oldval[i];
 sl = sl->next;
 }
 free(oldval);
 return v;
}