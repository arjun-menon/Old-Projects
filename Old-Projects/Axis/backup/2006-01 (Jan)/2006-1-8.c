/***************************************************
  Project Name: Basic List Processing Interpreter
  Author  Name: Arjun G. Menon  |  Date: 8/1/2006
****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* macro definitions */
#define STR_MAX 1024
#define TRUE  1
#define FALSE 0
#define LIST char **

/* data type definitions */
typedef signed short int int16;
typedef unsigned short int num16;
typedef signed long int int32;
typedef unsigned long int num32;
typedef signed __int64 int64;
typedef signed __int64 num64;
typedef unsigned char uchar;

/*********.
| Globals |
`*********/

num32 linenum = 0; /* number of lines executed */

struct bools
{
	/* boolean(s) */
	uchar x1:1;
	uchar x2:1;
	uchar x3:1;
	uchar x4:1;
	uchar x5:1;
	uchar x6:1;
	uchar x7:1;
	uchar x8:1;
};

/* ai_error is used as an error descriptor string. */
static char *ai_error;

static const char system_no_mem[] = "Not enough memory.";

struct double_list {
	char *id;
	num32 size;
	double *value;
	struct double_list * link;
} glob_dbl_list;

/**********************.
| Function Prototypes: |
`**********************/

/* memory management: */
double * new_dbl(const char *id, double_list *dbl_list);
double * get_dbl(const char *id, double_list *dbl_list);
char del_dbl(const char *id, double_list *dbl_list);
void show_dbl_list(double_list *dbl_list);
void exchange_vars(char *lhs, char *rhs);
void init_dbl_list(double_list *dbl_list);
void init_mem(double_list *dbl_list);
void free_list(LIST list, num32 size);

/* application functions */
void msg(const char *str, const uchar mode);
inline void pause_key(void);

/* lexical analysis: */
void runs(const char *str);
void exec(const char *str);
void eval_dbl(char *str, double *value); // put const for ptr in this and next 2
void sval_dbl(const char *str, double *value);
double * xval_dbl(const char *str, double *value);
LIST lex_expr(const char *str, num32 *args);

inline void init_memory();
inline void free_memory();

/* mathematical functions */
inline uchar eval_prior(char e);
inline num32 factorial(num32 value);
inline int32 concatenate(int32 lhs, int32 rhs);
inline void mov_dbl(double *value, double *rvalue, char pop_symbol);

/* string-related: */
uchar isvalid_id(char *str);
void trims(char *str, const char T);
inline char * allocs(num32 len);
inline void reallocstr(char *str);
inline char * clones(const char *str);
inline char * twins(const char *str, num32 len);

uchar debug_mode = 0;

/************************.
| Command User Interface |
`************************/

int main(int argc, char *argv[])
{
	atexit(free_memory);

	/* program starts here */
	if(argc<=1) {
		/* there are no arguments, so run the shell */
		void shell();
		shell();
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

void shell()
{
	num32 a = 0;
	LIST k = lex_expr("   ads     dd, x, fdg, last  ~ sdaf @ 1202 : sd",&a);
	printf("args = %u\n", a);
	for(num32 i=0; i<a; i++)
		printf("[%s]\n",k[i]);
	return;

	puts(
"\n*-------------------------------------\n\
| Welcome to the AI terminal. Type the   \n\
| program you wish to execute and press  \n\
| Enter key twice to run the program... \n\
`_______________________________________\n"
	);

	char str[STR_MAX];
	char prog[1000][STR_MAX];
	uchar count = 0;
	num32 lines_written = 0;

	// reception stage
	while(count<2)
	{
		fflush(stdin);
		fgets(str,STR_MAX,stdin);
		str[strlen(str)-1] = '\0';
		trims(str,' ');
		trims(str,'\t');
		if(!strlen(str)) count++;
		else strcpy(prog[lines_written++],str);
	}

	init_memory();

	if(lines_written == 0)
		goto shell_prompt;

	// execution stage
	count = 0;
	while(count < lines_written)
		runs(prog[count++]);
	return;

shell_prompt:
	debug_mode = 1;
	puts("AI:SHELL");
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

/**************************.
| Memory Management System |
`**************************/

double * new_dbl(const char *id, double_list *dbl_list)
{
	/* check if variable already exists */
	if(get_dbl(id, &glob_dbl_list) != NULL)
		return NULL;

	/* malloc a new int32_list */
	struct double_list * new_var = NULL;
	new_var = (struct double_list *) malloc
		      (sizeof(struct double_list));
	if(new_var == NULL)
		msg(system_no_mem,3);

	/* setup the elements of list */
	new_var ->id = allocs(strlen(id));
		strcpy(new_var ->id, id);
	new_var ->size = 1;
	new_var ->link = NULL;
	new_var ->value = (double *) malloc 
		              (sizeof(double) * new_var ->size);
	if(new_var ->value == NULL)
		msg(system_no_mem,3);

	/* link and return the variable */
	new_var ->link = dbl_list ->link;
	dbl_list ->link = new_var;

	return new_var ->value;
}

double * get_dbl(const char *id, double_list *dbl_list)
{
	if(id == 0)
		return dbl_list ->value;

	/* get an int32 from a list & return it's pointer. */
	struct double_list *next = dbl_list;
	while(next ->link != NULL)
	{
		next = next ->link;
		if(!stricmp(next ->id, id))
			return next ->value;
	}
	return NULL;
}

char del_dbl(const char *id, double_list *dbl_list)
{
	/* delete a variable an int32 list, if ok; return 0 else 1. */
	struct double_list *old_var = NULL;
	struct double_list *next = dbl_list;

	while(next ->link != NULL)
	{
		if(!strcmp(next ->link ->id, id))
		{
			old_var = next ->link;
			next ->link = old_var ->link;
			free(old_var);
			return 0; /* ok */
		}
		next = next ->link;
	}
	return 1;  /* failed */
}

void show_dbl_list(double_list *dbl_list)
{
	struct double_list *next = dbl_list;
	while(next ->link != NULL)
	{
		next = next ->link;
		printf("%s = %lf [%u]\n",next->id,*(next->value),next->size);
	}
}

num32 free_dbl_list(double_list *dbl_list)
{
	/* delete all the variables in the int32 list. */
	struct double_list *old_var = NULL;
	struct double_list *next = dbl_list;
	num32 count = 0;

	while(next ->link != NULL)
	{
		old_var = next ->link;
		next ->link = old_var ->link;
		free(old_var);
		count++;
	}
	return count;
}

void exchange_vars(char *lhs, char *rhs)
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
	double *value = get_dbl(rhs, &glob_dbl_list);
	if(value!=NULL)
	{
		/* okay, rhs is valid */
		double rvalue = *value;
		value = get_dbl(lhs, &glob_dbl_list);
		if(value!=NULL)
		{
			/* now swap the values */
			* get_dbl(rhs, &glob_dbl_list) = * value;
			* value = rvalue;
			//printf("%s => %s\n",lhs,rhs);
		}
		else {
			ai_error = allocs(32+strlen(lhs));
			sprintf(ai_error,"LHS Variable '%s' does not exist.",lhs);
		}
	}
	else {
		ai_error = allocs(32+strlen(rhs));
		sprintf(ai_error,"RHS Variable \"%s\" does not exist.",rhs);
	}
}

void init_dbl_list(double_list *dbl_list)
{
	dbl_list ->size = 1;
	dbl_list ->link = NULL;
	dbl_list ->value = (double *) (malloc(sizeof(double)));
	if(dbl_list ->value == NULL)
		msg(system_no_mem,3);
	*(dbl_list ->value) = 0;
}

inline void init_memory()
{
	init_dbl_list(&glob_dbl_list);
}

inline void free_memory()
{
	num32 free_dbl_list(double_list *dbl_list);
	printf("\n%u variable(s) freed\n", free_dbl_list(&glob_dbl_list));
}

void free_list(LIST list, num32 size)
{
	/* free a list of strings */
	for(num32 i=0; i<size; i++)
	{
		free(list[i]);
		list[i] = 0;
	}
}

/****************************.
| Lexical Analysis Functions |
`****************************/

void runs(const char *str)
{
	/*
	 * Run Function: runs() takes a string
	 * as argument for interpretation.
	 * Here, a count is kept on the line
	 * number, the comments are stripped
	 * off the string and the string is
	 * trimmed of white spaces. The new
	 * string is passed for further 
	 * interpretation to exec().
	 */

	struct bools bits = { 0, 0, 0, 0, 0, 0, 0, 0 };
	
	if(*str=='#'||*str==' '||*str=='\t')
	return;
	linenum += 1;

	/*
	 *  here str is trimmed of all spaces
	 *  and tabs and stored in string and.
	 */
	char *code = clones(str);
	trims(code,' ');
	trims(code,'\t');

	/* eliminate the comments */
	register num32 loop = 0;
	for(;code[loop]!='\0';loop++)
	{
		if(code[loop]==34||code[loop]==39)
			bits.x1 += 1;
		else if(code[loop]=='#'&&!bits.x1)
			code[loop] = '\0';
	}
	reallocstr(code);

	/* now execute "and" */
	ai_error = 0;
	exec(code);

	/* now runs() returns */
	if(ai_error) {
		if(strlen(ai_error))
			msg(ai_error,2);
		free(ai_error);
	}
	free(code);
}

void exec(const char *str)
{
	/*
	 * Execution Function: exec() takes a
	 * string as argument for execution.
	 * The string is first analysed for
	 * syntactical validity then is split
	 * into one or two pieces and executed.
	 */

	int32 strn = 0;
	struct bools bits = { 0, 0, 0, 0, 0, 0, 0, 0 };

	/* check validity of equation in number of brackets */
	{
		int32 brackets = 0;
		for(;*str!='\0';str++,strn++)
		{
			if(bits.x1==1||bits.x2==1)
				continue;
			else if(*str=='(')
				brackets++;
			else if(*str==')')
				brackets--;
			else if(*str==34)
				bits.x1++;
			else if(*str==39)
				bits.x2++;
		}
		if(brackets!=0)
		{ ai_error = clones("Invalid Syntax, incorrect number of parentheses.");
		return; }
		str -= strn;
	}

	/////////////
	// PART ONE
	/////////////

	char *lhs = allocs(strn);
	char *rhs = allocs(strn);
	/* here the rhs & lhs are obtained */
	{
		char *sptr = lhs;  /* sptr points at lhs */

		for(;*str!='\0';str++)
		{
			if(str[1]=='='&&*str!='<'&&*str!='>'
			&&*str!='='&&str[2]!='=') {
			*sptr++ = *str++;
			break; }
			else  { *sptr++ = *str; }
		}
		*sptr = '\0';

		if(*str=='=')
		{
			for(sptr=rhs,str++;*str!='\0';str++)
			{ *sptr++ = *str;
			  *sptr = '\0'; }
		}
		else {
			*rhs = '\0';
		} trims(lhs,' ');
	}

	/////////////
	// PART TWO 
	/////////////

	if(*rhs=='\0') /* <- empty rhs, so no assignment involved. */
	{
		/* check if lhs is a variable */
		double *value = NULL;
		value = get_dbl(lhs, &glob_dbl_list);

		if(value==NULL) {
			/* lhs is NOT a variable */
			double val=0;
			eval_dbl(lhs,&val);
			value = get_dbl(0, &glob_dbl_list);
			*value = val;

			if(debug_mode)
				printf("\n%lf\n",val);
		}
		else if(debug_mode)
			printf("\n%s = %lf\n",lhs,*value);
	}
	else /* rhs exists  */
	{
		/* check for [=>] exchange directive */
		if(*rhs=='>')
			exchange_vars(lhs,rhs);

		/* variable creation/assignment */
		else {
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
	 * Variable Assignment Function:
	 * asgn() takes two strings lhs
	 * and rhs as arguments. lhs is
	 * probably a variable name, and
	 * rhs the data to assign to it.
	 * asgn() tries to do this, and if
	 * it can't it dumps an description
	 * about the error in *err.
	 */

	double *value = NULL;
	double rvalue = 0;

	/* to find assignment operation */
	char esymbol=0;
	rvalue = int32(strlen(lhs)-1);
	lhs += int32(rvalue);

	/* now find the assignment operation */
	if(*lhs=='*'||*lhs=='/'||
	   *lhs=='+'||*lhs=='-'||
	   *lhs=='%'||*lhs=='^'||
	   *lhs=='~'||*lhs==':'||
	   *lhs=='<'||*lhs=='>'||
	   *lhs=='|'||*lhs=='&'||
	   *lhs=='!'||*lhs=='@')
	{
		esymbol = *lhs;
		*lhs = '\0';
	}

	/* set lhs ptr to origin */
	lhs -= int32(rvalue);
	
	LIST rhs_list = NULL;
	num32 rhs_args = 0;
	rhs_list = lex_expr(rhs, &rhs_args);

	LIST lhs_list = NULL;
	num32 lhs_args = 0;
	lhs_list = lex_expr(lhs, &lhs_args);

	if(rhs_args == lhs_args)
	{
		num32 i = 0;
		for(; i<lhs_args ; i++)
		{
			/* evaluate rhs */
			eval_dbl(rhs_list[i],&rvalue);

			/* check the variable's syntax*/
			if(!isvalid_id(lhs_list[i]))
				/* create the variable */
				value = new_dbl(lhs_list[i], &glob_dbl_list);
			else {
				ai_error = clones("Illegal Identifier Syntax.");
				return;
			}

			/* if variable already exists, then get it. */
			if(value==NULL)
			value = get_dbl(lhs_list[i], &glob_dbl_list);

			/* set the value */
			if(value!=NULL)
			mov_dbl(value,&rvalue,esymbol);

			/* getint32() hasn't worked properly */
			else ai_error = clones("Fatal Data Transfer Error.");

			if(debug_mode)
				printf("\n%s = %lf\n",lhs_list[i],*value);
		}
	}
	else
	{
		/* evaluate rhs */
		eval_dbl(rhs,&rvalue);

		/* check the variable's syntax*/
		if(!isvalid_id(lhs))
			/* create the variable */
			value = new_dbl(lhs, &glob_dbl_list);
		else {
			ai_error = clones("Illegal Identifier Syntax.");
			return;
		}

		/* if variable already exists, then get it. */
		if(value==NULL)
		value = get_dbl(lhs, &glob_dbl_list);

		/* set the value */
		if(value!=NULL)
		mov_dbl(value,&rvalue,esymbol);

		/* getint32() hasn't worked properly */
		else ai_error = clones("Fatal Data Transfer Error.");
	}
}

void eval_dbl(char *str, double *value)
{
	/*    Expression Evaluation Function (int32)
	 *  ==========================================
	 *  This function evaluates an infix expression
	 *  for an int32 result. First, it translates
	 *  the given expression to the postfix
	 *  form, and then evaluates the postfix
	 *  expression using the peval function.
	 *  The result is stored in int32 *value.
	 *  --------------------------------------------
	 *   WARNING: *str is subject to changes, so
	 *   transfer the expression to a temporary
	 *   string before passing it here.
	 */
	trims(str,' ');

	register num32 cur=0;

	/* count the number of operators */
	if(*str=='-') 
		/* to skip - sign */
	{ str++; cur++; }

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
	sval_dbl(str,value);
	return; }

	/* stacks for storing postfix expression */

	char * const ostack = (char *)
	malloc ((stack_size*2 + 1) * sizeof(char));

	double * const values = (double *)
	malloc ((stack_size + 1) * sizeof(double));

	if(ostack==NULL||values==NULL)
		msg(system_no_mem,3);

	/*  Rules: <ostack> contains operators(+,-,*..), wheras
	 *  <values> contains doubles. if char *ostack = 0, then 
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

	/* translation loop: */
	for(cur=0;;str++)
	{
		if(*str=='('&&cur) {
		/* to handle substring braces */
			register int32 pars = 0;
			for(;*str!='\0';str++)
			{
				if(*str=='(')
					pars++;
				if(*str==')')
					pars--;
				if(!pars)
					break;
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
			if(*str=='!'&&str[1]=='!')
				/* logical != */
				*++str='N';
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
			else if(*str=='*'&&str[1]=='*')
				/* power */
				*++str='P';
			else if(*str=='/'&&str[1]=='/')
				/* power */
				*++str='I';
			else if(*str=='>'&&str[1]=='>')
				/* bitwise right shift (>>) */
				*++str='R';
			else if(*str=='<'&&str[1]=='<')
				/* bitwise left shift (<<) */
				*++str='L';
			else if(*str=='>'&&str[1]=='<')
				/* maximum value */
				*++str='M';
			else if(*str=='+'&&str[1]=='&')
				/* concatenate */
				*++str='C';

			/* number insertion: */
			sval_dbl(tmp,&values[ival++]);

			if(negative)
			{
				values[ival-1]*=-1;
				negative = 0;
			}
			ostack[ops++] = 0;
			cur = *tmp = 0;

			/* jump... */
			after_vals:

			/* empty brackets */
			if(*str=='('&&str[1]==')')
			{ str[1] = '0'; continue; }

			/* operand insertion: */
			if(eval_prior(cstack[top])<eval_prior(*str)
			||*str=='(') cstack[++top] = *str;

			else
			{
				for(;top&&cstack[top]!='(';top--)
					ostack[ops++] = cstack[top];
				if(*str!=')')
					cstack[++top] = *str;
				else if(cstack[top]=='(')
					cstack[top--] = 0;
			}
			
			if(*str=='\0') break;
			prevOp = *str;
		} else tmp[cur++] = *str;
	}
	/* free memory: */
	free(cstack);
	free(tmp);
	}

//	printf("\n---[%u]---\n",ops);
//	for(cur=0,top=0;cur<ops;cur++)
//	if(!ostack[cur]) printf(" %lf ",values[top++]);
//	else printf(" %c ",ostack[cur]);
//	puts("\n-----------");
	
	/*******************************.
	| Postfix Expression Evaluator: |
	`*******************************/
	{

	/* temporary stack to evaluate expression */
	double * const stack = (double *)
	malloc ((stack_size + 1) * sizeof(double));
	if(stack==NULL) msg(system_no_mem,3);

	for(cur=0,top=0,ival=0;cur<ops;cur++)
	{
		if(!ostack[cur]) /* an operand */
			stack[ival++] = values[top++];
		else { /* an operator (+,-,*,..)[15] */

		///////////////////////////////////
		/// CORE MATHEMATICAL EXECUTIVE ///
		///////////////////////////////////
		switch(ostack[cur]) { 

		/* basic mathematical operators... */
		case '+': stack[--ival-1] += stack[ival]; break;
		case '-': stack[--ival-1] -= stack[ival]; break;
		case '*': stack[--ival-1] *= stack[ival]; break;
		case '/': stack[--ival-1] /= stack[ival]; break;
		case '%': stack[--ival-1] = fmod(
		          stack[ival-2],stack[ival-1]); break;

		/* the logical operators (==,!!,>=,&&,||,..) */
		case 'E': stack[ival---2] = stack[ival-1] == 
		          stack[ival-2] ? 1 : 0; break; /* == */
		case 'N': stack[ival---2] = stack[ival-1] == 
		          stack[ival-2] ? 0 : 1; break; /* == */
		case '>': stack[ival---2] = stack[ival-1] < 
		          stack[ival-2] ? 1 : 0; break; /* > */
		case 'F': stack[ival---2] = stack[ival-1] >= 
		          stack[ival-2] ? 1 : 0; break; /* >= */
		case '<': stack[ival---2] = stack[ival-1] > 
		          stack[ival-2] ? 1 : 0; break; /* < */
		case 'G': stack[ival---2] = stack[ival-1] >= 
		          stack[ival-2] ? 1 : 0; break; /* <= */
		case 'A': stack[ival---2] =(stack[ival-1]>0)&&(
		          stack[ival-2]) ? 1 : 0; break; /* && */
		case 'O': stack[ival---2] =(stack[ival-1]>0)||(
		          stack[ival-2]) ? 1 : 0; break; /* || */

		/* the bitwise operators (&,|,^,>>,<<,..)  */
		case '&': stack[ival-1] = int32(stack[ival-1]) & int32(stack[--ival]); break;
		case '|': stack[ival-1] = int32(stack[ival-1]) | int32(stack[--ival]); break;
		case '^': stack[ival-1] = int32(stack[ival-1]) ^ int32(stack[--ival]); break;
		case 'L': stack[ival-1] = int32( /* bitwise left shift */
		          stack[ival-1]) << int32(stack[--ival]); break;
		case 'R': stack[ival-1] = int32( /* bitwise right shift */
		          stack[ival-1]) >> int32(stack[--ival]); break;

		/* the special operators (**,+&,><,//)...  */
		case 'P': stack[--ival-1] = pow(
		          stack[ival-2],stack[ival-1]); break;
		case 'I': stack[--ival-1] = pow(
		          stack[ival-2],1/stack[ival-1]); break;
		case 'C': stack[--ival-1] = concatenate((int32)
		          stack[ival-2],(int32)stack[ival-1]); break;
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

void sval_dbl(const char *str, double *value)
{
	/*     Elementry Evaluator (int32)
	 *   ==============================
	 *  This function evaluates, the int32
	 *  value of an exprssion element. Operations 
	 *  ?, !, ~ and factorial are carried
	 *  out here. Function calls are made and
	 *  values of variables are retrieved too.
	 *  ---------------------------------------
	 *   Note: *str sould be trimmed of spaces
	 *   before being passed here.
	 */
	*value = 0;

	/* Out the 4 operations (?,!,~,fact),
	 * only 3 operations ~,? and ! appear
	 * on L.H.S. of str, where ? and ! can
	 * never coincide, but ~ can coincide with
	 * the other two. These operations can be
	 * combined in the following ways:
	 * ------------------------------------
	 *  nothing    -> 0 | !  only    -> 1 
	 *  ? only     -> 2 | ~  only    -> 3
	 *  ~ and !    -> 4 | ~  and ?   -> 5
	 * - - - - - - - - - - - -
	 * in case factorial needs
	 * to be found then add 10:
	 * - - - - - - - - - - - -
	 *  nothing    -> 10 | !  only    -> 11
	 *  ?  only    -> 12 | ~  only    -> 13
	 *  ~  and !   -> 14 | ~  and ?   -> 15
	 * -------------------------------------
	 * The number is found then kept in <ops>
	 */
	uchar ops = 0;

	/* now find ops */
	if(*str=='~'||*str=='!'||*str=='?') {
		if(*str=='~')
		{ ops += 3;
		  str++; }
		if(*str=='!')
		{ ops += 1;
		  str++; }
		else if(*str=='?')
		{ ops += 2;
		  str++; }
		if(*str=='~')
			ops += 3;
	} if(str[strlen(str)-1]=='!')
		ops += 10;

	/* extract the value */
	if((*str=='0')&&(str[1]=='x'||str[1]=='X'))
	/* hexadecimal */ sscanf(str,"%lx",value);

	else if((*str>='0'&&*str<='9')||
	(*str=='-'&&str[1]>='0'&&str[1]<='9'))
	/* decimal */ *value = atof(str);

	else if(*str==39)  /* ['] */
	*value = int32(*++str);

	else {
		/* get the value */
		double * var_ptr;
		var_ptr = xval_dbl(str, value);
	}

	/* do the special operations */
	if(ops>=10) {
		*value = factorial(num32(*value));
		ops -= 10;
	} if(ops>=3) {
		*value = ~int32(*value);
		ops -= 3;
	} if(ops == 1)  /* ! */
		*value = *value > 0 ? 0 : 1;
	if(ops == 2)  /* ? */
		*value = *value > 0 ? 1 : 0;
}

double * xval_dbl(const char *str, double *value)
{
	/*   Exclusive Evaluation Function
	 *  ------------------------------------
	 *  xval peforms exclusive evaluation,
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
	 *  pointed to by it is stored on *value.
	 *
	 *  If the *str describes volatile matter
	 *  (eg. functions returns), then it is
	 *  evaluated & stored in the variable
	 *  pointed at by *value.
	 *
	 *  Whereas if no value can be found, a
	 *  description of the error is written
	 *  and a NULL pointer is returned.
	 */

	LIST list = NULL;
	num32 n = 0;

	list = lex_expr(str,&n);

	if(list)
	{
		if(n == 1) /* extract variable */
		{
			if(!strcmp("_",str))
			{
				*value = *get_dbl(0, &glob_dbl_list);
				free_list(list,n);
				return get_dbl(0, &glob_dbl_list);
			}
			else if(get_dbl(str, &glob_dbl_list) != NULL)
			{
				*value = *get_dbl(str, &glob_dbl_list);
				free_list(list,n);
				return get_dbl(str, &glob_dbl_list);
			}
			else
			{
				*value = 0;
				free_list(list,n);
				ai_error = allocs(40+strlen(str));
				sprintf(ai_error,"Variable \"%s\" does not exist.",str);
				return get_dbl(str, &glob_dbl_list);
			}
		}
		else /* call function */
		{
			double call_func(char ** list, num32 args);
			*value = call_func(list,n);
			free_list(list,n);
			return NULL;
		}
	}
	else {
		ai_error = allocs(20+strlen(str));
		sprintf(ai_error,"Invalid Synatax: ",str);
		return NULL;
	}
}

LIST lex_expr(const char *str, num32 *args)
{
	/*  lex_expr_list() splits a an expression into
	 *  several component strings and malloc()s
	 *  them into a string array which is returned.
	 */

	struct bools bits = { 0, 0, 0, 0, 0, 0, 0, 0 };
	num32 strn = 0 , i;

	if(strlen(str))
		*args = 1;
	else {
		*args = 0;
		return 0;
	}

	char *tmp = allocs(strlen(str));

	/* go past the first opening bracket */
	for(;*str!='\0'&&*str!='(';str++,strn++);

	if(*str=='\0')
	/* there is no opening bracket */
	{
		/////////////
		// PART ONE
		/////////////

		str -= strn;
		char key = ' ';

		/* count the arguments */
		for(;*str!='\0';str++)
			if(*str==','&&!bits.x1)
				(*args)++;
			else if(*str==34)
				bits.x1++;
		str -= strn;

		if(*args > 1)
			key = ',';
		else
		{
			for(;*str!='\0';str++)
				if(*str==' '&&!bits.x1)
					(*args)++;
				else if(*str==34)
					bits.x1++;
			str -= strn;
		}

		/* string array for storing arguments: */
		char ** list = (char **) (malloc(sizeof(char **) * *args));
		if(list == NULL)
			msg(system_no_mem,3);
		for(i=0; i <= *args; i++)
			list[i] = 0;

		i = 0; /* must! be zero for the next loop */

		if(key == ',')
		{
			/* if the parameter forms a function call, then
			   it must be seperated form the first argument */
			
			for(strn=0;;strn++)
			{
				if(str[strn]==' '||str[strn]==','||str[strn]=='\0')
				{
					if(str[strn]==','||str[strn]=='\0')
						break;

					if(str[strn]==' ' && bits.x1!=1)
						continue;

					list[i] = allocs(strn);
					strncpy(list[i],str,strn);
					list[i][strn] = '\0';
					trims(list[i],' ');

					str += strn;
					i = 1;

					break;
				}
				else
					bits.x1 = 1;
			}

		}
		bits.x1 = 0;
		(*args)++;

		/* retrieve function arguments */
		for(strn=0;;str++)
		{
			if((*str==key&&!bits.x1)||*str=='\0')
			{
				*tmp = '\0';
				tmp -= strn;
				trims(tmp,' ');
				list[i++] = clones(tmp);
				strn = 0;

				if(*str=='\0')
					break;
			} else {
				*tmp++ = *str;
				strn++;
				if(*str==34)
					bits.x1++;
			}
		}
		free(tmp);
		return list;
	}
	else  /* opening bracket exists */
	{
		/////////////
		// PART TWO 
		/////////////

		str++;
		strn++;

		/* count the arguments */
		for(;*str!='\0';str++,strn++)
		{
			if(*str==','&&!bits.x1)
				(*args)++;
			else if(*str==34)
				bits.x1++;
			else if(*str==')'&&!bits.x1)
				break;
		}
	
		if(*str=='\0') {
		/* since a function must end with a closing bracket */
			free(tmp);
			return 0;
		} else {
		/* an extra 1 for the last argument */
			*args += 1;
		}

		/* string array for storing arguments: */
		char ** list = (char **) (malloc(sizeof(char **) * *args));
		if(list == NULL)
			msg(system_no_mem,3);
		for(num32 i=0; i <= *args; i++)
			list[i] = 0;

		str -= strn;
		/* retrieve function name...*/
		for(strn=0;*str!='(';str++,strn++)
			*tmp++ = *str;
		*tmp = '\0';

		i = 0;
		tmp -= strn;
		trims(tmp,' ');
		list[i++] = clones(tmp);

		/* retrieve function arguments */
		for(str++,strn=0;*str!='\0';str++)
		{
			if((*str==','||*str==')')&&!bits.x1)
			{
				*tmp = '\0';
				tmp -= strn;
				trims(tmp,' ');
				list[i++] = clones(tmp);
				strn = 0;
				if(*str==')') {
					tmp -= strn;
					break;
				}
			} else {
				*tmp++ = *str;
				strn++;
				if(*str==34)
					bits.x1++;
			}
		}
		free(tmp);
		return list;
	}
}

/******************.
| Functions System |
`******************/

double call_func(char ** list, num32 args)
{
	/* --- core standard library --- */

	/* core interface & language functions: */
	double clib_if(char *S1);
	double clib_not(char *S1);
	double clib_del(char *S1);
	double clib_print(char *S1);
	double clib_input(char *S1);

	/* core mathematical functions: */
	double clib_abs(char *S1);
	double clib_acos(char *S1);
	double clib_asin(char *S1);
	double clib_atan(char *S1);
	double clib_atan2(char *S1, char *S2);
	double clib_ceil(char *S1);
	double clib_cos(char *S1);
	double clib_cosh(char *S1);
	double clib_exp(char *S1);
	double clib_floor(char *S1);
	double clib_ldexp(char *S1, char *S2);
	double clib_log(char *S1);
	double clib_log10(char *S1);
	double clib_sin(char *S1);
	double clib_sinh(char *S1);
	double clib_sqrt(char *S1);
	double clib_tan(char *S1);
	double clib_tanh(char *S1);

	#define IFFN(FN) if(!stricmp(list[0],FN))

	/* interface & language functions router */
	IFFN("if")
		return clib_if(list[1]);
	else IFFN("not")
		return clib_not(list[1]);
	else IFFN("del")
		return clib_del(list[1]);
	else IFFN("print")
		return clib_print(list[1]);
	else IFFN("input")
		return clib_input(list[1]);

	/* mathematical functions router */
	else IFFN("abs")
		return clib_abs(list[1]);
	else IFFN("acos")
		return clib_acos(list[1]);
	else IFFN("asin")
		return clib_asin(list[1]);
	else IFFN("atan")
		return clib_atan(list[1]);
	else IFFN("atan2")
		return clib_atan2(list[1],list[2]);
	else IFFN("ceil")
		return clib_ceil(list[1]);
	else IFFN("cos")
		return clib_cos(list[1]);
	else IFFN("cosh")
		return clib_cosh(list[1]);
	else IFFN("exp")
		return clib_exp(list[1]);
	else IFFN("floor")
		return clib_floor(list[1]);
	else IFFN("ldexp")
		return clib_ldexp(list[1],list[2]);
	else IFFN("log")
		return clib_log(list[1]);
	else IFFN("log10")
		return clib_log10(list[1]);
	else IFFN("sin")
		return clib_sin(list[1]);
	else IFFN("sinh")
		return clib_sinh(list[1]);
	else IFFN("sqrt")
		return clib_sqrt(list[1]);
	else IFFN("tan")
		return clib_tan(list[1]);
	else IFFN("tanh")
		return clib_tanh(list[1]);

	#undef IFFN
	return 0;
}

/***********************.
| Core Standard Library |
`***********************/

/* core interface & language functions: */

double clib_if(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	if(A1 > 0) return 1;
	else return 0;
}

double clib_not(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	if(A1 < 0) return 1;
	else return 0;
}

double clib_del(char *S1)
{
	if(del_dbl(S1, &glob_dbl_list)) {
		ai_error = allocs(40+strlen(S1));
		sprintf(ai_error,"Variable \"%s\" does not exist.",S1);
	}
	return 0;
}

double clib_print(char *S1)
{
	double A1 = 0;
	if(!ai_error)
	eval_dbl(S1,&A1);
	if(!ai_error)
		printf("\n%lf\n",A1);
	return 0;
}

double clib_input(char *S1)
{
	double *var_ptr = NULL;
	var_ptr = get_dbl(S1, &glob_dbl_list);
	if(var_ptr == NULL) {
		ai_error = allocs(40+strlen(S1));
		sprintf(ai_error,"Variable \"%s\" does not exist.",S1);
		return 0;
	}
	printf("\n? ");
	char str[STR_MAX];
	gets(str);
	*var_ptr = atof(str);
	return 0;
}

/* core mathematical functions: */

double clib_abs(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return fabs(A1);
}

double clib_acos(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return acos(A1);
}

double clib_asin(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return asin(A1);
}

double clib_atan(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return atan(A1);
}

double clib_atan2(char *S1, char *S2)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	double A2 = 0;
	eval_dbl(S2,&A2);
	return atan2(A1,A2);
}

double clib_ceil(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return ceil(A1);
}

double clib_cos(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return cos(A1);
}

double clib_cosh(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return cosh(A1);
}

double clib_exp(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return exp(A1);
}

double clib_floor(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return floor(A1);
}

double clib_ldexp(char *S1, char *S2)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	double A2 = 0;
	eval_dbl(S2,&A2);
	return ldexp(A1,int32(A2));
}

double clib_log(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return log(A1);
}

double clib_log10(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return log10(A1);
}

double clib_sin(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return sin(A1);
}

double clib_sinh(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return sinh(A1);
}

double clib_sqrt(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return sqrt(A1);
}

double clib_tan(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return tan(A1);
}

double clib_tanh(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);
	return tanh(A1);
}

/************************.
| Mathematical Functions |
`************************/

inline int32 concatenate(int32 lhs, int32 rhs)
{
	// concatenates two integers.
	struct bools bits;
	bits.x1 = 0;

	if(lhs<0)
	{ bits.x1++;
	  lhs = labs(lhs);}
	if(rhs<0)
	{ bits.x1++;
	  rhs = labs(rhs);}

	num32 r = num32(rhs);
	while(r>0) {
		r /= 10;
		lhs *= 10;
	}
	return lhs+rhs;
}

inline num32 factorial(num32 value)
{
	// finds factorial(!) of an integer
	int32 factorial=1;
	for(;value>0;value--)
	factorial = factorial * value;
	return factorial;
}

inline void mov_dbl(double *value, double *rvalue, char pop_symbol)
{
	/* <double> transfer function */
	if(pop_symbol) {
		/* perform post-evaluation operations*/
		switch(pop_symbol) {
			case '*': *value *= *rvalue; break;
			case '/': *value /= *rvalue; break;
			case '+': *value += *rvalue; break;
			case '-': *value -= *rvalue; break;
			case '%': *value = int32(*value) % int32(*rvalue); break;
			case '&': *value = int32(*value) & int32(*rvalue); break;
			case '|': *value = int32(*value) | int32(*rvalue); break;
			case '<': *value = int32(*value) << int32(*rvalue); break;
			case '>': *value = int32(*value) >> int32(*rvalue); break;
			case '~': *value = ~int32(*rvalue); break;
			case '!': *value = factorial(num32(*rvalue)); break;
			case '^': *value = (int32)pow(*value,*rvalue); break;
			case '@': *value = fabs(*rvalue); break;
			case ':': *value = concatenate((int32)(value),int32(*rvalue)); break;
			default: *value = *rvalue;
		}
	} else /* there is no post-evaluation operator */
	*value  = *rvalue; /* this _just_ sets the value */
}

/*********************.
| String Manupilation |
`*********************/

uchar isvalid_id(char *str)
{
	if(*str>='0'&&*str<='9')
		return *str;

	for(;*str!='\0';str++)
		if((*str<'A'||*str>'Z')
		&& (*str<'a'||*str>'z')
		&& (*str<'0'||*str>'9')
		&& (*str!='_'))
			return *str;

	return 0;
}

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
	}
	str[-rt-lt]='\0';
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
	   according to it's length. */

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
	case 2: printf("\nLine %u Error: %s\n",linenum,str  ); break;
	case 3: pause_key();          exit(1); break;
	case 4: pause_key();          exit(2); break;
	case 5: /*!!!*/               abort(); break;
	default: return;             /* do nothing */
	}
}
