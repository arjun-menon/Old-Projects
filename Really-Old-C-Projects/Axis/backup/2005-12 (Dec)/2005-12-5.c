/***************************************************
  Project Name: Basic List Processing Interpreter
  Author  Name: Arjun G. Menon  |  Date: 5/12/2005
****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/* macro definitions */
#define int16   signed short int
#define num16   unsigned short int
#define int32   signed long int
#define num32   unsigned long int
#define int64   signed __int64
#define num64   signed __int64
#define uchar   unsigned char
#define forever while(1)
#define STR_MAX 1024

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
num32 setaddress(const char *str);
num32 getaddress(const char *str);
num32 deladdress(const char *str);
int32 * newint32(const char *id);
int32 * getint32(const char *id);
void free_memory();
inline void exchange_vars(char *lhs, char *rhs, char *err);
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
	forever {
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
	char *sptr = NULL;  /* sptr points at const *str */
	for(sptr=lhs;*str!='\0'&&*str!='=';str++)
		*sptr++ = *str;
	*sptr = '\0';
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
		value = getint32(lhs);

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
	value = newint32(lhs);
		/* if variable elready exists, get it. */
	if(value==NULL)
	value = getint32(lhs);
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
	   *str==')'||*str=='(')
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

	/* beware: deep mathematics ahead... */
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
		   *str=='\0') {
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
			/* bitwise and (&) */
			if(*str=='&'&&str[1]=='&')
				*++str='A';
			/* bitwise or (|) */
			if(*str=='|'&&str[1]=='|')
				*++str='O';
			/* bitwise xor (^) */
			if(*str=='^'&&str[1]=='^')
				*++str='X';
			/* number insertion: */
			vals_int32(tmp,&values[ival++]);
			if(negative) {
			values[ival-1]*=-1;
			negative = 0; }
			ostack[ops++] = 0;
			cur = *tmp = 0;
			after_vals:
			/* opernad insertion: */
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
	
	//printf("\n---[%ld]---\n",ops);
	//for(cur=0,top=0;cur<ops;cur++)
	//if(!ostack[cur]) printf(" %ld ",values[top++]);
	//else printf(" %c ",ostack[cur]);
	//puts("\n-----------");
	
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
		else { /* an operator (+,-,*,..) */
		switch(ostack[cur]) { /* do the math */
		case '+': stack[--ival-1] += stack[ival]; break;
		case '-': stack[--ival-1] -= stack[ival]; break;
		case '*': stack[--ival-1] *= stack[ival]; break;
		case '/': stack[--ival-1] /= stack[ival]; break;
		case '%': stack[--ival-1] %= stack[ival]; break;
		/* the bitwise operators (&,|,^,>>,<<)...  */
		case 'A': stack[--ival-1] &= stack[ival]; break;
		case 'O': stack[--ival-1] |= stack[ival]; break;
		case 'X': stack[--ival-1] ^= stack[ival]; break;
		case '<': stack[ival-1] = /* bitwise left shift */
		          stack[ival-1] << stack[--ival]; break;
		case '>': stack[ival-1] = /* bitwise right shift */
		          stack[ival-1] >> stack[--ival]; break;
		/* the special operators (^,&,|)...  */
		case '^': stack[--ival-1] = (int32) pow
		         (stack[ival-2],stack[ival-1]); break;
		case '&': concatenate(&stack[ival-1],stack[--ival]); break;
		case '|': stack[ival---2] = stack[ival-1] > stack[ival-2] ?
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
	 * ----------------------
	 *  nothing    -> 0
	 *  !  only    -> 1
	 *  !! only    -> 2
	 *  ~  only    -> 3
	 *  ~ and !    -> 4
	 *  ~ and !!   -> 5
	 * - - - - - - - - - - - -
	 * in case factorial needs
	 * to be found then add 10:
	 * - - - - - - - - - - - -
	 *  nothing    -> 10
	 *  !  only    -> 11
	 *  !! only    -> 12
	 *  ~  only    -> 13
	 *  ~ and !    -> 14
	 *  ~ and !!   -> 15
	 * ----------------------
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
				/* nullify variable */
				int32 *tmp;
				tmp = getint32(str);
				if(tmp!=NULL)
					*tmp = 0;
				if(ops==2||ops==5||
				   ops==12||ops==15) {
					/* destroy variable */
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
	*value = (getint32(str)==NULL) ? 0:*getint32(str);
	return (getint32(str)==NULL) ? NULL : getint32(str);

	return NULL;
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

struct struct_symbol_table
{
	char * string;
	num32 index;
	struct struct_symbol_table * link;
};
static struct struct_symbol_table * symbol_table_offset;

inline void initialize_symbol_table(void)
{
	symbol_table_offset = (struct struct_symbol_table *)
	malloc(sizeof(struct struct_symbol_table));
	if(!symbol_table_offset) msg(system_no_mem,3);
	symbol_table_offset ->index = 0;
	symbol_table_offset ->link = NULL;
	symbol_table_offset ->string = 0;
	//printf("table made at: %ld\n\n",symbol_table_offset);
}

num32 setaddress(const char *str)
{
	/* adds a new string to the symbol
	 * table and returns it's index.
	 */
	if(!symbol_table_offset)
	initialize_symbol_table();
	struct struct_symbol_table
	* linkage = symbol_table_offset, * pointer = NULL;

	while (linkage ->link)
	linkage = linkage ->link;
	num32 index = linkage ->index;
	linkage ->link = (struct struct_symbol_table *)
	malloc(sizeof(struct struct_symbol_table));
	if(linkage ->link==NULL) msg(system_no_mem,3);

	linkage = linkage ->link;
	linkage ->link = NULL;
	linkage ->index = index + 1;
	linkage ->string = clones(str);
	return linkage ->index;
}

num32 getaddress(const char *str)
{
	/*  searches the symbol table for
	 *  the string *str, and if found
	 *  returns it's index (>0), if not
	 *  found returns 0.
	 */
	struct struct_symbol_table * linkage = symbol_table_offset;
	
	while (linkage ->link) {
		linkage = linkage ->link;
		if(!strcmp(linkage ->string,str))
			return linkage ->index; /* gotcha! */
	}
	return 0; /* <- str does not exist */
}

num32 deladdress(const char *str)
{
	/*  searches the symbol table for
	 *  the string *str, and if found
	 *  deletes the entry from the symbol
	 *  table then returns it's erstwhile
	 *  index (>0), if not found returns 0.
	 */
	struct struct_symbol_table
	* linkage = symbol_table_offset, * pointer = NULL;

	while (linkage ->link) {
		if(strcmp(linkage ->link ->string,str)) {
			pointer = linkage ->link ->link;
			free(linkage ->link);
			linkage ->link = pointer;
			return 0;
		}
	} return 1;  /* <- str does not exist */
}

struct int32_list
{
	int32 address;
	int32 * value;
	int32 var_size;
	struct int32_list * link;
};

struct int32_table_struct
{
	char *ids;  /* variable identifier */
	int32 var;  /* variable value pointer */
};
struct int32_table_struct * int32_table = NULL;
static num32 int32_table_size = 0;

int32 * newint32(const char *id)
{
	/*  This function creates a new integer
	 *  whose name is defined by the string *id,
	 *  and returns an integer pointer to it.
	**/
	/*  --- WARNING --- [ CHECK *id VALIDITY BEFORE PASSING ]
	 * newint() does not check the validity of variable names,
	 * _even_ null variable names, are accepted. The caller
	 * should check the validity of the string being passed.
	 */
	/* check if a variable with the same name exists */
	if((getint32(id))!=NULL) return NULL;
	struct int32_table_struct * table_ptr;
	/* increment table size by 1 for 1 new variable */
	int32_table_size += 1;
	/* reallocate table structure with new size */
	if((table_ptr = (struct int32_table_struct *) 
	realloc (int32_table, sizeof(struct int32_table_struct)*
	int32_table_size))==NULL) msg(system_no_mem,3);
	else /* set the variable */
	int32_table = table_ptr;
	/* set the variable name */
	int32_table[int32_table_size-1].ids = clones(id);
	/* initiaze the variable to 0 */
	int32_table[int32_table_size-1].var = 0;
	/* return a pointer to the variable */
	return &int32_table[int32_table_size-1].var;
}

int32 * getint32(const char *id)
{
	/*  This function returns a integer pointer to an
	 *  existing integer whose is name is string *id.
	**/
	/* check if integer table is empty */
	if(!int32_table_size) return NULL;
	/* loop counter to find position in array */
	num32 table_pos = 0;
	/* find out the location of the variable */
	for(;table_pos<int32_table_size;table_pos++)
	if(!strcmp(int32_table++ ->ids,id)) break;
	/* reverse int32_table pointer to origin  */
	int32_table -= table_pos+1;
	/* check if the variable exists */
	if(table_pos>=int32_table_size) {
		int32_table++; return NULL; }
	/* return a pointer to the variable */
	return &int32_table[table_pos].var;
}

inline void exchange_vars(char *lhs, char *rhs, char *err)
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
	int32 *value = getint32(rhs);
	if(value!=NULL) {
		/* okay, rhs is valid */
		int32 rvalue = *value;
		value = getint32(lhs);
		if(value!=NULL) {
			/* now swap the values */
			* getint32(rhs) = * value;
			* value = rvalue;
			printf("%s => %s\n",lhs,rhs);
		} else
		sprintf(err,"LHS Variable '%s' does not exist.",lhs);
	} else
	sprintf(err,"RHS Variable '%s' does not exist.",rhs);
}

void free_memory()
{
	printf("\n%ld variables freed.\n",int32_table_size);
	free(int32_table);
}

/************************.
| Mathematical Functions |
`************************/

inline uchar eval_prior(char e)
{
	/* used to find operator priority */
	if(e=='(')               return 9;
	else if(e=='%')          return 8;
	else if(e=='^')          return 7;
	else if(e=='*'||e=='/')  return 6;
	else if(e=='+'||e=='-')  return 5;
	else if(e=='&')          return 4;
	else if(e=='<'||e=='>')  return 3;
	else if(e=='A'||e=='O')  return 2;
	else if(e=='X')          return 1;
	else /* if(e==')') */    return 0;
}

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
