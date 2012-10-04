/***************************************************
  Project Name: Basic List Processing Interpreter
  Author  Name: Arjun G. Menon  |  Date: 15/12/2005
****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* macro definitions */
#define STR_MAX 1024
#define TRUE  1
#define FALSE 0
/* data type definitions */
typedef signed short int int16;
typedef unsigned short int num16;
typedef signed long int int32;
typedef unsigned long int num32;
typedef signed __int64 int64;
typedef signed __int64 num64;
typedef unsigned char uchar;

/**********************.
| Function Prototypes: |
`**********************/

/* application functions */
void shell(const char *prog);
void msg(const char *str, const uchar mode);
inline void pause_key(void);
/* lexical analysis: */
void runs(const char *str);
void exec(const char *str);
void eval_int32(char *str, int32 *value);
void vals_int32(const char *str, int32 * const value);
int32 * lex_int32(const char *str, int32 *value);
/* memory management: */
inline void free_memory();
num32 new_idHash(const char *id);
num32 get_idHash(const char *id);
int32 * new_int32(struct int32_list * const int32_offset, const num32 id_num);
int32 * get_int32(struct int32_list * int32_offset, const num32 id_num);
char del_int32(struct int32_list * int32_offset, const num32 id_num);
void show_int32(struct int32_list * int32_offset);
void free_int32(struct int32_list * const int32_offset);
int32 * mynewint32(const char *id);
int32 * mygetint32(const char *id);
uchar mydelint32(const char *id);
void exchange_vars(char *lhs, char *rhs, char *err);
/* mathematical functions */
inline uchar eval_prior(char e);
inline num32 factorial(num32 value);
inline void concatenate(int32 *value, int32 rvalue);
inline void mov_int32(int32 *value, int32 *rvalue, char pop_symbol);
/* string-related: */
void trims(char *str, const char T);
inline char * allocs(num32 len);
inline void reallocstr(char *str);
inline char * clones(const char *str);
inline char * twins(const char *str, num32 len);
/* core standard library: */
void * lisp_sqrt(int32 argc, char *argv[]);

/************************.
| Command User Interface |
`************************/

int main(int argc, char *argv[])
{
	atexit(free_memory);
	/* program starts here */
	if(argc<=1) {
		/* there are no arguments, so run the shell */
		shell("LISP");
	} else {
		/* open each file (arg as filename) and run it. */
		FILE *afile=NULL;
		char str[1024];
		int32 i;
		for(i=1;i<argc;i++,afile=NULL) {
			if((afile = fopen(argv[i],"r"))!=NULL) {
			    while(fgets(str,1024,afile)!=NULL) {
					str[strlen(str)-1] = '\0';
					runs(str);
				} fclose(afile);
			} else
			printf("\nCould not open file: %s\n",argv[i]);
		}
	} return EXIT_SUCCESS;
}

void shell(const char *prog)
{
	char str[STR_MAX];
	puts(prog);
	while(TRUE) {
		printf("\n$ ");
		fflush(stdin);
		fgets(str,STR_MAX,stdin);
		str[strlen(str)-1] = '\0';
		trims(str,' ');
		trims(str,'\t');
		if(!strlen(str)) continue;
		if(!stricmp(str,"exit")) break;
		runs(str);
	}
}

/****************************.
| Lexical Analysis Functions |
`****************************/

static char lisp_error[STR_MAX+128];
/* lisp_error is used as an error descriptor string. */

/* global variables */
num32 linenum = 0;
struct /* anonymous */
{ /* global booleans */
	uchar qt:1;
} static bits;
static const char system_no_mem[] = "Not enough memory.";

void runs(const char *str)
{
	if(*str==';'||*str==' '||*str=='\t')
	return;
	linenum += 1;
	/*
	 *  here str is trimmed of all spaces
	 *  and tabs and stored in string and.
	 */
	char *and = clones(str);
	trims(and,' ');
	trims(and,'\t');
	/* eliminate the comments */
	register int32 loop = 0;
	for(;and[loop]!='\0';loop++) {
		if(and[loop]==34||and[loop]==39)
			bits.qt += 1;
		else if(and[loop]=='#'&&!bits.qt)
			and[loop] = '\0';
	}
	reallocstr(and);
	/* now execute "and" */
	lisp_error[0] = '\0';
	exec(and);
	/* now runs() returns */
	if(strlen(lisp_error))
	msg(lisp_error,2);
	free(and);
}

void exec(const char *str)
{
	int32 strn = 0;
	/* check validity of equation in number of brackets */
	int32 brackets = 0;
	for(;*str!='\0';str++,strn++) {
		if(*str=='(')
			brackets++;
		else if(*str==')')
			brackets--;
	} if(brackets!=0) {
		strcpy(lisp_error,"Invalid Syntax, incorrect number of parentheses.");
		return; }
	str -= strn;

	char *lhs = allocs(strn);
	char *rhs = allocs(strn);
	/* here the rhs & lhs are obtained */
	{
	char *sptr = lhs;  /* sptr points at lhs */
	for(;*str!='\0';str++) {
		if(str[1]=='='&&*str!='<'&&*str!='>'
		   &&*str!='='&&str[2]!='=') {
			*sptr++ = *str++;
			break;
		} else { 
			*sptr++ = *str;
		}
	} *sptr = '\0';
	if(*str=='=') {
		for(sptr=rhs,str++;*str!='\0';str++)
		{ *sptr++ = *str;
		  *sptr = '\0'; }
	} else { *rhs = '\0'; }
	trims(lhs,' ');
	}

	if(*rhs=='\0') { /* <- empty rhs, so no assignment involved. */

		/* check if rhs is a variable */
		int32 *value = NULL;
		value = mygetint32(lhs);

		if(value==NULL) {
			/* rhs is NOT a variable */
			int32 val=0;
			eval_int32(lhs,&val);
			printf("\n%ld\n",val);
		}
		else printf("%s = %ld\n",lhs,*value);
	} else { /* rhs exists  */
		/* check for [=>] exchange directive */
		if(*rhs=='>')
			exchange_vars(lhs,rhs,lisp_error);
		else { /* variable creation/assignment */
			void asgn(char *rhs, char *lhs);
			asgn(rhs,lhs);
		}
	}
	/* free memeory */
	free(lhs);
	free(rhs);
}

void asgn(char *rhs, char *lhs)
{
	/*
	 * Variable Assignment Function;
	 * perfroms a variable assignment
	 * on var with names rhs & lhs, then
	 * does the appropriate post-evaluation
	 * operations and if errored, then gives
	 * a description about the error in *err.
	 * ------------------------------------------------
	 * WARNING: Strings *rhs and *lhs are subject to
	 * changes, forget them after calling this function.
	 */

	int32 *value = NULL;
	int32 rvalue = 0;

	/* to find assignment operator */
	char esymbol=0;
	rvalue=strlen(lhs)-1;
	lhs += rvalue;

	/* now find the assignment operator */
	if(*lhs=='*'||*lhs=='/'||
	   *lhs=='+'||*lhs=='-'||
	   *lhs=='%'||*lhs=='^'||
	   *lhs=='~'||*lhs==':'||
	   *lhs=='<'||*lhs=='>'||
	   *lhs=='|'||*lhs=='&'||
	   *lhs=='!'||*lhs=='=') {
		esymbol = *lhs;
		*lhs = '\0';
	} /* set lhs ptr to origin */
	lhs -= rvalue;
		/* evaluate rhs */
	eval_int32(rhs,&rvalue);
	value = mynewint32(lhs);
		/* if variable elready exists, get it. */
	if(value==NULL)
	value = mygetint32(lhs);
		/* set the value */
	if(value!=NULL)
	mov_int32(value,&rvalue,esymbol);
	else  /* getint32() hasn't worked properly */
	strcpy(lisp_error,"Fatal Data Transfer Error.");
	printf("%s = %ld\n",lhs,*value);
}

void eval_int32(char *str, int32 *value)
{
	/*     Expression Evaluation Function (int32)
	 *   ========================================
	 *  This function evaluates an infix expression
	 *  for an int32 result. First, it translates
	 *  the given expression to the postfix
	 *  form, and then evaluates the postfix
	 *  expression using the peval function.
	 *  The result is stored in int32 *value.
	 *  --------------------------------------------
	 *   WARNING: *str is subject to slight changes,
	 *   so transfer expression to temporary string
	 *   before passing it here.
	 */
	trims(str,' ');

	register num32 cur=0;
	/* count the number of operators */
	if(*str=='-') { str++; cur++; }
	num32 stack_size=0;
	for(;*str!='\0';str++,cur++)
	if(*str=='+'||*str=='-'||
	   *str=='^'||*str=='%'||
	   *str=='*'||*str=='/'||
	   *str=='<'||*str=='>'||
	   *str=='&'||*str=='|'||
	   *str==')'||*str=='('||(
	   *str=='='&&str[1]=='='))
	   stack_size++;
	str -= cur;

	/* no operators */
	if(!stack_size) {
	vals_int32(str,value);
	return; }

	/* stacks for storing postfix expression */
	char * const ostack = (char *)
	malloc ((stack_size*2 + 1) * sizeof(char));
	int32 * const values = (int32 *)
	malloc ((stack_size + 1) * sizeof(int32));
	if(ostack==NULL||values==NULL) msg(system_no_mem,3);

	/*  Rules: ostack contains operators(+,-,*..), wheras
	 *  values contains numbers. if char *ostack = 0, then 
	 *  int32 *values is holds a valid number of the expression, 
	 *  otherwise char *ostack contains  a valid math operator. 
	 *  Based on these rules the expression is designed.
	 */

	/* important loop counters... */
	register num32 top=0,ival=0,ops=0;

	/***********************************.
	| ( infix => postfix ) translation: |
	`***********************************/
	{
	/* temporary character array */
	char * const tmp = allocs(strlen(str));
	/* temporary stack to hold operators (+,-,*,..) */
	uchar * const cstack = (uchar *)
	malloc ((stack_size + 1) * sizeof(uchar)+11);
	if(cstack==NULL) msg(system_no_mem,3); *cstack = '\0';
	char prevOp = '\0', negative=0;

	/* beware: mathematics ahead... */
	for(cur=0;;str++)
	{
		if(*str=='('&&cur) {
		/* to handle substring baraces */
			register int32 pars = 0;
			for(;*str!='\0';str++) {
				if(*str=='(') pars++;
				if(*str==')') pars--;
				if(!pars) break;
				tmp[cur++] = *str;
			} if(*str=='\0') break;
			else tmp[cur++] = ')';
			continue;
		}
		if(*str=='+'||*str=='-'||
		   *str=='*'||*str=='/'||
		   *str=='^'||*str=='%'||
		   *str=='<'||*str=='>'||
		   *str=='('||*str==')'||
		   *str=='&'||*str=='|'||
		   *str=='='||*str=='\0') {
			/* finalise tmp */
			tmp[cur] = '\0';
			trims(tmp,' ');
			for(cur=0;tmp[cur]!='\0';cur++);
			/* skip vals for opening braces */
			if(*str=='(')
				goto after_vals;
			if(!cur) {
				/* skip the translation in
		        case of repition of operators */
				if(prevOp==')'||prevOp=='(')
					goto after_vals;
				else {
					if(*str=='-')
						negative++;
					prevOp = *str;
					if(*str=='\0')
						break;
					continue;
				}
			}
			/* doubly operators */
			if(*str=='='&&str[1]=='=')
				/* logical == */
				*++str='E';
			else if(*str=='>'&&str[1]=='=')
				/* logical >= */
				*++str='F';
			else if(*str=='<'&&str[1]=='=')
				/* logical <= */
				*++str='G';
			else if(*str=='&'&&str[1]=='&')
				/* logical AND */
				*++str='A';
			else if(*str=='|'&&str[1]=='|')
				/* logical OR */
				*++str='O';
			else if(*str=='>'&&str[1]=='>')
				/* bitwise right shift (>>) */
				*++str='R';
			else if(*str=='<'&&str[1]=='<')
				/* bitwise left shift (<<) */
				*++str='L';
			else if(*str=='^'&&str[1]=='^')
				/* bitwise exclusive or (^) */
				*++str='X';
			else if(*str=='>'&&str[1]=='<')
				/* maximum value */
				*++str='M';
			else if(*str=='+'&&str[1]=='&')
				/* concatenate */
				*++str='C';
			/* number insertion: */
			vals_int32(tmp,&values[ival++]);
			if(negative) {
			values[ival-1]*=-1;
			negative = 0; }
			ostack[ops++] = 0;
			cur = *tmp = 0;
			after_vals:
			/* operand insertion: */
			if(eval_prior(cstack[top])<eval_prior(*str)
			||*str=='(') cstack[++top] = *str;
			else {
				for(;top&&cstack[top]!='(';top--)
					ostack[ops++] = cstack[top];
				if(*str!=')')
					cstack[++top] = *str;
				else if(cstack[top]=='(')
					cstack[top--] = 0;
			} if(*str=='\0') break;
			prevOp = *str;
		} else tmp[cur++] = *str;
	}
	/* free memory: */
	free(cstack);
	free(tmp);
	}
	
	printf("\n---[%ld]---\n",ops);
	for(cur=0,top=0;cur<ops;cur++)
	if(!ostack[cur]) printf(" %ld ",values[top++]);
	else printf(" %c ",ostack[cur]);
	puts("\n-----------");
	
	/*******************************.
	| Postfix Expression Evaluator: |
	`*******************************/
	{
	/* temporary stack to evaluate expression */
	int32 * const stack = (int32 *)
	malloc ((stack_size + 1) * sizeof(int32));
	if(stack==NULL) msg(system_no_mem,3);

	for(cur=0,top=0,ival=0;cur<ops;cur++)
	{
		if(!ostack[cur]) /* an operand */
			stack[ival++] = values[top++];
		else { /* an operator (+,-,*,..)[15] */
		switch(ostack[cur]) { /* do the math */
		case '+': stack[--ival-1] += stack[ival]; break;
		case '-': stack[--ival-1] -= stack[ival]; break;
		case '*': stack[--ival-1] *= stack[ival]; break;
		case '/': stack[--ival-1] /= stack[ival]; break;
		case '%': stack[--ival-1] %= stack[ival]; break;
		/* the logical operators (==,>=,&&,||,..) */
		case 'E': stack[ival---2] = stack[ival-1] == 
		          stack[ival-2] ? 1 : 0; break; /* == */
		case '>': stack[ival---2] = stack[ival-1] < 
		          stack[ival-2] ? 1 : 0; break; /* > */
		case 'F': stack[ival---2] = stack[ival-1] >= 
		          stack[ival-2] ? 1 : 0; break; /* >= */
		case '<': stack[ival---2] = stack[ival-1] > 
		          stack[ival-2] ? 1 : 0; break; /* < */
		case 'G': stack[ival---2] = stack[ival-1] >= 
		          stack[ival-2] ? 1 : 0; break; /* <= */
		case 'A': stack[ival---2] = stack[ival-1] && 
		          stack[ival-2] ? 1 : 0; break; /* && */
		case 'O': stack[ival---2] = stack[ival-1] ||
		          stack[ival-2] ? 1 : 0; break; /* || */
		/* the bitwise operators (&,|,^^,>>,<<,..)  */
		case '&': stack[--ival-1] &= stack[ival]; break;
		case '|': stack[--ival-1] |= stack[ival]; break;
		case 'X': stack[--ival-1] ^= stack[ival]; break;
		case 'L': stack[ival-1] = /* bitwise left shift */
		          stack[ival-1] << stack[--ival]; break;
		case 'R': stack[ival-1] = /* bitwise right shift */
		          stack[ival-1] >> stack[--ival]; break;
		/* the special operators (^,+&,><)...  */
		case '^': stack[--ival-1] = (int32) pow
		         (stack[ival-2],stack[ival-1]); break;
		case 'C': concatenate(&stack[ival-1],stack[--ival]); break;
		case 'M': stack[ival---2] = stack[ival-1] > stack[ival-2] ?
		          stack[ival-1] : stack[ival-2]; break;
		default:  stack[--ival-1] = stack[ival]; }
		}
	}
	/* set value */
	*value = stack[0];
	free(stack);
	}
	/* free memory: */
	free(ostack);
	free(values);
}

inline uchar eval_prior(char e)
{
	/* used to find operator priority */
	if(e=='(')               return 12;
	else if(e=='<'||e=='>')  return 11;
	else if(e=='F'||e=='G')  return 10;
	else if(e=='A')          return  9;
	else if(e=='O')          return  8;
	else if(e=='^')          return  7;
	else if(e=='%')          return  6;
	else if(e=='*'||e=='/')  return  5;
	else if(e=='+'||e=='-')  return  4;
	else if(e=='&')          return  3;
	else if(e=='|')          return  2;
	else if(e=='X')          return  1;
	else                     return  0;
}

void vals_int32(const char *str, int32 * const value)
{
	/*     Elementry Evaluator (int32)
	 *   ==============================
	 *  This function evaluates, the int32
	 *    value of an exprssion element.
	 *  Operations ~,!,!! and factorial are carried
	 *  out here. Function calls are made and
	 *  values of variables are retrieved too.
	 *  ---------------------------------------
	 *   Note: *str sould be trimmed of spaces
	 *   before being passed here.
	 */
	*value = 0;

	/* Out the 4 operations (!,!!,~,fact),
	 * only 3 operations ~,! and !! appear
	 * on L.H.S. of str, where ! and !! can
	 * never coincide, but ~ can coincide with
	 * the other two. These operations can be
	 * permuted in the following ways:
	 * ------------------------------------
	 *  nothing    -> 0 | !  only    -> 1 
	 *  !! only    -> 2 | ~  only    -> 3
	 *  ~ and !    -> 4 | ~ and !!   -> 5
	 * - - - - - - - - - - - -
	 * in case factorial needs
	 * to be found then add 10:
	 * - - - - - - - - - - - -
	 *  nothing    -> 10 | !  only    -> 11
	 *  !! only    -> 12 | ~  only    -> 13
	 *  ~ and !    -> 14 | ~ and !!   -> 15
	 * -------------------------------------
	 * The number is kept in ops.
	 */
	uchar ops = 0;

	/* now find ops */
	if(*str=='~'||*str=='!') {
		if(*str++=='~') {
			ops += 3;
			if(*str++=='!') {
				ops++;
				if(*str=='!')
				{ ops++; str++; }
			} else str--;
		} else { /* (*str=='!') */
			ops++;
			if(*str++=='!') ops++; 
			else str--;
		}
	} if(str[strlen(str)-1]=='!')
		ops += 10;
	//printf("lex: %s -> %d\n",str,ops);

	/* extract the value */
	if((*str=='0')&&(str[1]=='x'||str[1]=='X'))
	/* hexadecimal */ sscanf(str,"%lx",value);
	else if((*str>='0'&&*str<='9')||
	(*str=='-'&&str[1]>='0'&&str[1]<='9'))
	/* decimal */ *value = atol(str);
	else {
		/* get the value */
		int32 * var_ptr;
		var_ptr = lex_int32(str, value);
		if(var_ptr) {
			/* operate on the value */
			if(ops==1||ops==2||ops==4||ops==5||
			ops==11||ops==12||ops==14||ops==15) {
				/* boolean negation */
				int32 *tmp;
				tmp = mygetint32(str);
				if(tmp!=NULL)
					*tmp = 0;
				if(ops==2||ops==5||
				   ops==12||ops==15) {
					/* boolean negation & nullify variable */
					mydelint32(str);
					printf("\ndel %s\n",str);
				}
			}
		}
	}
	/* find factorial & complement if needed */
	if(ops>=10) {
		*value = factorial(*value);
		ops -= 10;
	} if(ops>=3)
		*value = ~*value;
}

int32 * lex_int32(const char *str, int32 *value)
{
	/*   Abstract Lexical Analysis Function
	 *  ------------------------------------
	 *  lex peforms abstract lexical analysis.
	 *  It accepts abstract entities, such as
	 *  function calls, variable names, e.t.c
	 *  for analysis. Assignment operations,
	 *  definitions, declarations, constants
	 *  (strings, numbers,..) are NOT accepted.
	 *
	 *  First it analyses a string, and finds
	 *  out it's value (if there exists one).
	 *  The value should be a int32. If not,
	 *  then it will be typecasted to int32.
	 *
	 *  If *str describes some pointable matter
	 *  (eg. variable names), then a pointer to
	 *  the variable is returned and the int32
	 *  pointed to by *lex is give it's value.
	 *
	 *  If the *str describes volatile matter
	 *  (eg. functions returns), then it is
	 *  evaluated & stored in the variable
	 *  pointed at by *lex.
	 *
	 *  Whereas if no value can be found, a
	 *  description of the error is written
	 *  into *err & the NULL is returned.
	 */

	// <-------------- NOT COMPLETED ------------------>

	/* junk: , delete this code... */
	*value = (mygetint32(str)==NULL) ? 0:*mygetint32(str);
	const char fn_sqrt[] = "sqrt";
	if(!strncmp(str,fn_sqrt,strlen(fn_sqrt))) {
		str +=  strlen(fn_sqrt);
		for(;(*str<='0'||*str>='9')&&str!='\0';str++);
		*value = atoi(str); printf("\n%s -> %ld\n",
		fn_sqrt,*value); *value = (int32) sqrt(*value);
	} return (mygetint32(str)==NULL) ? NULL : mygetint32(str);

	// <-------------- NOT COMPLETED ------------------>
}

/*********************************.
| Data Storage & Retrieval System |
`*********************************/

/*  There are 6 basic data types in my language:
 *  ---------------------------------
 *  null <-- void   => 0x00
 *  int  <-- int32  => 0x01
 *  num  <-- num32  => 0x02
 *  real <-- double => 0x03
 *  chr  <-- char   => 0x04
 *  byte <-- uchar  => 0x05
 *  ---------------------------------
 *  These set of definitions simplify things:
 */
 #define DT_NULL  0x00
 #define DT_INT   0x01
 #define DT_NUM   0x02
 #define DT_REAL  0x03
 #define DT_CHR   0x04
 #define DT_BYTE  0x05
 /* note: DT means Data Type */

struct int32_list {
	num32 id_num;
	int32 * value;
	int32 array_size;
	struct int32_list * next;
} static int32my;

int32 * mynewint32(const char *id)
{
	num32 address = 0;
	address = new_idHash(id);
	if(!address)
		return NULL;
	return new_int32(&int32my,address);
}

int32 * mygetint32(const char *id)
{
	num32 address = 0;
	address = get_idHash(id);
	if(!address)
		return NULL;
	return get_int32(&int32my,address);
}

uchar mydelint32(const char *id)
{
	num32 address = 0;
	address = get_idHash(id);
	if(!address)
		return 1;
	if(new_int32(&int32my,address))
		return 2;
	return 0;
}

struct struct_idHashTable
{
	char *str;  /* hash string */
	num32 key;  /* hash value */
};
struct struct_idHashTable * idHashTable = NULL;
static num32 idHashTable_size = 0;

num32 new_idHash(const char *id)
{
	if((get_idHash(id)))
		return 0;
	/* increment the hash table size 
	   by 1 for 1 new variable */
	idHashTable_size += 1;	

	/* reallocate the hash table with the new size */
	{ struct struct_idHashTable * table_ptr = NULL;
	if((table_ptr = (struct struct_idHashTable *)realloc(idHashTable,
	sizeof(struct struct_idHashTable)*idHashTable_size)) == NULL)
	msg(system_no_mem,3); else idHashTable = table_ptr; }

	/* set the variable name & key */
	idHashTable[idHashTable_size-1].str = clones(id);
	idHashTable[idHashTable_size-1].key = idHashTable_size;

	/* return a pointer to the variable */
	return idHashTable[idHashTable_size-1].key;
}

num32 get_idHash(const char *id)
{
	/* check if the hash table is empty */
	if(!idHashTable_size)
		return NULL;

	/* find out the location of the variable */
	register num32 index = 0;
	for(;index < idHashTable_size; index++)
		if(!strcmp(idHashTable++ ->str, id))
			break;
	idHashTable -= index + 1;

	/* check if the variable exists */
	if(index >= idHashTable_size)
	{ idHashTable++; return 0; }

	/* return a pointer to the variable */
	return idHashTable[index].key; 
}

void free_idHash(void)
{
	free(idHashTable);
	printf("\n%lu objects freed.\n",idHashTable_size);
}

int32 * new_int32(struct int32_list * const int32_offset, const num32 id_num)
{
	/* malloc a new int32_list */
	struct int32_list * new_var = NULL;
	if((new_var = (struct int32_list*) malloc
	(sizeof(struct int32_list))) == NULL)
	msg(system_no_mem,3);

	/* setup the elements of list */
	new_var ->id_num = id_num;
	new_var ->array_size = 1;
	new_var ->next = NULL;
	new_var ->value = (int32 *) malloc
	(sizeof(int32)*new_var->array_size);

	/* link and return the variable */
	new_var ->next = int32_offset ->next;
	int32_offset ->next = new_var;
	return int32_offset ->next ->value;
}

int32 * get_int32(struct int32_list * int32_offset, const num32 id_num)
{
	/* get an int32 from a list & return it's pointer. */
	while(int32_offset ->next) {
		int32_offset = int32_offset ->next;
		if(int32_offset ->id_num == id_num)
			return int32_offset ->value;
	} return NULL;
}

char del_int32(struct int32_list * int32_offset, const num32 id_num)
{
	/* delete a variable an int32 list, if ok; return 0 else 1. */
	struct int32_list * old_var;
	while(int32_offset ->next) {
		if(int32_offset ->next ->id_num == id_num) {
			old_var = int32_offset ->next;
			int32_offset ->next = old_var ->next;
			free(old_var);
			return 0; /* ok */
		}
		int32_offset = int32_offset ->next;
	} return 1;  /* failed */
}

void free_int32(struct int32_list * const int32_offset)
{
	/* delete all the variables in the int32 list. */
	struct int32_list * old_var;
	while(int32_offset ->next) {
		old_var = int32_offset ->next;
		int32_offset ->next = old_var ->next;
		free(old_var);
	}
}

void show_int32(struct int32_list * int32_offset)
{
	while(int32_offset ->next) {
	int32_offset = int32_offset ->next;
	printf("%ld => %ld := %ld = [%lu] %lu\n",int32_offset,
	int32_offset ->next,int32_offset ->value,
	int32_offset ->id_num,*(int32_offset ->value));
	}
}

void exchange_vars(char *lhs, char *rhs, char *err)
{
	/*
	 * Variable Exchange Function, swaps the value 
	 * stored in two variable lhs & rhs. If they
	 * don't exist error message is stored in *err,
	 * and function is returned.
	 * ------------------------------------------------
	 * WARNING: Strings *rhs and *lhs are subject to
	 * changes, forget them after calling this function.
	 */  
	rhs++; /* to avoid '>' */
	trims(rhs,' ');
	/* now exchange */
	int32 *value = mygetint32(rhs);
	if(value!=NULL) {
		/* okay, rhs is valid */
		int32 rvalue = *value;
		value = mygetint32(lhs);
		if(value!=NULL) {
			/* now swap the values */
			* mygetint32(rhs) = * value;
			* value = rvalue;
			printf("%s => %s\n",lhs,rhs);
		} else
		sprintf(err,"LHS Variable '%s' does not exist.",lhs);
	} else
	sprintf(err,"RHS Variable '%s' does not exist.",rhs);
}

inline void free_memory()
{
	void free_int32(struct int32_list * const int32_offset);
	free_int32(&int32my);
	void free_idHash(void);
	free_idHash();
}

/************************.
| Mathematical Functions |
`************************/

inline void concatenate(int32 *value, int32 rvalue)
{
	// concatenates two integers.
	int32 _rvalue;
	_rvalue = rvalue;
	do { _rvalue/= 10;
	*value *= 10; }
	while(_rvalue!=0);
	*value += rvalue;
}

inline num32 factorial(num32 value)
{
	// finds factorial(!) of an integer
	int32 factorial=1;
	for(;value>0;value--)
	factorial = factorial * value;
	return factorial;
}

inline void mov_int32(int32 *value, int32 *rvalue, char pop_symbol)
{
	/* integer transfer function */
	if(pop_symbol) {
		/* perform post-evaluation operations*/
		switch(pop_symbol) {
			case '*': *value *= *rvalue; break;
			case '/': *value /= *rvalue; break;
			case '+': *value += *rvalue; break;
			case '-': *value -= *rvalue; break;
			case '%': *value %= *rvalue; break;
			case '&': *value &= *rvalue; break;
			case '|': *value |= *rvalue; break;
			case '<': *value<<= *rvalue; break;
			case '>': *value>>= *rvalue; break;
			case '~': *value  = ~*rvalue; break;
			case '!': *value  = factorial(*rvalue); break;
			case '^': *value  = (int32)pow(*value,*rvalue); break;
			case '=': *value  = labs(*rvalue); break;
			case ':': concatenate(value,*rvalue); break;
			default: *value = *rvalue;
		}
	} else /* there is no post-evaluation operator */
	*value  = *rvalue; /* this _just_ sets the value */
}

/*********************.
| String Manupilation |
`*********************/

void trims(char *str, const char T)
{
	/* removes a character(T) from 
	   left & right sides of a string. */
	num16 rt,lt;
	for(rt=0;;str++)
		if(*str==T) rt++;
		else break;
	for(lt=0;*str;str++) {
		if(*str==T) lt++;
		else if(*str) lt=0;
		str[-rt]=*str;
	} str[-rt-lt]='\0';
}

inline char * allocs(num32 len)
{
	/* allocates an empty string 
	   of length (int32 len). */
	char *str = (char *) malloc(len+1);
	if(str==NULL) msg(system_no_mem,3);
	*str = '\0';
	return str;
}

inline void reallocstr(char *str)
{
	/* shrink a string's size in memory 
	   according to it's length. Works
	   only for malloc()ed strings. */
	char *cp = NULL;
	const num32 len = strlen(str) + 1;
	str = (char *) realloc(str,len);
	if((cp = (char *)(realloc(str,
	(len))))!=NULL) str = cp;
	else msg(system_no_mem,3);
}

inline char * clones(const char *str)
{
	/* allocates a copy of string *str & returns it. */
	const num32 len = strlen(str) + 1;
	char *strc = (char *) malloc(len);
	if(strc==NULL)
		msg(system_no_mem,3);
	strcpy(strc,str);
	return strc;
}

inline char * twins(const char *str, num32 len)
{
	/* allocates a _strncpy_ of *str & returns it's address */
	if(!len) return NULL;
	char *strc = (char *) malloc(len+1);
	if(strc==NULL)
		msg(system_no_mem,3);
	strncpy(strc,str,len);
	strc[len]='\0';
	return strc;
}

/****************************.
| Core LISP Standard Library |
`****************************/
void * lisp_sqrt(int32 argc, char *argv[])
{
	return NULL;
}

/***********************.
| Application Functions |
`***********************/

inline void pause_key(void)
{ fflush(stdin); getc(stdin); }

void msg(const char *str, const uchar mode)
{
	/*
	 *  Interpreter Messenging Function
	 * ---------------------------------
	 *  This function is used by the 
	 *  interpreter to relay messages to
	 *  the user such as error reports,
	 *  warning, notices, e.t.c. The
	 *  number in const mode defines the
	 *  nature of message in str,
	 *  and has a special meaning
	 *  for each of these values:
	 *
	 *  0 - Notice         (return)
	 *  1 - Warning        (return)
	 *  2 - User Error     (return)
	 *  3 - Program Error  (exit:1)
	 *  4 - System Error   (exit:2)
	 *  5 - Critical Error (abort)
	 *  ? - [ return ]
	 */
	switch(mode) {
	case 0: printf("\nNotice: %s\n",str ); break;
	case 1: printf("\nWarning: %s\n",str); break;
	case 2: printf("\nError: %s\n",str  ); break;
	case 3: pause_key();          exit(1); break;
	case 4: pause_key();          exit(2); break;
	case 5: /*!!!*/               abort(); break;
	default: return;             /* do nothing */
	}
}
