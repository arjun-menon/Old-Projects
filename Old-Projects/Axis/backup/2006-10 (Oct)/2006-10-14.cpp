/***************************************************
    AXIS (Advanced eXtensible Interpreter System)
   .---------------------------------------------.
   |    Program Name: AXIS CORE                  |
   |    Program Version: 0.1                     |
   |    Author Name: Arjun G. Menon              |
   |    Release Date: 2006 October 14            |
****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <fcntl.h>

#define TRUE  1
#define FALSE 0
#define LIST char **
#define STR_MAX 1024
#define MAX_LINES 1000

/* data type substitutes */
#define int16 signed short int
#define num16 unsigned short int
#define int32 signed long int
#define num32 unsigned long int
#define int64 signed __int64
#define num64 signed __int64
#define byte unsigned char

////////////////////////////////////
// --- AXIS CORE STARTS HERE ---  //
////////////////////////////////////

/*********.
| Globals |
`*********/

/* number of lines executed */
num32 linenum = 0;
num32 goto_line = 0;
/* ai_error is used as an error descriptor string. */
static char *ai_error;
struct bools;
struct double_list;

/**********************.
| Function Prototypes: |
`**********************/

/* memory management: */
inline void init_memory();
inline num32 free_memory();
void init_dbl_list(double_list *dbl_list);
num32 free_dbl_list(double_list *dbl_list);
double * new_dbl(const char *id, double_list *dbl_list);
double * get_dbl(const char *id, double_list *dbl_list);
char del_dbl(const char *id, double_list *dbl_list);
void show_dbl_list(double_list *dbl_list);
void exchange_vars(char *lhs, char *rhs);
LIST make_list(const num32 size, const num32 length);
void free_list(LIST list, const num32 size);
void store_str(char *str, LIST* str_array, num32 *array_size);
const char * fetch_str(num32 index, LIST* str_array,
					   const num32 array_size);
void free_str_array(LIST* str_array, num32 array_size);

/* core execution: */
inline void initialize();
inline void terminate();
inline byte prog_write(char *str);
inline const char * prog_read(num32 index);
inline void prog_show();
inline void free_prog_table();
void run_line(const char *str);
num32 exec_line(const char *str, byte direct);
// < ! make pointers const for next 3 >
void eval_dbl(char *str, double *value);
void sval_dbl(const char *str, double *value);
double * xval_dbl(const char *str, double *value);

/* lexical analysis */
LIST lex_expr(const char *str, num32 *args);
int32 lex_subscript(const char *str, char *var_name);

/* mathematical functions */
inline byte eval_prior(char e);
inline num32 factorial(num32 value);
inline int32 concatenate(int32 lhs, int32 rhs);
inline void mov_dbl(double *value, double *rvalue, 
					char pop_symbol);

/* string manupilation: */
byte isvalid_id(const char *str);
void trims(char *str, const char T);
char prechar(char *str, const char *src, const char *key);
char postchar(char *str,const char *src, const char *key);
int strrht(char *str, const char *src, const num32 n);
int strlft(char *str, const char *src, const num32 n);
int strmid(char *str, const char *src, 
		   const num32 m, const num32 n);
void strnum(char *str, const num32 num, const num32 len);
void strbluff(char *str, const char *bluff, const num32 n);
inline void strnully(char *str, const num32 n);
inline void strnull(char *str);
void string(char *str);

/* string allocation: */
inline char * allocs(num32 len);
inline void reallocstr(char *str);
inline char * clones(const char *str);
inline char * twins(const char *str, num32 len);

/* application functions */
byte run_file(char *fname);
void axis_shell();
LIST axis_writer();
void axis_console();
void msg(const char *str, const byte mode);
inline void pause_key(void);
char file_exists(const char *fname);

/* extended axis toolkit: */
void c2h(int argc, char *argv[]);
double eqa(const char *str);
void f2c();
void eas();

/*******************.
| Other Definitions |
`*******************/

byte debug_mode = 0;

struct bools
{
	/* boolean(s) */
	byte x1:1;
	byte x2:1;
	byte x3:1;
	byte x4:1;
	byte x5:1;
	byte x6:1;
	byte x7:1;
	byte x8:1;
};

/************************.
| Command User Interface |
`************************/

int main(int argc, char *argv[])
{
	/* program starts here */
	if(argc<=1) {
		/* there are no arguments, so run the shell */
		axis_shell();
	} else {
		/* open each file (arg as filename) and run it. */
		int32 i;
		for(i=1; i<argc; i++) {
			if(!run_file(argv[i]))
			printf("\nCould not open file: %s\n",argv[i]);
		}
	} return EXIT_SUCCESS;
}

byte run_file(char *fname)
{
	FILE *afile=NULL;
	char str[STR_MAX];
	if((afile = fopen(fname,"r"))!=NULL) {
		initialize();
	    while(fgets(str,STR_MAX,afile)!=NULL) {
			string(str);
			//printf("Run[%d]: %s \n",strlen(str),str);
			run_line(str);
		}
		fclose(afile);
		terminate();
		return 1;    /* success */
	} else
		return 0;    /* failure */
}

void axis_shell()
{
	LIST program = NULL;
	program = axis_writer();
	if(program == NULL)
		axis_console();

	else {
		printf("\nSave program? (Hit Enter to decline): ");
		char str[STR_MAX];
		fgets(str,STR_MAX,stdin);
		string(str);
		//for(int i=0; i<100; i++)
		//	printf("P[%d] = %c\n",i,*program[i]);
		if(strlen(str)) {
			FILE *fp = NULL;
			fp = fopen(str,"w");
			for(int i=0; *program[i]!='\0'; i++) {
				fwrite(program[i],strlen(program[i]),1,fp);
				fputc('\n',fp);  /* insert newline */
			}
			fclose(fp);
		}
	}
}

LIST axis_writer()
{
	/*
	char s[10];
	int d= lex_subscript("x[4]",s);
	printf("%s, %d",s,d);
	char s[10];
	int d= lex_subscript("x[4]",s);
	printf("%s, %d",s,d);
	puts("(debug)! Lexer Dump...\n");
	num32 a = 0; char buf[100];
	for(a=0;a<100;a++) buf[a] = 0;
	scanf("%[^\n]", buf);
	if(!strlen(buf))
	strcpy(buf,"   ads     dd, x, fdg, last  ~ sdaf @ 1202 : sd");
	LIST k = lex_expr(buf,&a);
	printf("args = %u\n", a);
	for(num32 i=0; i<a; i++)
		printf("[%s]\n",k[i]);
	*/

	puts(
"\n*---------------------------------------\n\
| Welcome to the AXIS terminal. Type the   \n\
| program you wish to execute and press  \n\
| Enter key twice to run the program... \n\
`_______________________________________\n"
	);

	char str[STR_MAX];
	LIST prog = NULL;
	prog = make_list(MAX_LINES, STR_MAX);
	byte count = 0;
	num32 lines_written = 0;

	// reception stage
	while(count<2)
	{
		fflush(stdin);
		fgets(str,STR_MAX,stdin);
		string(str);
		trims(str,' ');
		trims(str,'\t');
		if(!strlen(str)) count++;
		else strcpy(prog[lines_written++],str);
	}

	if(lines_written == 0)
		return NULL;

	// execution stage
execute:
	initialize();
	count = 0;
	while(count < lines_written)
		run_line(prog[count++]);
	terminate();

	printf("\nRun Again? (yes/y): ");
	char again[STR_MAX];
	fgets(again,STR_MAX,stdin);
	string(again);
	if(!stricmp(again,"yes") 
	|| !stricmp(again,"y"))
		goto execute;

	return prog;
}

void axis_console()
{
	initialize();
	puts("AI:SHELL");
	char str[STR_MAX];

	/* since the shell is primarily used
	   for debugging we keep the debug on */
	debug_mode = 1;

	/* this loop fishes out the instructions from stdin
	   and runs them until an "exit" is encountered. */
	while(TRUE) {
		printf("\n$ ");
		fflush(stdin);
		fgets(str,STR_MAX,stdin);
		string(str);
		trims(str,' ');
		trims(str,'\t');
		if(!strlen(str)) continue;
		if(!stricmp(str,"exit")) break;
		run_line(str);
	}
	terminate();
}

/**************************.
| Memory Management System |
`**************************/

static const char system_no_mem[] = "Not enough memory.";

struct double_list {
	char *id;
	num32 size;
	double *value;
	struct double_list * link;
} glob_dbl_list;

inline void init_memory()
{
	init_dbl_list(&glob_dbl_list);
}

inline num32 free_memory()
{
	num32 freed = 0;
	freed += free_dbl_list(&glob_dbl_list);
	free_prog_table();
	return freed;
}

double * new_dbl(const char *id, double_list *dbl_list)
{
	/* find out declaration details */
	char *var_id = clones(id);
	num32 size = abs(lex_subscript(id, var_id));
	if(size < 1) size = 1;

	/* check the variable's syntax*/
	if(isvalid_id(var_id)) {
		ai_error = clones("Illegal Identifier Syntax.");
		return NULL;
	}

	/* check if variable already exists */
	if(get_dbl(var_id, &glob_dbl_list) != NULL)
		return NULL;

	/* malloc a new double_list */
	struct double_list * new_var = NULL;
	new_var = (struct double_list *) malloc
		      (sizeof(struct double_list));
	if(new_var == NULL)
		msg(system_no_mem, 4);

	/* setup the elements of list */
	new_var ->id = allocs(strlen(var_id));
		strcpy(new_var ->id, var_id);
	new_var ->size = size;
	new_var ->link = NULL;
	new_var ->value = (double *) malloc 
		              (sizeof(double) * new_var ->size);
	if(new_var ->value == NULL)
		msg(system_no_mem, 4);

	/* link and return the variable */
	new_var ->link = dbl_list ->link;
	dbl_list ->link = new_var;

	return new_var ->value;
}

double * get_dbl(const char *id, double_list *dbl_list)
{
	if(id == NULL)
		return dbl_list ->value;

	/* find out request details */
	char *var_id = allocs(strlen(id));
	int32 index = lex_subscript(id, var_id);

	/* get the double from the list & return it's pointer. */
	struct double_list *next = dbl_list;
	while(next ->link != NULL)
	{
		next = next ->link;
		if(!stricmp(next ->id, var_id)) {
			if(index < (int32)(next ->size) && index >= 0)
				return &(next ->value[index]);
			else if(index == -1) {
				/* return size of array */
				double *dtmp = (double *) malloc(sizeof(double));
				*dtmp = next ->size;
				return dtmp;
			}
			else {
				ai_error = clones("Array out of bounds.");
				return NULL;
			}
		}
	}
	return NULL;
}

char del_dbl(const char *id, double_list *dbl_list)
{
	/* delete the variable 'id' from the 
	   double list, if ok; return 0 else 1. */

	struct double_list *old_var = NULL;
	struct double_list *next = dbl_list;

	while(next ->link != NULL)
	{
		if(!strcmp(next ->link ->id, id))
		{
			old_var = next ->link;
			next ->link = old_var ->link;
			free(old_var);
			old_var = NULL;
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
		printf("%s = %lf [%lu]\n",next->id,
			*(next->value),next->size);
	}
}

void init_dbl_list(double_list *dbl_list)
{
	dbl_list ->size = 1;
	dbl_list ->link = NULL;
	dbl_list ->value = (double *)(malloc(sizeof(double)));

	if(dbl_list ->value == NULL)
		msg(system_no_mem, 4);
	*(dbl_list ->value) = 0;
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
		old_var = NULL;
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
	if(ai_error)
		if(strlen(ai_error))
			return;
	if(value!=NULL)
	{
		/* okay, rhs is valid */
		double rvalue = *value;
		value = get_dbl(lhs, &glob_dbl_list);
		if(ai_error)
			if(strlen(ai_error))
				return;
		if(value!=NULL)
		{
			/* now swap the values */
			* get_dbl(rhs, &glob_dbl_list) = * value;
			* value = rvalue;
			if(debug_mode)
			  printf("%s => %s\n",lhs,rhs);
		}
		else {
			ai_error = allocs(32+strlen(lhs));
			sprintf(ai_error,
			"LHS Variable '%s' does not exist.",lhs);
		}
	}
	else {
		ai_error = allocs(32+strlen(rhs));
		sprintf(ai_error,
		"RHS Variable \"%s\" does not exist.",rhs);
	}
}

LIST make_list(const num32 size, const num32 length)
{
	LIST k;
	k = (LIST)(malloc(size * sizeof(char*)));
	if(k == NULL)
		msg(system_no_mem, 4);
	register num32 i;
	for(i=0; i<size; i++) {
		k[i] = (char *)(malloc(length * sizeof(char)));
		if(k[i] == NULL)
			msg(system_no_mem, 4);
		*k[i] = '\0';
	}
	return k;
}

void free_list(LIST list, const num32 size)
{
	/* free a list of strings */
	for(num32 i=0; i<size; i++)
	{
		free(list[i]);
		list[i] = NULL;
	}
}

void store_str(char *str, LIST* str_array, num32 *array_size)
{
	(*array_size)++;
	(*str_array) = (LIST) realloc((*str_array), 
		(*array_size) * sizeof(char *));
	if(str_array == NULL)
		msg(system_no_mem, 4);

	(*str_array)[(*array_size)-1] = allocs(strlen(str));
	strcpy((*str_array)[(*array_size)-1], str);
}

const char * fetch_str(num32 index, LIST* str_array, const num32 array_size)
{
	if(index <= array_size)
		return (*str_array)[index];
	else return NULL;
}

void free_str_array(LIST* str_array, num32 array_size)
{
	num32 i = 0;
	for(;i<array_size;i++) {
		free((*str_array)[i]);
		(*str_array)[i] = NULL;
	}
	free(*str_array);
	*str_array = NULL;
}

/*******************************.
| Core Program Execution System |
`*******************************/

inline void initialize()
{
	init_memory();
}

inline void terminate()
{
	linenum = 0;
	goto_line = 0;
	printf("\n%lu variable(s) freed\n", free_memory());
}

static LIST prog_table = NULL;
num32 prog_table_size = 0;

inline byte prog_write(char *str)
{
	if(!strlen(str) || *str == '\n')
		return 0; /* string rejected */
	store_str(str, &prog_table, &prog_table_size);
	return 1; /* success */
}

inline const char * prog_read(num32 index)
{
	if(index > 0 && index <= prog_table_size) 
		return fetch_str(index-1, 
		&prog_table, prog_table_size); 
	else
		return NULL;
}

inline void prog_show()
{
	register num32 i = 0;
	for(i=0; i<prog_table_size; i++)
		puts(prog_table[i]);
}

inline void free_prog_table()
{
	free_str_array(&prog_table, prog_table_size);
}

void run_line(const char *str)
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

	char *code = clones(str);
	trims(code,' ');
	trims(code,'\t');

	if(*code=='#'||*code==' '
	 ||*code=='\t'||*code=='\0')
	return;

	/*
	 *  here str is trimmed of all spaces
	 *  and tabs and stored in string and.
	 */

	struct bools bits = { 0, 0, 0, 0, 0, 0, 0, 0 };

	/* eliminate the comments */
	register num32 loop = 0;
	for(;code[loop]!='\0';loop++)
	{
		if(code[loop]=='\"'||code[loop]=='\'')
			bits.x1 += 1;
		else if(code[loop]=='#'&&!bits.x1)
			code[loop] = '\0';
	}
	reallocstr(code);

	if(prog_write(code))
		linenum++;
	else
		return;
	//printf("\n%d = %d\n",linenum,prog_table_size);
	num32 next_line;

	if(goto_line > linenum) {
		//printf("%s skipped.\n",code);
		goto quit;
	}

	/* now execute code */
	ai_error = 0;
	next_line = exec_line(code, 0);

	/* check for errors */
	if(ai_error) {
		if(strlen(ai_error))
			msg(ai_error,2);
		free(ai_error);
		ai_error = NULL;
	}

	/* jump to next line if needed */
checkpoint:
	if(next_line)
		if(prog_read(next_line))
		/* go to the past */
	{
		if(next_line == linenum) {
			msg("Goto cannot reference itself.",2);
			goto quit;
		}
		//printf("\nGoto Past: %u\n",next_line);
		register num32 i = next_line;

		while(i < linenum) {
			const char *ns = NULL;
			ns = prog_read(i);
			if(ns == NULL)
				break;

			/* now execute code */
			//puts(ns);
			ai_error = 0;
			next_line = exec_line(ns, 0);

			/* check for errors */
			if(ai_error) {
				if(strlen(ai_error))
					msg(ai_error,2);
				free(ai_error);
				ai_error = NULL;
			}

			if(next_line)
				goto checkpoint;
			i++;
		}
	} else { /* go to the future */
		//printf("\nGoto Future: %u\n",next_line);
		goto_line = next_line;
	}

	quit:
	free(code);
	code = NULL;
}

num32 exec_line(const char *str, byte direct)
{
	/*
	 * Execution Function: exec() takes a
	 * string as argument for execution.
	 * The string is first analysed for
	 * syntactical validity then is split
	 * into one or two pieces and executed.
	 */

	//////////////////
	// VERIFY SYNTAX
	//////////////////

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
			else if(*str=='\"')
				bits.x1++;
			else if(*str=='\'')
				bits.x2++;
		}
		if(brackets!=0)
		{ ai_error = clones(
		"Invalid Syntax, incorrect number of parentheses.");
		return 0; }
		str -= strn;
	}

	///////////////////////
	// INSTRUCTION LOOPER
	///////////////////////

	static int32 loop = 0;
	if(loop > 0 && !direct) {
		//printf("\nWill do \"%s\" %u times.\n",str,loop);
		for(;loop>1; loop--) {
			exec_line(str, 1);
			if(ai_error)
				if(strlen(ai_error))
					return 0;
		}
		loop = 0;
	}
	else if(loop < 0) {
		loop = 0;
		return 0;
	}

	///////////////////////////
	// INVOKE KEY SUBROUTINES
	///////////////////////////

	char *tmp = allocs(strn);

	/* invoke << term >> */

	if(!strlft(tmp,str,4) && !stricmp(tmp,"term"))
	{
		terminate();
		initialize();

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << debug >> */

	if(!strlft(tmp,str,6) && !stricmp(tmp,"debug "))
	{
		postchar(tmp,str," ");

		if((!stricmp(tmp,"1"))||
		   (!stricmp(tmp,"on"))||
		   (!stricmp(tmp,"true")))
		{
			debug_mode = 1;
			puts("\nDebug switched on.");
		} else 
		if((!stricmp(tmp,"0"))||
		   (!stricmp(tmp,"off"))||
		   (!stricmp(tmp,"false")))
		{
			debug_mode = 0;
			puts("\nDebug switched off.");
		} else
			printf("Error: Debug mode \'%s\' does not exist.",tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	else if(!strlft(tmp,str,5) && !stricmp(tmp,"debug"))
	{
		if(debug_mode)
			puts("\nDebug mode is on.");
		else
			puts("\nDebug mode is off.");

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << pause >> */

	else if(!strlft(tmp,str,5) && !stricmp(tmp,"pause"))
	{
		printf("\nPress any key to continue . . . ");
		pause_key();

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << exec >> */

	else if(!strlft(tmp,str,5) && !stricmp(tmp,"exec "))
	{
		postchar(tmp,str," ");
		if(debug_mode)
			printf("\nExecuting: %s\n",tmp);
		printf("\n");
		system(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << write >> */

	else if(!strlft(tmp,str,6) && !stricmp(tmp,"write "))
	{
		postchar(tmp,str," ");
		puts(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	else if(!strlft(tmp,str,7) && !stricmp(tmp,"newline"))
	{
		postchar(tmp,str," ");
		puts("");

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << loop >> */

	else if(!strlft(tmp,str,5) && !stricmp(tmp,"loop "))
	{
		postchar(tmp,str," ");

		double val=0;
		eval_dbl(tmp,&val);
		loop = abs((int32)(floor(val)));

		free(tmp);
		tmp = NULL;
		return 0;
	}

	else if(!strlft(tmp,str,7) && !stricmp(tmp,"repeat "))
	{
		postchar(tmp,str," ");

		double val=0;
		eval_dbl(tmp,&val);
		loop = abs((int32)(floor(val)));

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << goto >> */

	else if(!strlft(tmp,str,5) && !stricmp(tmp,"goto "))
	{
		postchar(tmp,str," ");

		double val=0;
		eval_dbl(tmp,&val);
		num32 next_line = 0;
		next_line = (num32)(abs((int32)(floor(val))));

		free(tmp);
		tmp = NULL;
		return next_line;
	}
	else if(*str == '>')
	{
		postchar(tmp,str,">");

		double val=0;
		eval_dbl(tmp,&val);
		num32 next_line = 0;
		next_line = (num32)(abs((int32)(floor(val))));

		free(tmp);
		tmp = NULL;
		return next_line;	
	}

	/* invoke << prog >> */

	else if(!strlft(tmp,str,5) && !stricmp(tmp,"prog "))
	{
		postchar(tmp,str," ");
		int line_no = atoi(tmp);
		const char *line = prog_read(line_no);
		printf("\n");
		if(line != NULL)
			puts(line);
		else
			puts("Line Number not found.");

		free(tmp);
		tmp = NULL;
		return 0;
	}

	else if(!strlft(tmp,str,4) && !stricmp(tmp,"prog"))
	{
		printf("\n");
		prog_show();

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << eqa >> */

	else if(!strlft(tmp,str,4) && !stricmp(tmp,"eqa "))
	{
		postchar(tmp,str," ");
		eqa(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	else if(!strlft(tmp,str,5) && !stricmp(tmp,"eval "))
	{
		postchar(tmp,str," ");
		eqa(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << exit >> */

	else if(!strlft(tmp,str,4) && (
	!stricmp(tmp,"exit") || !stricmp(tmp,"quit")))
	{
		terminate();
		exit(0);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	free(tmp);
	tmp = NULL;

	/////////////////////
	// SPLIT EXPRESSION
	/////////////////////

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
		}
	}
	trims(lhs,' ');
	trims(rhs,' ');

	////////////////////////
	// EXECUTE INSTRUCTION 
	////////////////////////

	if(*rhs=='\0') /* <- empty rhs, so no assignment involved. */
	{
		/* check if lhs is a variable */
		double *value = NULL;
		value = get_dbl(lhs, &glob_dbl_list);
		if(ai_error)
			if(strlen(ai_error))
				return 0;

		if(value==NULL) {
			/* lhs is NOT a variable */
			double val=0;
			eval_dbl(lhs,&val);
			value = get_dbl(0, &glob_dbl_list);
			if(ai_error)
				if(strlen(ai_error))
					return 0;
			*value = val;

			/* check for iterator */
			if(lhs[strlen(lhs)-1]=='>'
			    ||!strncmp(lhs,"if",2)) {
				loop = abs((int32)(floor(val)));
				if(loop == 0)
					loop = -1;
				//puts("loop set");
			}
			else if(debug_mode)
				printf("\n%lf\n",val);

			if(debug_mode) {
				if(!strncmp(lhs,"if",2)) {
					if(val>0)
						puts("\ntrue");
					else
						puts("\nfalse");
				}
			}
		}
		else
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
	lhs = NULL;
	free(rhs);
	rhs = NULL;
	return 0;
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

	trims(lhs,' ');
	trims(rhs,' ');

	/* to find assignment operation */
	char esymbol=0;
	rvalue = (int32)(strlen(lhs)-1);
	lhs += (int32)(rvalue);

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
	lhs -= (int32)(rvalue);
	
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

			/* create the variable */
			value = new_dbl(lhs_list[i], &glob_dbl_list);
			if(ai_error)
				if(strlen(ai_error))
					return;

			/* if variable already exists, then get it. */
			if(value==NULL)
			value = get_dbl(lhs_list[i], &glob_dbl_list);
			if(ai_error)
				if(strlen(ai_error))
					return;

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

		/* create the variable */
		value = new_dbl(lhs, &glob_dbl_list);
		if(strlen(ai_error))
			return;

		/* if variable already exists, then get it. */
		if(value==NULL)
		value = get_dbl(lhs, &glob_dbl_list);
		if(ai_error)
			if(strlen(ai_error))
				return;

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

	if(ostack == NULL || values == NULL)
		msg(system_no_mem, 4);

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
	byte * const cstack = (byte *)
	malloc ((stack_size + 1) * sizeof(byte)+11);

	if(cstack == NULL)
	  msg(system_no_mem, 4); 
	*cstack = '\0';
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
	if(stack == NULL)
	  msg(system_no_mem, 4);

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
		case '+': stack[--ival-1] += 
		          stack[ival]; break;
		case '-': stack[--ival-1] -= 
		          stack[ival]; break;
		case '*': stack[--ival-1] *= 
		          stack[ival]; break;
		case '/': stack[--ival-1] /= 
		          stack[ival]; break;
		case '%': stack[--ival-1] = fmod(
		          stack[ival-2],
		          stack[ival-1]); break;

		/* the logical operators (==,!!,>=,&&,||,..) */
		case 'E': stack[ival---2] = 
		          stack[ival-1] == 
		          stack[ival-2] ? 1 : 0; break; /* == */
		case 'N': stack[ival---2] = 
		          stack[ival-1] == 
		          stack[ival-2] ? 0 : 1; break; /* == */
		case '>': stack[ival---2] = 
		          stack[ival-1] < 
		          stack[ival-2] ? 1 : 0; break; /* > */
		case 'F': stack[ival---2] = 
		          stack[ival-1] >= 
		          stack[ival-2] ? 1 : 0; break; /* >= */
		case '<': stack[ival---2] = 
		          stack[ival-1] > 
		          stack[ival-2] ? 1 : 0; break; /* < */
		case 'G': stack[ival---2] = 
		          stack[ival-1] >= 
		          stack[ival-2] ? 1 : 0; break; /* <= */
		case 'A': stack[ival---2] =
		         (stack[ival-1]>0)&&(
		          stack[ival-2]>0) ? 1 : 0; break; /* && */
		case 'O': stack[ival---2] =
		         (stack[ival-1]>0)||(
		          stack[ival-2]>0) ? 1 : 0; break; /* || */

		/* the bitwise operators (&,|,^,>>,<<,..)  */
		case '&': stack[ival-1] = 
		  (int32)(stack[ival-1]) & 
		  (int32)(stack[--ival]); break;
		case '|': stack[ival-1] = 
		  (int32)(stack[ival-1]) | 
		  (int32)(stack[--ival]); break;
		case '^': stack[ival-1] = 
		  (int32)(stack[ival-1]) ^ 
		  (int32)(stack[--ival]); break;
		case 'L': stack[ival-1] = /* bitwise left shift */
		  (int32)(stack[ival-1]) << 
		  (int32)(stack[--ival]); break;
		case 'R': stack[ival-1] = /* bitwise right shift */
		  (int32)(stack[ival-1]) >> 
		  (int32)(stack[--ival]); break;

		/* the special operators (**,+&,><,//)...  */
		case 'P': stack[--ival-1] = pow(
		          stack[ival-2],
		          stack[ival-1]); break;
		case 'I': stack[--ival-1] = pow(
		          stack[ival-2],
		        1/stack[ival-1]); break;
		case 'C': stack[--ival-1] = concatenate((int32)
		          stack[ival-2],
		   (int32)stack[ival-1]); break;
		case 'M': stack[ival---2] = 
		          stack[ival-1] > 
		          stack[ival-2] ?
		          stack[ival-1] : 
		          stack[ival-2]; break;
		default:  stack[--ival-1] = 
		          stack[ival];
		          }
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

inline byte eval_prior(char e)
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
	//puts(str);

	byte ops = 0;
	
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
	*value = (int32)(*++str);

	else {
		/* get the value */
		double * var_ptr;
		var_ptr = xval_dbl(str, value);
	}

	/* do the special operations */
	if(ops>=10) {
		*value = factorial((num32)(*value));
		ops -= 10;
	} if(ops>=3) {
		*value = ~(int32)(*value);
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
	double call_func(char ** list, num32 args);

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
				if(ai_error)
					if(strlen(ai_error))
						return NULL;
				free_list(list,n);
				return get_dbl(0, &glob_dbl_list);
			}
			else if(get_dbl(str, &glob_dbl_list) != NULL)
			{
				*value = *get_dbl(str, &glob_dbl_list);
				if(ai_error)
					if(strlen(ai_error))
						return NULL;
				free_list(list,n);
				return get_dbl(str, &glob_dbl_list);
			}
			else
			{
				if(call_func(list,n))
				return get_dbl(str, &glob_dbl_list);
				*value = 0;
				free_list(list,n);
				ai_error = allocs(40+strlen(str));
				sprintf(ai_error,
				"Variable \"%s\" does not exist.",str);
				return get_dbl(str, &glob_dbl_list);
			}
		}
		else /* call function */
		{
			*value = call_func(list,n);
			free_list(list,n);
			return NULL;
		}
	}
	else {
		ai_error = allocs(20+strlen(str));
		sprintf(ai_error,"Invalid Synatax: %s",str);
		return NULL;
	}
}

/****************************.
| Lexical Analysis Functions |
`****************************/

LIST lex_expr(const char *str, num32 *args)
{
	/*  lex_expr_list() splits a an expression into
	 *  several component strings and malloc()s
	 *  them into a string array which is returned.
	 */

//	 printf("Lexing: %s\n\n",str);

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
		char ** list = (char **)
			(malloc(sizeof(char *) * *args));
		if(list == NULL)
			msg(system_no_mem, 4);
		for(i=0; i <= *args; i++)
			list[i] = 0;

		i = 0; /* must! be zero for the next loop */

		goto hAcK;  // this func if full of bugs :-(

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
		
		hAcK: // this func must be rewritten

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
		tmp = NULL;
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
			tmp = NULL;
			return 0;
		} else {
		/* an extra 1 for the last argument */
			*args += 1;
		}

		/* string array for storing arguments: */
		char ** list = (char **)
			(malloc(sizeof(char *) * *args));
		if(list == NULL)
			msg(system_no_mem, 4);
		for(i=0; i <= *args; i++)
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
		tmp = NULL;
		return list;
	}
}

int32 lex_subscript(const char *str, char *var_name)
{
	char *tmp = allocs(strlen(str));
	double val=0;

	prechar(var_name,str,"[");
	if(!strlen(var_name))
		goto not_found;
	trims(var_name,' ');
	postchar(tmp,str,"[");
	prechar(tmp,tmp,"]");
	if(!strlen(var_name))
		goto not_found;
	trims(tmp,' ');
	if(!strlen(tmp))
		return -1;
	eval_dbl(tmp,&val);
	return abs((int32)(floor(val)));

not_found:
	strcpy(var_name, str);
	return 0;
}

/******************.
| Functions System |
`******************/

double call_func(char ** list, num32 args)
{
//puts(list[0]);
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

	#define FN_CALL(FN) else if(!stricmp(list[0],FN)) return
	#define PR_CALL(FN) else if(!stricmp(list[0],FN))

	if(!strlen(list[0]))
		return 0;

	/* interface & language functions router */
	FN_CALL("if")      clib_if(list[1]);
	FN_CALL("not")     clib_not(list[1]);
	FN_CALL("del")     clib_del(list[1]);
	FN_CALL("print")   clib_print(list[1]);
	FN_CALL("input")   clib_input(list[1]);

	/* mathematical functions router */
	FN_CALL("abs")     clib_abs(list[1]);
	FN_CALL("acos")    clib_acos(list[1]);
	FN_CALL("asin")    clib_asin(list[1]);
	FN_CALL("atan")    clib_atan(list[1]);
	FN_CALL("atan2")   clib_atan2(list[1],list[2]);
	FN_CALL("ceil")    clib_ceil(list[1]);
	FN_CALL("cos")     clib_cos(list[1]);
	FN_CALL("cosh")    clib_cosh(list[1]);
	FN_CALL("exp")     clib_exp(list[1]);
	FN_CALL("floor")   clib_floor(list[1]);
	FN_CALL("ldexp")   clib_ldexp(list[1],list[2]);
	FN_CALL("log")     clib_log(list[1]);
	FN_CALL("log10")   clib_log10(list[1]);
	FN_CALL("sin")     clib_sin(list[1]);
	FN_CALL("sinh")    clib_sinh(list[1]);
	FN_CALL("sqrt")    clib_sqrt(list[1]);
	FN_CALL("tan")     clib_tan(list[1]);
	FN_CALL("tanh")    clib_tanh(list[1]);

	/* extended axis toolkit router*/
	FN_CALL("eqa")     eqa(list[1]);
	FN_CALL("eval")    eqa(list[1]);
	PR_CALL("f2c")   { printf("\n"); f2c(); return 1; }
	PR_CALL("c2h")   { printf("\n"); c2h(0,NULL); return 1; }
	PR_CALL("eas")   { printf("\n"); eas(); return 1; }

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
	if(ai_error)
		if(strlen(ai_error))
			return 0;
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
	return ldexp(A1,(int32)(A2));
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

	num32 r = (num32)(rhs);
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
			case '%': *value = (int32)(
			          *value) % (int32)(*rvalue); break;
			case '&': *value = (int32)(
			          *value) & (int32)(*rvalue); break;
			case '|': *value = (int32)(
			          *value) | (int32)(*rvalue); break;
			case '<': *value = (int32)(
			          *value) << (int32)(*rvalue); break;
			case '>': *value = (int32)(
			          *value) >> (int32)(*rvalue); break;
			case '~': *value = ~(int32)(*rvalue); break;
			case '!': *value = factorial((num32)(*rvalue)); break;
			case '^': *value = (int32)pow(*value,*rvalue); break;
			case '@': *value = fabs(*rvalue); break;
			case ':': *value = concatenate((int32)(value),
			                   (int32)(*rvalue)); break;
			default:  *value = *rvalue;
		}
	} else /* there is no post-evaluation operator */
	*value  = *rvalue; /* this _just_ sets the value */
}

/*********************.
| String Manupilation |
`*********************/

byte isvalid_id(const char *str)
{
	// checks the syntactical validity
	// of a proposed identifier.

	if(*str>='0'&&*str<='9')
		return *str;

	for(;*str!='\0';str++)
		if((*str<'A'||*str>'Z')
		&& (*str<'a'||*str>'z')
		&& (*str<'0'||*str>'9')
		&& (*str!='_'))
			return *str;

	return 0; /* 0 means good */
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

char prechar(char *str, const char *src, const char *key)
{
	// prechar can be used to extract a part of a string.
	// it takes a set of characters from *src preceding 
	// any one of the characters first encountered in *key.
	// these extracted characters are stored in *str.
	// Note: *key can be used to make multiple comparisns

	if(*src=='\0')
		/* if str is empty return 0 */
	return 0;
	if(*key=='\0')
		/* if key is empty return 0 */
	return 0;
	num32 klen = strlen(key);
	num32 slen = strlen(src);
	  /* loop counters */
	num32 u=0, i=0, j=0, k=0;

	for(u=0;u<klen;u++)
		/* to take each char from key for comparisn */
	{
		if(*key=='\0')
			/* exit loop if all keys are over */
		break;
		for(i=0;i<slen;i++)
			/* to take each char from str for comparisn */
		{
			if(*src=='\0')
				/* check if it is the end of str */
			break;
			if(*src==*key)
				/* compare *key and *str */
			{
				for(k=0;k<i;k++)
					/* reverse str pointer to start  */
				src--;
				for(k=0;k<i;k++)
					/* to take char from str for storage */
				{
					/* here the new string is made */
					*str=*src;
					str++;
					src++;
				}
				/* set termination point for string */
				*str='\0';
				return *key;
			}
			/* move pointer to next char in src */
			src++;
		}
		for(j=0;j<i;j++)
			/* reverse str pointer to start  */
		src--;
			/* move pointer to next char in key */
		key++;
	}
	return 0;
}

char postchar(char *str,const char *src, const char *key)
{
	// postchar can be used to extract a part of a string.
	// it takes a set of characters from *src encountered 
	// after any one of the characters first encountered in *key.
	// these extracted characters are stored in *str.
	// Note: *key can be used to make multiple comparisns

	if(*src=='\0')
		/* if str is empty return 0 */
	return 0;
	if(*key=='\0')
		/* if key is empty return 0 */
	return 0;
	num32 klen = strlen(key);
	num32 slen = strlen(src);
	  /* loop counters */
	num32 u=0, i=0, j=0, k=0;

	for(u=0;u<klen;u++)
		/* to take each char from key for comparisn */
	{
		if(*key=='\0')
			/* exit loop if all keys are over */
		break;
		for(i=0;i<slen;i++)
			/* to take each char from str for comparisn */
		{
			if(*src=='\0')
				/* check if it is the end of str */
			break;
			if(*src==*key)
				/* compare *key and *str */
			{
				/* increment pointer by one block to avoid *key */
				src++;
				for(k=0;k<(slen-i);k++)
					/* to take char from str for storage */
				{
					/* here the new string is made */
					*str=*src;
					str++;
					src++;
				}
				/* set termination point for string */
				*str='\0';
				return *key;
			}
			/* move pointer to next char in src */
			src++;
		}
		for(j=0;j<i;j++)
			/* reverse str pointer to start  */
		src--;
		  /* move pointer to next char in key */
		key++;
	}
	return 0;
}

int strrht(char *str, const char *src, const num32 n)
{
	// strrht can be used to extract a part of a string.
	// it takes upto 'n' characters from the right and
	// stores them in *str. if successfull, returns 0.
	// Note: if 'n' exceeds the string length of *str,
	// then the return will be no.of exceed chars.

	num32 len = strlen(src);
	num32 i;  /* loop counter */
	for(i=1;i<=(len-n);i++)
		/* to move pointer to n blocks before null */
	src++;

	for(i=1;i<=n;i++)
	{
		if(*src=='\0')
			/* in case of premature string termination */
		{ 
			*str='\0';
			  /* calculates how bad the error was */
			return ((n-i)+1);
		}
		/* here the new string is made */
		*str=*src;
		str++;
		src++;
	}

	/* set termination point for string */
	*str='\0';
	return 0;
}

int strlft(char *str, const char *src, const num32 n)
{
	// strlft can be used to extract a part of a string.
	// it takes upto 'n' characters from the left and
	// stores them in *str. if successfull, returns 0.
	// Note: if 'n' exceeds the string length of *str,
	// then the return will be no.of exceed chars.

	num32 i;
	/* main loop counter */
	for(i=1;i<=n;i++)
	{
		if(*src=='\0')
			/* in case of premature string termination */
		{
			*str='\0';
			  /* calculates how bad the error was */
			return ((n-i)+1);
		}
		  /* here the new string is made */
		*str=*src;
		str++;
		src++;
	}

	/* set termination point for string */
	*str='\0';
	return 0;
}

int strmid(char *str, const char *src, const num32 m, 
                              const num32 n)
{
	// strmid can be used to extract a part of a string.
	// it takes upto 'n' characters that are encountered
	// after the first 'm' set of characters and
	// stores them in *str. if successfull, returns 0.
	// Note: if 'n' or  'm' exceeds the string length of
	// *str, then the return will be no.of exceed chars.

	num32 i;
	/* main loop counter */
	for(i=1;i<=m;i++)
	{
		if(*src=='\0')
			/* in case of premature string termination */
		{
			*str='\0';
			  /* calculates how bad the error was */
			return (((n+m)-i)+1);
		}
		  /* to move pointer to m blocks forward */
		src++;
	}

	for(i=1;i<=n;i++)
	{
		if(*src=='\0')  
			/* in case of premature string termination */
		{
			*str='\0';
			  /* calculates how bad the error was */
			return ((n-i)+1);
		}
		  /* here the new string is made */
		*str=*src;
		str++;
		src++;
	}

	/* set termination point for string */
	*str='\0';
	return 0;
}

void strnum(char *str, const num32 num,
			const num32 len)
{
	// this function writes a number 'num' of specific 
	// width 'len' into the string *str.

	num32 digits = 0;
	num32 _num = num;
	for(;_num;digits++)
		_num /= 10;

	num32 zeros = len-digits;
	if(zeros==len)
		zeros -= 1;
	for(;zeros;zeros--)
		*str++='0';

	if(!digits)
		str[++digits-1]='0';
	str[digits]='\0'; /* end string */

	for(_num=num;_num;_num/=10)
	str[digits---1] = (char)((_num%10)+'0');
}

void string(char *str)
{
	/* remove a new-line char. 
	from strings if present. */

	if(*str=='\0')
		return;
	if(*str=='\n') {
		*str = '\0';
		return;
	}

	short int i;
	for(i=0;str[i]!='\0';i++)
	{
		if(str[i]=='\n')
		{ 
			str[i]='\0';
			break;
		}
	}
}

void strbluff(char *str, const char *bluff, const num32 n)
{
	// fills up a string with a another string 'n' times.

	for(register num32 i=0; i<n; i++)
		strcat(str, bluff);
}

inline void strnully(char *str, const num32 n)
{
	for(register num32 i=0; i<n; i++) *str = '\0';
}

inline void strnull(char *str)
{
	*str = '\0';
}

/***************************.
| Dynamic String Allocation |
`***************************/

inline char * allocs(num32 len)
{
	/* allocates an empty string 
	   of length (num32 len). */

	char *str = (char *) malloc(len+1);
	if(str == NULL)
	  msg(system_no_mem, 4);
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
	else msg(system_no_mem, 4);
}

inline char * clones(const char *str)
{
	/* allocates a copy of string *str & returns it. */

	const num32 len = strlen(str) + 1;
	char *strc = (char *) malloc(len);

	if(strc == NULL)
		msg(system_no_mem, 4);
	strcpy(strc,str);
	return strc;
}

inline char * twins(const char *str, num32 len)
{
	/* allocates a _strncpy_ of *str & returns it's address */

	if(!len) return NULL;
	char *strc = (char *) malloc(len+1);

	if(strc == NULL)
		msg(system_no_mem, 4);
	strncpy(strc,str,len);
	strc[len]='\0';
	return strc;
}

/***********************.
| Application Functions |
`***********************/

inline void pause_key(void)
{ fflush(stdin); getc(stdin); }

void msg(const char *str, const byte mode)
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
	case 3: printf("\nFatal Error: %s\n\a",str); 
		pause_key(); exit(1); break;
	case 4: printf("\nCritical Error: %s\n\a\a",str); 
		pause_key(); exit(2); break;
	case 5: puts("\a\a\a");       abort(); break;
	default: return;             /* do nothing */
	}
}

char file_exists(const char *fname)
{
	if(*fname=='\0'
	|| *fname=='\n'
	|| !strlen(fname))
		return 0;

     if(*fname == '-')
       fname++;
     
     FILE *tmp;
     tmp = fopen(fname,"r");

     if(!tmp)
       return 0;
     else
       fclose(tmp);

     return 1;
}

//////////////////////////////////
// --- AXIS CORE ENDS HERE ---  //
//////////////////////////////////

/*****************************************/

/////////////////////////////////////
// --- AXIS LIBRARY - TOOLKIT ---  //
/////////////////////////////////////

/*   -  < c2html >  -   */

/**********************************************
  C SOURCE CODE TO HTML PAGE TRANSLATOR v0.16
***********************************************/

#define COL_MAX 32

struct colors
{
	char chr[COL_MAX];
	char text[COL_MAX];
	char string[COL_MAX];
	char comment[COL_MAX];
	char keyword[COL_MAX];
	char numbers[COL_MAX];
};

void c2h(int argc, char *argv[])
{
	/* function declarations */
	void default_colors(struct colors *c);
	void c2html(const char *filename, char *err, struct colors *c);

	/* varaible declarations */
	char filename[STR_MAX];
	char err[STR_MAX+256];
	err[0] = filename[0] = '\0';
	struct colors c;
	default_colors(&c);

	/* user interface */
	puts("===========================================");
	puts(" << SOURCE CODE TEXT TO HTML TRANSLATOR >> ");
	puts("===========================================");
	puts("  This program converts C source code text "); 
	puts("  file to an HTML page with line numbers.  ");
	puts("  This program was written by Arjun Menon. ");
	puts("-------------------------------------------");
	puts("  To modify the default color settings,    ");
	puts("  precede the filename with : colon symbol ");
	puts("  for monochrome, or ? for user-defined.   ");
	puts("===========================================");

	if(argc<=1)  /* the are no arguments */
	{
		puts("Please enter the name of the file to translate...");

		filename_entry:
		printf("\nFilename: ");

		fgets(filename,STR_MAX,stdin);
		string(filename);

		if(!file_exists(filename))
		{
		   puts("No such file exists");
		   goto filename_entry;
		}
	}
	else  /* there are arguments */
	{
		/* only the first argument is read */
		strcpy(filename,argv[1]);
		printf("\nFilename: %s\n",filename);
	}

	/* now translate the file */
	c2html(filename,err,&c);

	/* check for errors */
	if(strlen(err))
	printf("\nError: %s\n",err);

	/* bye bye */
	puts("\nProgram Terminated.");
	getc(stdin);

	return;
}

void default_colors(struct colors *c)
{
	/* set default colors for C elements */
	strcpy(c->chr, "magenta");
	strcpy(c->text, "black");
	strcpy(c->string, "red");
	strcpy(c->comment, "green");
	strcpy(c->keyword, "blue");
	strcpy(c->numbers, "purple");
}

void userdef_colors(struct colors *c)
{
	/* input colors from the user */
	printf("\n");
	printf("Color of text: ");
	fgets(c->text,COL_MAX,stdin);
	fflush(stdin);
	printf("Color of char: ");
	fgets(c->chr,COL_MAX,stdin);
	fflush(stdin);
	printf("Color of string: ");
	fgets(c->string,COL_MAX,stdin);
	fflush(stdin);
	printf("Color of comment: ");
	fgets(c->comment,COL_MAX,stdin);
	fflush(stdin);
	printf("Color of keywords: ");
	fgets(c->keyword,COL_MAX,stdin);
	fflush(stdin);
	printf("Color of numbers: ");
	fgets(c->numbers,COL_MAX,stdin);
	fflush(stdin);
	/* convert to string */
	void string(char *str);
	string(c->chr);
	string(c->text);
	string(c->string);
	string(c->comment);
	string(c->keyword);
	string(c->numbers);
}

void c2html(const char *filename, char *err, struct colors *c)
{
	/* <- core function -> here the C source 
	   code is tranlated to an HTML web page. */

	struct /* common bits */
	{
		unsigned char color : 1;  /* color the code */
		unsigned char qt1 : 1;  /* double quotes */
		unsigned char qt2 : 1;  /* single quotes */
		unsigned char cm1 : 1;  /* multi-line comments */
		unsigned char cm2 : 1;  /* single-line comments */
		unsigned char num : 1;  /* numerical constants */
		unsigned char pre : 1;  /* preprocessors */
		unsigned char cont : 1;  /* preprocessors */
	}
	bits = { 1, 0, 0, 0, 0, 0, 0, 0 };

	if(!strlen(filename))
	{
		puts("\nPlease enter a filename.");
		return;
	}

	if(*filename==':')
	{
		bits.color = 0;
		filename++;
	}
	else if(*filename=='?')
	{
		userdef_colors(c);
		filename++;
	}

	FILE *cfile,*hfile;
	cfile = hfile = NULL;

	char htmlfile[STR_MAX];
	strcpy(htmlfile,filename);
	strcat(htmlfile,".html");

	if((cfile = fopen(filename,"r"))==NULL)
	{
		sprintf(err,"Could not open file: %s",filename);
		return;
	}

	unsigned short int line_length = 0;
	char input_str[STR_MAX];
	printf("\nHow many letters per line? ");
	fgets(input_str,STR_MAX,stdin);
	string(input_str);
	line_length = atoi(input_str);
	if(line_length <= 1)
		line_length = 80;

	printf("\nAnalyzing file: %s ...\n\n",filename);

	register char move = 0;
	unsigned int strmax, strnow, lines, pages;
	unsigned long int charcount = 0;
	unsigned long int letters = 0;
	strmax = strnow = lines = pages = 0;

	for(;move!=EOF;strnow++, charcount++)
	{
		move = fgetc(cfile);
		if(move=='\n')
		{
			strmax = strnow>strmax ? strnow:strmax;
			strnow = 0;
			lines++;
		}
		if(move<127 && move!='\n' && move!= '\r')
			letters++;
	}

	pages = lines/50;
	pages += lines%50 < 10 ? 0 : 1;

	printf("No. of pages = %d\n",pages);
	printf("No. of lines = %d\n",lines);
	printf("No. of chars = %ld\n",charcount);
	printf("No. of letters = %ld\n",letters);
	
	if(charcount<=1)
	{
		strcpy(err,"Too few characters.");
		fclose(cfile);
		return;
	}

	if((hfile = fopen(htmlfile,"w"))==NULL)
	{
		sprintf(err,"Could not create file: %s",htmlfile);
		fclose(cfile);
		return;
	}

	printf("\nTranslating file: %s ...    ",filename);

	fprintf(hfile,"<HTML><HEAD><TITLE>%s</TITLE></HEAD>\n",filename);
	fprintf(hfile,"<BODY TEXT=%s BGCOLOR=white><CENTER><H1>%s</H1></CENTER><HR><CODE>\n",c->text,filename);

	char *wstr = NULL, *wwstr = NULL;
	if((wstr = (char *) malloc
	(sizeof(char)*(strmax+1)))==NULL)
	{
		strcpy(err,"Out of memory.");
		fclose(hfile);
		fclose(cfile);
		return;
	}
	else *wstr = '\0';

	unsigned int digits = 0;
	unsigned int _lines = lines;
	for(;_lines;digits++) _lines /= 10;

	char *nstr = NULL;
	if((nstr = (char *) malloc
	(sizeof(char)*(digits+1)))==NULL)
	{
		strcpy(err,"Out of memory.");
		free(wstr);
		fclose(hfile);
		fclose(cfile);
		return;
	}
	else *nstr = '\0';
	fclose(cfile);

	unsigned int lnum = 0; 
	unsigned register int cnum = 0;
	cfile = fopen(filename,"r");

	/* C keywords */
	char keys[33][9] =
	{ 
		"auto","break","case","char",
		"const","continue","default",
		"double","do","else","enum",
		"extern","float","for","goto",
		"if","int","long","register",
		"return","short","signed",
		"sizeof","static","struct",
		"switch","typedef","union",
		"unsigned","void","volatile",
		"while", "inline"
	};

	for(bits.cont=0;fgets(wstr,strmax,cfile)!=NULL;lnum++)
	{
		unsigned int remaining = 0;
		unsigned int split_offset = 0;

		remainder:

		/* show translation progress */
		char prog[4] = "00%";
		double progn = (lnum*100)/lines;

		if(progn<100)
		strnum(prog,(int)progn,progn<10?1:2);
		strcat(prog,"%");

		if(strlen(prog)==2)
		printf("\b\b%s",prog);


		if(strlen(prog)==3)
		printf("\b\b\b%s",prog);
		
		/* insert line numbers */
		short int numlength = 0;
		if(remaining)
			goto so_go_on;

		strnum(nstr,lnum,digits);
		numlength = strlen(nstr);
		fprintf(hfile,"<B>%s</B> &nbsp;",nstr);

		so_go_on:

		/* check if the line has been commented off */
		if(bits.cm1&&lnum)
		fprintf(hfile,"<FONT COLOR=%s>",c->comment);

		/* reset the booleans in bits */
		if(bits.cont && bits.pre)
			fprintf(hfile,"<B>");
		else
			bits.pre = 0;
		if(bits.cont && bits.qt1)
			fprintf(hfile,"<FONT COLOR=%s>",c->string);
		else
			bits.qt1 = 0;
		bits.qt2 = 0;
		bits.cm2 = 0;
		bits.num = 0;

		unsigned int line_len = 0;
		line_len = line_length - numlength;

		if(remaining)
		{
			strnum(nstr,lnum,digits);
			numlength = strlen(nstr);
			strnull(nstr);
			strbluff(nstr,"&nbsp;" ,numlength);
			fprintf(hfile,"%s &nbsp;",nstr);

			char *t = allocs(line_len+1);
			strmid(t,wwstr,split_offset,line_len);
			strcpy(wstr,t);
			split_offset += strlen(wstr);
			remaining -= strlen(wstr);
		}

		if(strlen(wstr)>line_len && remaining <= 0)
		{
			split_offset = 0;
			char *t = allocs(line_len+1);
			strmid(t,wstr,split_offset,line_len);
			wwstr = clones(wstr);
			strcpy(wstr,t);

			split_offset += strlen(wstr);
			remaining = strlen(wwstr)-line_len;
		}

		for(cnum=0;cnum<strlen(wstr);cnum++)
		{
			/* color up the keywords */
			int keyw;
			for(keyw=0;keyw<33;keyw++)
			if(!strncmp(&wstr[cnum],keys[keyw],
				strlen(keys[keyw]))&&!(
			   wstr[cnum-1]>='A'&&
			   wstr[cnum-1]<='Z')&&!(
			   wstr[cnum-1]>='a'&&
			   wstr[cnum-1]<='z')&&!(
			   wstr[cnum-1]>='0'&&
			   wstr[cnum-1]<='9')&&
			   wstr[cnum-1]!='_'
               &&(!bits.cm1)
               &&(!bits.cm2)
               &&(!bits.qt1)
               &&(!bits.qt2)
               &&bits.color)
			{
				if(!(wstr[cnum+strlen(keys[keyw])]>='A'&&
			         wstr[cnum+strlen(keys[keyw])]<='Z')&&!(
			         wstr[cnum+strlen(keys[keyw])]>='a'&&
			         wstr[cnum+strlen(keys[keyw])]<='z')&&!(
					 wstr[cnum+strlen(keys[keyw])]>='0'&&
			         wstr[cnum+strlen(keys[keyw])]<='9')&&
			         wstr[cnum+strlen(keys[keyw])]!='_') {
				fprintf(hfile,"<FONT COLOR=%s>%s</FONT>",c->keyword,keys[keyw]);
				cnum += strlen(keys[keyw]);
				}
			}

			/* color up the comments */
			if(!strncmp(&wstr[cnum],"/*",2)
			   &&!bits.qt1
			   &&!bits.qt2
			   &&!bits.cm2
			   &&bits.color)
			{
				fprintf(hfile,"<FONT COLOR=%s>/",c->comment);
				bits.cm1 = 1;
			}

			else if(!strncmp(&wstr[cnum],"*/",2)
				    &&!bits.qt1
					&&!bits.qt2
				    &&!bits.cm2
					&&(bits.cm1)
					&&bits.color)
			{
				fprintf(hfile,"*/</FONT>");
				cnum++;
				bits.cm1 = 0;
			}

			else if((!strncmp(&wstr[cnum],"//",2))
				     &&!bits.qt1
					 &&!bits.qt2
				     &&!bits.cm2
					 &&!bits.cm1
					 &&bits.color)
			{
				fprintf(hfile,"<FONT COLOR=%s>/",c->comment);
				bits.cm2 = 1;
			}

			/* color up the strings */
			else if(wstr[cnum]==34
				    &&(!bits.cm2)
					&&(!bits.cm1)
					&&(!bits.pre)
					&&bits.color)
			{
				if(wstr[cnum-1]!='\\') {
				if(!bits.qt1)
				{
					fprintf(hfile,"<FONT COLOR=%s>%c",c->string,34);
					bits.qt1 = 1;
				}
				else
				{
					fprintf(hfile,"%c</FONT>",34);
					bits.qt1 = 0;
				} }
			}

			/* color up the chars */
			else if(wstr[cnum]==39
					&&(!bits.qt1)
				    &&(!bits.cm2)
					&&(!bits.cm1)
					&&(!bits.pre)
					&&bits.color)
			{
				if(wstr[cnum-1]!='\\') {
				if(!bits.qt2)
				{
					fprintf(hfile,"<FONT COLOR=%s>%c",c->chr,39);
					bits.qt2 = 1;
				}
				else
				{
					fprintf(hfile,"%c</FONT>",39);
					bits.qt2 = 0;
				} }
			}

			/* color up the numbers */
			else if(wstr[cnum]>='0'&&
				    wstr[cnum]<='9'&&!(
					wstr[cnum-1]>'A'&&
					wstr[cnum-1]<'Z')&&!(
					wstr[cnum-1]>'a'&&
					wstr[cnum-1]<'z')&&
					wstr[cnum-1]!='_'
				    &&(!bits.cm2)
					&&(!bits.cm1)
					&&(!bits.qt1)
					&&(!bits.qt2)
					&&(!bits.num)
					&&(!bits.pre)
					&&bits.color)
			{
				fprintf(hfile,"<FONT COLOR=%s>%c",c->numbers,wstr[cnum]);
				bits.num = 1;
			}

			else if(bits.num /* to end number colouring */
				    &&((wstr[cnum]<'0')
				    ||(wstr[cnum]>'9')))
			{
			    fprintf(hfile,"</FONT>%c",wstr[cnum]);
				bits.num = 0;
			}

			/* color up the preprocessors */
			else if(wstr[cnum]=='#'
				    &&(!bits.cm2)
					&&(!bits.cm1)
					&&(!bits.qt1)
					&&(!bits.qt2)
					&&(!bits.pre)
					&&(bits.color))
			{
				fprintf(hfile,"<B>#");
				bits.pre = 1;
			}

			/* replace the C symbols
			 with their HTML equivalent */
			else if(wstr[cnum-1]==' '&&wstr[cnum]==' ')
			fprintf(hfile,"&nbsp;");
			else if(wstr[cnum]=='\t')
			fprintf(hfile,"&nbsp; &nbsp; ");
			else if(wstr[cnum]=='<')
			fprintf(hfile,"&lt;");
			else if(wstr[cnum]=='>')
			fprintf(hfile,"&gt;");
			else if(wstr[cnum]=='\n') {
				fprintf(hfile,"</FONT>");
				if(wstr[cnum-1]=='\\')
					bits.cont = 1;
				else
					bits.cont = 0;
			break;
			} else
			fprintf(hfile,"%c",wstr[cnum]);
		}

		if(bits.pre)
		fprintf(hfile,"</B>");

		/* end font tag */
		if(bits.cm1||
		   bits.cm2||
		   bits.qt1||
		   bits.qt2||
		   bits.num)
		fprintf(hfile,"</FONT>");

		/* end the line */
		fprintf(hfile,"<BR>\n");

		if(remaining)
			goto remainder;
	}

	fputs("</CODE><HR NOSHADE><FONT FACE=Arial>\n",hfile);
	fprintf(hfile,"No. of pages = %d<BR>\n",pages);
	fprintf(hfile,"No. of lines = %d<BR>\n",lines);
	fprintf(hfile,"No. of chars = %ld<BR>\n",charcount);
	fprintf(hfile,"No. of letters = %ld\n",letters);

	fputs("</FONT></BODY></HTML>",hfile);
	puts("\b\b\b100%\n\nTranslation Complete.");
	printf("HTML File: %s written.\n",htmlfile);

	free(wstr);
	fclose(hfile);
	fclose(cfile);
}

#undef COL_MAX

/*   -  < eqa >  -   */

/*******************************************
  EQUATION/EXPRESSION ANALYZER & EVALUATOR
********************************************/

double eqa(const char *str)
{
	void evali(const char *str, char *err, int *eval);
	puts("\nEquation Analysis...");
	char err[STR_MAX];
	strcpy(err,"");
	int val = 0;
	evali(str,err,&val);
	if(strlen(err))
		puts(err);
	printf("\n%s = %d\n", str, val);
	return val;
}

inline char strisn(const char *str)
{
	// checks wether a string consists of real/integer numbers.
	for(;*str!='\0';str++)
	if((*str<'0'||*str>'9')&&*str!='.')
	return *str; return 0;
}

void evali(const char *str, char *err, int *eval)
{
	//  Mathematical Expression Evaluation Function (int)
	// ---------------------------------------------------
	//  This functions solves infix expressions as long
	//	integers. When an infix expression is passed via
	//  *str, the expression is broken into parts, and the
	//	part with the highest prority is passed on recursively 
	//	onto the function itself. This recurvise process is 
	//  repeated until the whole expression has been solved.
	//  Results of the evaluation are stored in int *eval.

	char *cp = NULL;

	if((*str=='\0')||(strlen(str)==0))
	{
		strcpy(err,"Empty Expression");
		return;
	}
	int i = 0;  /* common loop counter */

	char bcheck = 0;
	for(i=0;*str!='\0';str++,i++)
	{
		/* check if there are brackets in an equation. */
		if(*str=='('||*str==')')
		/* bcheck is 0 if there are no brackets & vice-versa. */
		{ bcheck = 1;
		break; }
	}
	for(;i>0;i--)
	str--;  /* reverse str pointer to original position */

	if(!bcheck)
	{
		/* this is the core of eval where the calculations are done.
		at this level, the equation does not have any brackets. */

		const char symbols[]="^*/%+-&";
		const char se[]="Invalid Syntax";
		const char ue[]="Unknown Symbol";

		char *tmp = NULL;
		tmp = allocs(strlen(str)+1);
		*tmp='\0';

		/* check wether str has reached the absolute stage */
		if(prechar(tmp,str,symbols)==0)
		{
			 /* char to store num_check results */
			char ncheck = 0;
			ncheck = strisn(str);
			if(!ncheck)
			{
				/* str is a number (real or integer) */
				*eval = atoi(str);
				free(tmp);
				tmp = NULL;
			}
			else if(!isvalid_id(str))
			{
				double pass = 0;
				strcpy(tmp,str);
				eval_dbl(tmp, &pass);
				free(tmp);
				tmp = NULL;
				*eval = abs((int32)(floor(pass)));
			}
			else
			{
				/* not a variable, number nor function; hence an error */
				strcpy(err,se);
				strcat(err,": ");
				strcat(err,str);
				free(tmp);
				tmp = NULL;
			}
			return;
		}
		else  /* there are symbols in str */
		{
			free(tmp);
			tmp = NULL;
		}

		/* now search for symbols */
		char *pre = NULL; /* string preceding the operator */
		char *pos = NULL; /* string succeding the operator */

		/* now allocate the variables pre & pos */
		pre = allocs(strlen(str)+1);
		*pre='\0';

		pos = allocs(strlen(str)+1);
		*pos='\0';

		/* split the equation using the arithmetic
		operator that has the highest priority */
		char symbol = 0;

		if(prechar(pre,str,"^"))
		{ if(postchar(pos,str,"^"))
		symbol = '^'; }

		else if(prechar(pre,str,"*"))
		{ if(postchar(pos,str,"*"))
		symbol = '*'; }

		else if(prechar(pre,str,"/"))
		{ if(postchar(pos,str,"/"))
		symbol = '/'; }

		else if(prechar(pre,str,"%"))
		{ if(postchar(pos,str,"%"))
		symbol = '%'; }

		else if(prechar(pre,str,"+"))
		{ if(postchar(pos,str,"+"))
		symbol = '+'; }

		else if(prechar(pre,str,"-"))
		{ if(postchar(pos,str,"-"))
		symbol = '-'; }

		else if(prechar(pre,str,"&"))
		{ if(postchar(pos,str,"&"))
		symbol = '&'; }

		else {
		strcpy(err,ue);
		strcat(err,": ");
		*++err=symbol;
		*++err='\0';
		return; }

		char *ax = NULL; /* value preceding of operator */
		char *bx = NULL; /* value succeding the operator */
		char *cx = NULL; /* value of ax and bx processed */

		/* now allocate ax and bx */
		ax = allocs(strlen(pre)+1);
		*ax='\0';

		bx = allocs(strlen(pos)+1);
		*bx='\0';

		/* find out the contents of bx */
		char *ebx = NULL; /* temp string to build bx */
		ebx = allocs(strlen(pos)+1);
		*ebx='\0';

		strcpy(bx,pos);
		strcpy(ebx,bx);
		for(;;)  /* infinite loop */
		{
			if(!prechar(bx,ebx,symbols))
			{
				strcpy(bx,ebx);
				free(ebx);
				ebx = NULL;
				/* de-allocate ebx */
				break;
			}
			else /* here ebx is build */
			strcpy(ebx,bx);
		}

		/* find out the contents of ax */
		char *eax = NULL; /* temp string to build ax */
		eax = allocs(strlen(pre)+1);
		*eax='\0';

		strcpy(ax,pre);
		strcpy(eax,ax);
		for(;;)  /* infinite loop */
		{
			if(!postchar(ax,eax,symbols))
			{
				strcpy(ax,eax);
				free(eax);
				eax = NULL;
				/* de-allocate eax */
				break;
			}
			else /* here eax is build */
			strcpy(eax,ax);
		}

		/* variables to store (pre-ax) and (pre-bx) */
		char *prex = NULL;	/* string of (pre-ax) */
		char *posx = NULL;	/* string of (pos-ax) */

		/* now allocate prex and posx */
		prex = allocs(strlen(pre)+1);
		*prex='\0';

		posx = allocs(strlen(pos)+1);
		*posx='\0';

		/* find prex and posx */
		strlft(prex,pre,(strlen(pre)-strlen(ax)));
		strrht(posx,pos,(strlen(pos)-strlen(bx)));

		/* de-allocate pre & pos */
		free(pre);
		pre = NULL;
		free(pos);
		pos = NULL;

		/* process ax and bx to find cx */
		//printf("\nax=%s",ax);
		//printf("\nbx=%s",bx);
		int evala=0;
		int evalb=0;
		int evalc=0;

		if(strisn(ax))
		{
			tmp = clones(ax);
			double pass = 0;
			eval_dbl(tmp, &pass);
			int pass_ = 0;
			pass_ = abs((int32)(floor(pass)));
			itoa(pass_,ax,10);
		}
		if(strisn(bx))
		{
			tmp = clones(bx);
			double pass = 0;
			eval_dbl(tmp, &pass);
			int pass_ = 0;
			pass_ = abs((int32)(floor(pass)));
			itoa(pass_,bx,10);
			//strcpy(bx,"");
		}

		evala = atoi(ax);
		evalb = atoi(bx);
		switch(symbol)
		{
		case '^':	/* power */
			evalc = (int)(pow(evala,evalb));
			printf("pow  %d, %d\n", evala, evalb);
			break;
		case '*':	/* multiplication */
			evalc = evala * evalb;
			printf("mul  %d, %d\n", evala, evalb);
			break;
		case '/':	/* division */
			evalc = evala / evalb;
			printf("div  %d, %d\n", evala, evalb);
			break;
		case '%':	/* modulus */
			evalc = evala % evalb;
			printf("mod  %d, %d\n", evala, evalb);
			break;
		case '+':	/* addition */
			evalc = evala + evalb;
			printf("add  %d, %d\n", evala, evalb);
			break;
		case '-':	/* subtraction */
			evalc = evala - evalb;
			printf("sub  %d, %d", evala, evalb);
			break;
		case '&':	/* concatenation */
			//
			if((cp=(char *)(realloc(cx,sizeof(char)*
			(strlen(ax)+strlen(bx)+1))))!=NULL)
				cx = cp;
			else 
				msg(system_no_mem,4);
			strcpy(cx,ax);
			strcat(cx,bx);
			printf("cat  %s, %s\n", ax, bx);
			evalc = atoi(cx);
			break;
		default:
			strcpy(err,ue);
			strcat(err,": ");
			*++err=symbol;
			*++err='\0';
			return;
		}

		/* find the length of evalc */
		int cxlen = 8;
		if(evalc<0)
		{ cxlen = 16; }
		else if(evalc==0)
		{ cxlen = 2; }
		else {
			cxlen = 0;
			int ctmp = evalc;
			while(ctmp>0) { ctmp/=10; cxlen++; }
		}

		/* allocate cx having the length of evalc */
		cx = allocs(cxlen+1);
		*cx='\0';

		itoa(evalc,cx,10);
		//printf("\ncx=%s\n",cx);
		//printf("\nstep: %s",cx);

		/* de-allocate ax & bx */
		free(ax); ax = NULL;
		free(bx); bx = NULL;

		/* variable to store one-step solved equation */
		char *ex = NULL;
		ex = allocs(strlen(prex)+strlen(cx)+strlen(posx)+1);
		*ex='\0';

		/* find ex using cx in prex and posx */
		strcpy(ex,prex);
		strcat(ex,cx);
		strcat(ex,posx);

		/* now de-allocate cx, prex & posx */
		free(cx);
		free(prex);
		free(posx);
		cx = NULL;
		prex = NULL;
		posx = NULL;

		/* recurse ex on eval for next-step solving */
		int integer=0;
		evali(ex,err,&integer);
		*eval = integer;

		/* de-allocate ex & return */
		free(ex);
		ex = NULL;
		return;
	}
	else
	{
		// check if an equation is correct in no. of brackets.
		unsigned int brackets[2]={0,0};
		for(i=0;*str!='\0';str++,i++)
		{
			/* find the no. of brackets */
			if(*str=='(')
			brackets[0]+=1;
			if(*str==')')
			brackets[1]+=1;
		}
		/* reverse str pointer to original position */
		for(;i>0;i--)
		str--;  

		if(brackets[0]==brackets[1])
		{
			char *pres = NULL;
			char *poss = NULL;
			char *mids = NULL;
			char *midx = NULL;
			char *newx = NULL;

			/* now allocate the above strings */
			pres = allocs(strlen(str)+1);
			*pres='\0';

			pres = allocs(strlen(str)+1);
			*poss='\0';

			mids = allocs(strlen(str)+1);
			*mids='\0';

			midx = allocs(strlen(str)+1);
			*midx='\0';

			newx = allocs(strlen(str)+1);
			*newx='\0';

			/* now split the string */
			unsigned long int cs = 0;
			unsigned long int bcount = 0;

			/* make pres */
			for(cs=0;*str!='\0';cs++)
			{
				if(*str=='(')
				bcount++;
				if(bcount==brackets[0])
				break;
				*pres++ = *str++;
			}
			for(*pres = '\0';cs>0;cs--)
			pres--;

			/* make mids */
			for(cs=0,str++;((*str!=')')&&(*str!='\0'));cs++)
			*mids++ = *str++;
			for(*mids = '\0';cs>0;cs--)
			mids--;

			/* make poss */
			for(cs=0,str++;*str!='\0';cs++)
			*poss++ = *str++;
			for(*poss = '\0';cs>0;cs--)
			poss--;

			/* ok, now evaluate */
			int integer = 0;
			evali(mids,err,&integer);
			itoa(integer,midx,10);

			strcpy(newx,pres);
			strcat(newx,midx);
			strcat(newx,poss);

			evali(newx,err,&integer);
			*eval = integer;
			return;
		}
		else
		{
			strcpy(err,"Invalid Equation, inequal number of brackets.");
			return;
		}
	}
}

/*   -  < eas >  -   */

/******************************************
  E-Mail Address Scanner/Extraction Tool
*******************************************/

void eas()
{
	unsigned int mail_scan(char *sfile, char *dfile, char *err);

    puts("\n E-Mail Address Scanner ");
    puts(" ======================== ");
    puts(" This program searches random data and extracts ");
    puts(" e-mail addresses out of them if present. \n    "); 
    
    printf("Serch file: ");
    char sfile[255];
    fgets(sfile,255,stdin);
    sfile[strlen(sfile)-1] = '\0';
    
    printf("\nDump file: ");
    char dfile[255];
    fgets(dfile,255,stdin);
    dfile[strlen(dfile)-1] = '\0';
    
    char err[255];
    err[0] = '\0';
    unsigned int count = 0;
    count = mail_scan(sfile, dfile, err);
    
    if(!strlen(err))
      printf("\nScan Successful. Found %u addresses.\n",count);
    else
      printf("\nScan failed. Error: %s\n",err);
    
    fflush(stdin);
    getchar();    
}

unsigned int mail_scan(char *sfile, char *dfile,char *err)
{
     FILE *fs = NULL;
     fs = fopen(sfile,"r");
     if(!fs) {
       sprintf(err,"File '%s' does not exist.",sfile);
       return 0;
     }
     
     FILE *fd = NULL;
     fd = fopen(dfile,"w");
     if(!fd) {
       sprintf(err,"Could not create '%s' file.",dfile);
       return 0;
     }
          
     char str[STR_MAX];
     register unsigned int i;
     unsigned int count = 0;
     
     printf("\n");
     
     while(fgets(str,200,fs)==str)
     {
       char Slft[STR_MAX], Srht[STR_MAX], Email[STR_MAX];
       if(prechar(Slft,str,"@")=='@')
       {                        
         postchar(Srht,str,"@");
         
         for(i=0;Srht[i]!='\0';i++)
           if(!(isalnum(Srht[i])||Srht[i]=='.'||Srht[i]=='_'))
             Srht[i] = '\0';

         long int j = -1;
         for(i=0;Slft[i]!='\0';i++)
           if(!(isalnum(Slft[i])||Slft[i]=='.'||Slft[i]=='_'))
             j = i;
         j++;
         
         char *SlftT = NULL;    
         SlftT = clones(&Slft[j]);

         sprintf(Email,"%s@%s",SlftT,Srht);
         count++;
         fprintf(fd,"%s\n",Email);
         puts(Email);
       }
     }
     
     fclose(fd);
     fclose(fs);
     return count;
}

/*   -  < f2c >  -   */

/******************************************
  FILE  TO  C  LANGAUGE  TRANSLATOR v0.15
*******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#define ERR_SZ 256
#define STR_SZ 1024
#define BUF_SZ 2048

void f2c()
{
    void file2c(const char *filename, const char *cfilename, char *err, char platform);
	char filename[STR_SZ], cfilename[STR_SZ], err[STR_SZ+ERR_SZ], platform;
	err[0] = filename[0] = cfilename[0] = platform = '\0';

    puts("===========================================");
    puts(" <<  FILE  TO  C  LANGAUGE  TRANSLATOR  >> ");
    puts("===========================================");
    puts("  This program converts any ordinary file  "); 
    puts("  into a C program that generates the file ");
    puts("  from an array made of the file's bytes.  ");
    puts("  This program was written by Arjun Menon. ");
    puts("-------------------------------------------");
    puts("   U S A G E    I N S T R U C T I O N S :  ");
    puts(" Please enter the name of the file to translate");
    puts(" and target filename. To use same target filename, ");
    puts(" leave blank; otherwise enter the target name...");
    puts(" By default, the array is written in hexadecimal ");
    puts(" with 16 numbers on one line of code, to modify ");
    puts(" these settings precede the source filename with ");
    puts(" a single hyphen '-' character.");
    puts("-------------------------------------------");

    source_filename_entry:
    printf("\nSource Filename: ");

    fgets(filename,STR_SZ,stdin);
    string(filename);

    if(!file_exists(filename))
    {
      puts("No such file exists");
      goto source_filename_entry;
    }
    
    new_filename_entry:
	printf("\nNew Filename: "); 

    fgets(cfilename,STR_SZ,stdin);
    string(cfilename);

    if(file_exists(cfilename)) {
      puts("Warning file already exists.");
      printf("Continue operation (y/n) ? ");

      char option_key = '\0';
      fflush(stdin);
      option_key = getchar();
      option_key = tolower(option_key);
      fflush(stdin);

      if(option_key!='y' || option_key!='Y')
        goto new_filename_entry;
    }    
    
    if(!strlen(cfilename)) strcpy(cfilename, filename);
    printf("\
\nAvailable Platforms...\
\nDOS / Linux / UNIX (d)\
\nConsole Executable (c)\
\nWindows Executable (w)\
\nPlatform Option: ");

    fflush(stdin);
    platform = getchar();
    platform = tolower(platform);

    file2c(filename,cfilename,err,platform);
	if(strlen(err))
	printf("\nError: %s\n",err);

	puts("\nProgram Terminated.");
	fflush(stdin);
	getchar();

	return;
}

void file2c(const char *filename, const char *cfilename, char *err, char platform)
{
    if(!strlen(filename))
	{
		puts("\nPlease enter a filename.");
		return;
	}

	unsigned int cells = 16;
	char format = 'x';
	
	if(*filename=='-')
	{
        filename++;
        printf("\nHow many number per line (1 - 100): ");
        scanf("%u",&cells);

        if(cells<1||cells>100)
          cells = 16;
        fflush(stdin);
        
        printf("\nFormatting Hex(x), Dec(d), Octal(o), Character(c); press one key: ");
        format = getchar();
        format = tolower(format);

        if(format!='x'&&format!='d'&&format!='o'&&format!='c')
        format = 'x';
    }
    if(*cfilename=='-')
        cfilename++;
	
	FILE *infile = NULL;
	int bytes = 0;

	unsigned char buffer[BUF_SZ];
	
	if((infile = fopen(filename,"rb")) == NULL)
	{
		sprintf(err,"Could not open file '%s'",filename);
		fclose(infile);
		return;
    }
    
    unsigned long int fsize = 0;
    while(1)
    {
        bytes = fread(buffer, sizeof(char), BUF_SZ, infile);
        
        if(bytes>0)
          fsize += bytes;
        else
          break;
    }
    fclose(infile);
    infile = fopen(filename,"rb");
    
    char newfile[STR_SZ+3];
    sprintf(newfile,"%s.c",cfilename);

    FILE *cfile = NULL;
    cfile = fopen(newfile,"w");
    
    if(platform == 'w')
    {
      fprintf(cfile,"\
/* File Generator for: %s\n\
   Platform: Microsoft Windows  */\n\n\
/* Code automatically generated by\n\
   Arjun's Programming Toolkit */\n\n\
#include <windows.h>\n\
#include <fcntl.h>\n\n\
// WARNING: DO NOT MODIFY THIS ARRAY !\n\
unsigned char data[%lu]=\n{\n"
      ,cfilename,fsize);
    }
    else if(platform == 'c')
    {
      fprintf(cfile,"\n\
int main()\n{\
\n\tint file = open(\"%s\", O_CREAT|O_BINARY|O_WRONLY);\
\n\twrite(file, data, %lu);\n\tclose(file);\
\n\tsystem(file);\n\n\treturn 0;\n}\n"
      ,cfilename,fsize);
    }
    else
    {
      fprintf(cfile,"\
/* File Generator for: %s\n\
   Platform: DOS/Linux/UNIX  */\n\n\
/* Code automatically generated by\n\
   Arjun's Programming Toolkit */\n\n\
#include <stdio.h>\n\
#include <fcntl.h>\n\n\
// WARNING: DO NOT MODIFY THIS ARRAY !\n\
unsigned char data[%lu]=\n{\n"
      ,cfilename,fsize);
    }
    
    long unsigned int count = 0;
    short unsigned int i = 0, n = 0;

	while(1)
	{
        bytes = fread(buffer, sizeof(char), BUF_SZ, infile);

        if(bytes>0)
        {
            for(i=0; i<bytes; i++)
            {
                count++;

                if(count<fsize)
                {
                  switch(format) {
                  case 'c':
                       if(buffer[i]!='\n'
                       &&buffer[i]!='\r'
                       &&buffer[i]!='\0'
                       &&buffer[i]!='\''
                       &&buffer[i]!='\"'
                       &&buffer[i]!='\\')
                         fprintf(cfile,"'%c',",(unsigned char)(buffer[i]));
                       else
                         fprintf(cfile,"%u,",(unsigned char)(buffer[i]));
                       break;

                  case 'd':
                       fprintf(cfile,"%u,",(unsigned char)(buffer[i]));
                       break;

                  case 'o':
                       fprintf(cfile,"0%o,",(unsigned char)(buffer[i]));
                       break;

                  default:
                       fprintf(cfile,"0x%x,",(unsigned char)(buffer[i]));
                       break;
                  }
				  n++;

                  if(n>=cells)
				  {
                    fprintf(cfile,"\n");
                    n = 0;
				  }
                }

                else if(count==fsize)
                  switch(format) {
                  case 'd':
                       fprintf(cfile,"%u\n};\n",(unsigned char)(buffer[i]));
                       break;

                  case 'o':
                       fprintf(cfile,"0%o\n};\n",(unsigned char)(buffer[i]));
                       break;

                  default:
                       fprintf(cfile,"0x%x\n};\n",(unsigned char)(buffer[i]));
                       break;
                  }
            }
        }
        else break;
    }
    
    if(platform == 'w')
    {
      fprintf(cfile,"\n\
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)\n{\
\n\tchar str[255];\n\tif(!GetTempPath(255,str));\n\tstrcpy(str,\"C:\\\\\");\n\tstrcat(str,\"%s\");\
\n\tint file = open(str, O_CREAT|O_BINARY|O_WRONLY);\
\n\twrite(file, data, %lu);\n\tclose(file);\n\
\n\tSTARTUPINFO si;\n\tPROCESS_INFORMATION pi;\n\
\n\tZeroMemory( &si, sizeof(si) );\n\tsi.cb = sizeof(si);\
\n\tZeroMemory( &pi, sizeof(pi) );\n\
\n\tCreateProcess( NULL, str, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);\n\
\n\tCloseHandle( pi.hProcess );\n\tCloseHandle( pi.hThread );\n\n\treturn 0;\n}\n"
      ,cfilename ,fsize);
    }
    else
    {
      fprintf(cfile,"\n\
int main()\n{\
\n\tint file = open(\"%s\", O_CREAT|O_BINARY|O_WRONLY);\
\n\twrite(file, data, %lu);\n\tclose(file);\n\n\treturn 0;\n}\n"
      ,cfilename,fsize);
    }
    
	fclose(cfile);
	fclose(infile);

	printf("\nFile '%s' successfully translated to '%s'\n",filename,newfile);
}

#undef ERR_SZ
#undef STR_SZ
#undef BUF_SZ

//////////////////////////
// --- END OF CODE ---  //
//////////////////////////

