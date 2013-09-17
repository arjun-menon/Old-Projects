/***************************************************
  Project Name: Basic List Processing Interpreter
  Author  Name: Arjun G. Menon  |  Date: 1/12/2005
****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define STR_MAX 1024

/**********************.
| Function Prototypes: |
`**********************/

void msg(const char *str, const unsigned char mode);
/* lexical analysis */
void runs(const char *str);
void eval_long(char *str, long int *value);
void vals_long(const char *str, long int *value);
inline unsigned char eval_prior(char e);
/* memory management: */
unsigned long int setaddress(const char *str);
unsigned long int getaddress(const char *str);
unsigned long int deladdress(const char *str);
long int * newint(const char *id);
long int * getint(const char *id);
void free_memory();
inline void exchange_vars(char *lhs, char *rhs, char *err);
/* mathematical functions */
inline unsigned long int factorial(unsigned long int value);
inline void concatenate(long int *value, long int rvalue);
inline void mov_int(long int *value, long int *rvalue, char pop_symbol);
/* string-related: */
void trims(char *str, const char T);
inline char * allocs(unsigned long int len);
inline void reallocstr(char *str);
inline char * clones(const char *str);
inline char * twins(const char *str, unsigned long int len);

/************************.
| Command User Interface |
`************************/

int main(int argc, char *argv[])
{
	atexit(free_memory);
	/* program starts here */
	if(argc<=1) {
		/* there are no arguments, so run the shell */
		void shell(char *);
		shell("LISP");
	} else {
		/* open each file (arg as filename) and run it. */
		FILE *afile=NULL;
		char str[1024];
		long int i;
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

void shell(char *name)
{
	setaddress("i");
	setaddress("am");
	setaddress("arjun");
	getaddress("yes");
	deladdress("am");
	deladdress("i");
	setaddress("menon");
	getaddress("arjun");

	char str[STR_MAX];
	puts(name);
	while(1) {
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

/* global variables */
unsigned long int linenum = 0;
struct common_bits
{ /* global booleans */
	unsigned char qt:1;
} bits;
char no_mem[] = "Not enough memory.";

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
	register long int loop = 0;
	for(;and[loop]!='\0';loop++) {
		if(and[loop]==34||and[loop]==39)
			bits.qt += 1;
		else if(and[loop]==';'&&!bits.qt)
			and[loop] = '\0';
	}
	/* now execute "and" */
	void exec(const char *);
	reallocstr(and);
	exec(and);
	free(and);
}

void exec(const char *str)
{
	long int strn = 0;
	/* check validity of equation in number of brackets */
	long int brackets = 0;
	for(;*str!='\0';str++,strn++) {
		if(*str=='(')
			brackets++;
		else if(*str==')')
			brackets--;
	} if(brackets!=0) {
		printf("\nError: Invalid Syntax, incorrect number of parentheses.\n");
		return; }
	str -= strn;

	char *lhs = allocs(strn);
	char *rhs = allocs(strn);
	char *sptr = NULL;  /* here, sptr points at const *str */

	/* here the rhs & lhs are obtained */
	for(sptr=lhs;*str!='\0'&&*str!='=';str++)
		*sptr++ = *str;
	*sptr = '\0';
	if(*str=='=') {
		for(sptr=rhs,str++;*str!='\0';str++)
		{ *sptr++ = *str;
		  *sptr = '\0'; }
	} else { *rhs = '\0'; }
	trims(lhs,' ');
	/* here onwards, sptr is used as an error descrptor string. */
	sptr = (char *) allocs(strn+64);
	*sptr = '\0';

	if(*rhs=='\0') { /* <- empty rhs, so no assignment involved. */

		/* check if rhs is a variable */
		long int *value = NULL;
		value = getint(lhs);

		if(value==NULL) {
			/* rhs is NOT a variable */
			long val=0;
			eval_long(lhs,&val);
			printf("\n%ld\n",val);
		}
		else printf("%s = %ld\n",lhs,*value);
	} else { /* rhs exists  */
		/* check for [=>] exchange directive */
		if(*rhs=='>') {
			exchange_vars(lhs,rhs,sptr);
			if(strlen(sptr)) printf("\nError: %s\n",sptr);
		}
		else /* variable creation/assignment */
		{
			void asgn(char *rhs, char *lhs, char *err);
			asgn(rhs,lhs,sptr);
			if(strlen(sptr)) printf("\nError: %s\n",sptr);
		}
	}
	/* free memeory */
	free(sptr);
	free(lhs);
	free(rhs);
}

void asgn(char *rhs, char *lhs, char *err)
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

	long int *value = NULL;
	long int rvalue = 0;

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
	eval_long(rhs,&rvalue);
	value = newint(lhs);
		/* if variable elready exists, get it. */
	if(value==NULL)
	value = getint(lhs);
		/* set the value */
	if(value!=NULL)
	mov_int(value,&rvalue,esymbol);
	else  /* getint() hasn't worked properly */
	strcpy(err,"Fatal Data Transfer Error.");
	printf("%s = %ld\n",lhs,*value);
}

void eval_long(char *str, long int *value)
{
	/*     Expression Evaluation Function (long)
	 *   ========================================
	 *  This function evaluates an infix expression
	 *  for a long int result. First, it translates
	 *  the given expression to the postfix
	 *  form, and then evaluates the postfix
	 *  expression using the peval function.
	 *  The result is stored in int *value.
	 *  --------------------------------------------
	 *   WARNING: *str is subject to slight changes,
	 *   so transfer expression to temporary string
     *   before passing it here.
	 */
	trims(str,' ');

	register unsigned long int cur=0;
	/* count the number of operators */
	if(*str=='-') { str++; cur++; }
	unsigned long int stack_size=0;
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
	vals_long(str,value);
	return; }

	/* stacks for storing postfix expression */
	char * const ostack = (char *)
	malloc ((stack_size*2 + 1) * sizeof(char));
	long int * const values = (long int *)
	malloc ((stack_size + 1) * sizeof(long int));
	if(ostack==NULL||values==NULL) msg(no_mem,3);

	/*  Rules: ostack contains operators(+,-,*..), wheras
	 *  values contains numbers. if char *ostack = 0, then 
	 *	int *values is holds a valid number of the expression, 
	 *	otherwise char *ostack contains  a valid math operator. 
	 *	Based on these rules the expression is designed.
	 */

	/* important loop counters... */
	register unsigned long int top=0,ival=0,ops=0;

	/***********************************.
	| ( infix => postfix ) translation: |
    `***********************************/
	{
	/* temporary character array */
	char * const tmp = allocs(strlen(str));
	/* temporary stack to hold operators (+,-,*,..) */
	unsigned char * const cstack = (unsigned char *)
	malloc ((stack_size + 1) * sizeof(unsigned char)+11);
	if(cstack==NULL) msg(no_mem,3); *cstack = '\0';
	char prevOp = '\0', negative=0;
	
	for(cur=0;;str++)
	{
		if(*str=='('&&cur) {
			register int pars = 0;
			for(;*str!='\0';str++) {
				if(*str=='(') pars++;
				if(*str==')') pars--;
				if(!pars) break;
				tmp[cur++] = *str;
			}
			if(*str=='\0') break;
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
					if(prevOp=='-')
						negative++;
					prevOp = *str;
					if(*str=='\0') break;
					continue;
				}
			}
			/* bitwise and (&) */
			if(*str=='&'&&str[1]=='&')
				*++str='A';
			/* bitwise or (|) */
			if(*str=='|'&&str[1]=='|')
				*++str='O';
			/* number insertion: */
			vals_long(tmp,&values[ival++]);
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
	long int * const stack = (long int *)
	malloc ((stack_size + 1) * sizeof(long int));
	if(stack==NULL) msg(no_mem,3);

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
		case 'A': stack[--ival-1] &= stack[ival]; break;
		case 'O': stack[--ival-1] |= stack[ival]; break;
		case '<': stack[ival-1] = /* bitwise left shift */
		          stack[ival-1] << stack[--ival]; break;
		case '>': stack[ival-1] = /* bitwise right shift */
		          stack[ival-1] >> stack[--ival]; break;
		case '^': stack[--ival-1] = (long int) pow
		         (stack[ival-2],stack[ival-1]); break;
		case '&': concatenate(&stack[ival-1],stack[--ival]); break;
		default: stack[--ival-1] = stack[ival]; }
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

inline unsigned char eval_prior(char e)
{
	/* used by eval to find operator priority */
	if(e=='(')               return 8;
	else if(e=='%')          return 7;
	else if(e=='^')          return 6;
	else if(e=='*'||e=='/')  return 5;
	else if(e=='+'||e=='-')  return 4;
	else if(e=='&')          return 3;
	else if(e=='<'||e=='>')  return 2;
	else if(e=='A'||e=='O')  return 1;
	else                     return 0;
}

void vals_long(const char *str, long int *value)
{
	puts(str);
	/*     Elementry Evaluator (long)
	 *   ==============================
	 *	This function evaluates, the long
	 *  int value of an exprssion element.
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
	unsigned char ops = 0;

	/* now find ops */
	if(*str=='~'||*str=='!') {
		if(*str++=='~') {
			ops += 3;
			if(*str++=='!') {
				ops++;
				if(*str=='!') ops++;
				else str--;
			} else str--;
		} else { /* (*str++=='!') */
			ops++;
			if(*str++=='!') ops++;
			else str--;
		}
	} if(str[strlen(str)-1]=='!')
		ops += 10;

	/* extract the value */
	if(!strncmp(str,"0x",2))
		sscanf(str,"%lx",value);
	else if((*str>='0'&&*str<='9')||
	(*str=='-'&&str[1]>='0'&&str[1]<='9'))
		*value = atol(str);
	else {
		/* get variable value */
		*value = (getint(str)==NULL) ? 0:*getint(str);
		if(ops==1||ops==2||ops==4||ops==5||
		ops==11||ops==12||ops==14||ops==15) {
			/* nullify variable */
			long int *tmp;
			tmp = getint(str);
			if(tmp!=NULL)
				*tmp = 0;
		}
		else if(ops==2||ops==5||
		       ops==12||ops==15) {
			/* destroy variable */
		}
	}
	/* find factorial & complement */
	if(ops>=10) {
		*value = factorial(*value);
		ops -= 10;
	} if(ops>=3)
		*value = ~*value;
}

/*********************************.
| Data Storage & Retrieval System |
`*********************************/

/*  There are 6 basic data types in my language:
 *  ---------------------------------
 *  null <--     void       => 0x00
 *  int  <--  signed int    => 0x01
 *  num  <--  unsigned int  => 0x02
 *  real <--                => 0x03
 *  chr  <--                => 0x04
 *  byte <-- unsingned char => 0x05
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
	unsigned long int index;
	struct struct_symbol_table * link;
};
static struct struct_symbol_table * symbol_table_offset;

inline void initialize_symbol_table(void)
{
	symbol_table_offset = (struct struct_symbol_table *)
	malloc(sizeof(struct struct_symbol_table));
	if(!symbol_table_offset) msg(no_mem,3);
	symbol_table_offset ->index = 0;
	symbol_table_offset ->link = NULL;
	symbol_table_offset ->string = 0;
	//printf("table made at: %ld\n\n",symbol_table_offset);
}

unsigned long int setaddress(const char *str)
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
	unsigned long int index = linkage ->index;
	linkage ->link = (struct struct_symbol_table *)
	malloc(sizeof(struct struct_symbol_table));
	if(linkage ->link==NULL) msg(no_mem,3);

	linkage = linkage ->link;
	linkage ->link = NULL;
	linkage ->index = index + 1;
	linkage ->string = clones(str);
	return linkage ->index;
}

unsigned long int getaddress(const char *str)
{
	/*  searches the symbol table for
	 *  the string *str, and if found
	 *  returns it's index (>0), if not
	 *  found returns 0.
	 */
	struct struct_symbol_table * linkage = symbol_table_offset;
	
	while (linkage ->link) {
		linkage = linkage ->link;
		//if(!strcmp(linkage ->string,str))
		//	return linkage ->index; /* gotcha! */
		printf("(%ld)->[%d]=> %ld : %s\n",linkage,
		linkage ->link,linkage ->index, linkage ->string);
	}
	return 0; /* <- str does not exist */
}

unsigned long int deladdress(const char *str)
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

struct long_int_list
{
	long int address;
	long int * value;
	long int var_size;
	struct long_int_list * link;
};






struct int_table_struct
{
	char *ids;  /* variable identifier */
	long int var;  /* variable value pointer */
};
struct int_table_struct * int_table = NULL;
static unsigned long int int_table_size = 0;

long int * newint(const char *id)
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
	if((getint(id))!=NULL) return NULL;
	struct int_table_struct * table_ptr;
	/* increment table size by 1 for 1 new variable */
	int_table_size += 1;
	/* reallocate table structure with new size */
	if((table_ptr = (struct int_table_struct *) 
		realloc (int_table, sizeof(struct int_table_struct)*
		int_table_size))==NULL) msg(no_mem,3);
	else /* set the variable */
		int_table = table_ptr;
	/* set the variable name */
	int_table[int_table_size-1].ids = clones(id);
	/* initiaze the variable to 0 */
	int_table[int_table_size-1].var = 0;
	/* return a pointer to the variable */
	return &int_table[int_table_size-1].var;
}

long int * getint(const char *id)
{
	/*  This function returns a integer pointer to an
	 *  existing integer whose is name is string *id.
	**/
	/* check if integer table is empty */
	if(!int_table_size) return NULL;
	/* loop counter to find position in array */
	unsigned long int table_pos = 0;
	/* find out the location of the variable */
	for(;table_pos<int_table_size;table_pos++)
	if(!strcmp(int_table++ ->ids,id)) break;
	/* reverse int_table pointer to origin  */
	int_table -= table_pos+1;
	/* check if the variable exists */
	if(table_pos>=int_table_size) {
		int_table++; return NULL; }
	/* return a pointer to the variable */
	return &int_table[table_pos].var;
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
	long int *value = getint(rhs);
	if(value!=NULL) {
		/* okay, rhs is valid */
		long int rvalue = *value;
		value = getint(lhs);
		if(value!=NULL) {
			/* now swap the values */
			* getint(rhs) = * value;
			* value = rvalue;
			printf("%s => %s\n",lhs,rhs);
		} else
		sprintf(err,"LHS Variable '%s' does not exist.",lhs);
	} else
	sprintf(err,"RHS Variable '%s' does not exist.",rhs);
}

void free_memory()
{
	printf("\n%ld variables freed.\n",int_table_size);
	free(int_table);
}

/************************.
| Mathematical Functions |
`************************/

inline void concatenate(long int *value, long int rvalue)
{
	// concatenates two integers.
	long int _rvalue;
	_rvalue = rvalue;
	do { _rvalue/= 10;
	*value *= 10; }
	while(_rvalue!=0);
	*value += rvalue;
}

inline unsigned long int factorial(unsigned long int value)
{
	// finds factorial(!) of an integer
	long int factorial=1;
	for(;value>0;value--)
	factorial = factorial * value;
	return factorial;
}

inline void mov_int(long int *value, long int *rvalue, char pop_symbol)
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
			case '^': *value  = (long int)pow(*value,*rvalue); break;
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
	unsigned short int rt,lt;
	for(rt=0;;str++)
		if(*str==T) rt++;
		else break;
	for(lt=0;*str;str++) {
		if(*str==T) lt++;
		else if(*str) lt=0;
		str[-rt]=*str;
	} str[-rt-lt]='\0';
}

inline char * allocs(unsigned long int len)
{
	/* allocates an empty string 
	   of length (int len). */
	char *str = (char *) malloc(len+1);
	if(str==NULL) msg(no_mem,3);
	*str = '\0';
	return str;
}

inline void reallocstr(char *str)
{
	/* shrink a string's size in memory 
	   according to it's length. Works
	   only for malloc()ed strings. */
	char *cp = NULL;
	const unsigned long int len = strlen(str) + 1;
	str = (char *) realloc(str,len);
	if((cp = (char *)(realloc(str,
	(len))))!=NULL) str = cp;
	else msg(no_mem,3);
}

inline char * clones(const char *str)
{
	/* allocates a copy of string *str & returns it. */
	const unsigned long int len = strlen(str) + 1;
	char *strc = (char *) malloc(len);
	if(strc==NULL)
		msg(no_mem,3);
	strcpy(strc,str);
	return strc;
}

inline char * twins(const char *str, unsigned long int len)
{
	/* allocates a _strncpy_ of *str & returns it's address */
	if(!len) return NULL;
	char *strc = (char *) malloc(len+1);
	if(strc==NULL)
		msg(no_mem,3);
	strncpy(strc,str,len);
	strc[len]='\0';
	return strc;
}

/***********************.
| Application Functions |
`***********************/

void msg(const char *str, const unsigned char mode)
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
	case 0: printf("\nNotice: %s\n",str); break;
	case 1: printf("\nWarning: %s\n",str); break;
	case 2: printf("\nError: %s\n",str); break;
	case 3: exit(1); break;
	case 4: exit(2); break;
	case 5: abort(); break;
	default: return;
	}
}
