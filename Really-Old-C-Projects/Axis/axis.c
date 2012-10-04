/***************************************************
    AXIS (Advanced eXtensible Interpreter System)
   .---------------------------------------------.
   |    Program Name: AXIS CORE                  |
   |    Program Version: 1.0                     |
   |    Author Name: Arjun G. Menon              |
****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <math.h>

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

/*********.
| Globals |
`*********/

num32 linenum = 0;
num32 goto_line = 0;
byte debug_mode = 0;
struct bools;
struct double_list;
/* error message when malloc() or realloc() fails */
static const char system_no_mem[] = "Not enough memory.";
/* ai_error is used as an error descriptor string. */
static char *ai_error;

/**********************.
| Function Prototypes: |
`**********************/

/* application functions */
byte run_file(char *fname);
void axis_shell();
LIST axis_writer();
void axis_console();
void msg(const char *str, const byte mode);
void pause_key(void);

/* memory management: */
void init_memory();
num32 free_memory();
void init_dbl_list(struct double_list *dbl_list);
num32 free_dbl_list(struct double_list *dbl_list);
double * new_dbl(const char *id, struct double_list *dbl_list);
double * get_dbl(const char *id, struct double_list *dbl_list);
double * new_dbl_var(const char *var_id, struct double_list *dbl_list, num32 size);
double * get_dbl_var(const char *var_id, struct double_list *dbl_list, int32 index);
void resize_dbl_var(const char *id, struct double_list *dbl_list, num32 new_size);
char del_dbl_var(const char *id, struct double_list *dbl_list);
void mov_dbl_str(const char *id, struct double_list *dbl_list, const char *str);
void show_dbl_str(const char *id, struct double_list *dbl_list, FILE *buf);
void show_dbl_list(struct double_list *dbl_list, FILE *buf);
int32 * hash_newkey(const char *id, 
		struct hash_table_struct **hash_table,
		num32 *hash_table_size);
int32 * hash_getkey(const char *id,
		struct hash_table_struct **hash_table,
		num32 *hash_table_size);
void exchange_vars(char *lhs, char *rhs);
LIST make_list(const num32 size, const num32 length);
void free_list(LIST list, const num32 size);
void store_str(char *str, LIST* str_array, num32 *array_size);
const char * fetch_str(num32 index, LIST* str_array,
					   const num32 array_size);
void free_str_array(LIST* str_array, num32 array_size);

/* core execution: */
void initialize();
void terminate();
byte prog_write(char *str);
const char * prog_read(num32 index);
void prog_show(FILE *buf);
void free_prog_table();
void set_goto_line(const char *str, num32 line);
num32 get_goto_line(const char *str);
void free_goto_table();
void run_line(const char *str);
num32 exec_line(const char *str, byte direct);
void asgn(char *rhs, char *lhs);
void mov_dbl(double *value, 
double *rvalue, char pop_symbol);
void eval_dbl(const char *expr, double *value);
void sval_dbl(const char *str, double *value);
void xval_dbl(const char *str, double *value);
double call_func(char ** list, num32 args);
void call_proc(char *str);

/* language executives */
num32 invoke_keywords(const char *str, 
			int32 *loop, byte *cont);
void operation_print(char *str, char newline);
void operation_term();
void operation_del(char *str);
void operation_dump();
void operation_dumpf(char *str);
void operation_debug(char *str);
void operation_debugm();
void operation_pause();
void operation_exec(char *str);
void operation_lisp(char *str);
void operation_write(char *str);
void operation_repeat(char *str, int32 *loop);
void operation_progl(char *str);
void operation_prog();
void operation_file(char *str);
void operation_release();
void operation_scan(char *str);
void operation_store(char *str);
void operation_scribble(char *str);
void operation_exit();

/* lexical analysis */
LIST lex_expr(const char *str, num32 *args);
LIST lex_command(const char *str, num32 *args, 
				 num32 strn, num32 str_sz);
LIST lex_function(const char *str, num32 *args, 
				  num32 strn, num32 str_sz);
void lex_equation(const char *str, char *lhs, char *rhs);
int32 lex_subscript(const char *str, char *var_name);
char check_format(const char *str);
byte isvalid_id(const char *str);
void verify_brackets(const char *str);

/* mathematical functions */
byte eval_prior(char e);
num32 factorial(num32 value);
int32 concatenate(int32 lhs, int32 rhs);
int32 fibonacci(int32 n);

/* string manupilation: */
void trim(char *str, const char T);
void rtrim(char *str, const char T);
void ltrim(char *str, const char T);
void trims(char *str);
void strclean(char *str, const char T);
char prechar(char *str, const char *src, const char *key);
char postchar(char *str,const char *src, const char *key);
int strrht(char *str, const char *src, const num32 n);
int strlft(char *str, const char *src, const num32 n);
int strmid(char *str, const char *src, 
		   const num32 m, const num32 n);
void strnum(char *str, const num32 num, const num32 len);
int32 strsim(const char *str, const char *cmp);
void strip_quotes(char *str);
void string(char *str);
void strlower(char *str);
void strupper(char *str);
void strchar(char *str, const char ch);
void strcatc(char *str, const char chr);
char strisc(const char *str, const char chr);
num32 charcount(const char *str, const char ch);
void strbluff(char *str, const char *bluff, const num32 n);
void strnully(char *str, const num32 n);
void strnull(char *str);

/* string allocation: */
char * allocs(num32 len);
char * reallocstr(char *str);
char * clones(const char *str);
char * twins(const char *str, num32 len);

/* file system management functions: */
char file_exists(const char *fname);

/* --- core standard library --- */

/* core interface & language functions: */
double clib_if(char *S1);
double clib_not(char *S1);
double clib_del(char *S1);
double clib_print(char *S1);
double clib_input(byte evaluative, char *S1);
double clib_read(char *S1, byte create);

/* extensions to the C mathematics library */
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
double clib_decimal(char *S1);
double clib_golden(char *S1);
double clib_goldens(char *S1);
double clib_fibonacci(char *S1);
double clib_fibonaccis(char *S1);

/*******************.
| Console Interface |
`*******************/

int main(int argc, char *argv[])
{
	int32 i;
	char str[1024];

	/* program starts here */
	if(argc<=1)
	{
		/* there are no arguments, so run the shell */
		axis_shell();
	}
	else
	{
		/* open each file (arg as filename) and run it. */
		
		strcpy(str,"");
		for(i=1; i<argc; i++)
		{
			strcat(str,argv[i]);
			strcat(str," ");
		}
		
		// could be a path with spaces in it.
		if(!run_file(str))
		{
			// run each arg
			for(i=1; i<argc; i++) {
				if(!run_file(argv[i])) {
					printf("\nCould not open file: %s\n",
						argv[i]); pause_key();
				}
			}
		}
	}

	puts("\nGoodbye.");
	return EXIT_SUCCESS;
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
	char str[STR_MAX];
	int i;
	FILE *fp = NULL;

	program = axis_writer();
	if(program == NULL)
		axis_console();

	else {
		printf("\nSave program? (Hit Enter to decline): ");
		fgets(str,STR_MAX,stdin);
		string(str);
		if(strlen(str)) {
			fp = fopen(str,"w");
			for(i=0; *program[i]!='\0'; i++) {
				fwrite(program[i],strlen(program[i]),1,fp);
				fputc('\n',fp);  /* insert newline */
			}
			fclose(fp);
		}
	}
}

LIST axis_writer()
{
	char str[STR_MAX];
	LIST prog = NULL;
	num32 lines_written = 0;
	byte count = 0;
	char again[STR_MAX];

	puts(
"\n*---------------------------------------\n\
| Welcome to the AXIS terminal. Type the   \n\
| program you wish to execute and press  \n\
| Enter key twice to run the program... \n\
`_______________________________________\n"
	);

	prog = make_list(MAX_LINES, STR_MAX);

	// reception stage
	while(count<2)
	{
		fflush(stdin);
		fgets(str,STR_MAX,stdin);
		string(str);
		trims(str);
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
	fgets(again,STR_MAX,stdin);
	string(again);
	if(!strsim(again,"yes") 
	|| !strsim(again,"y"))
		goto execute;

	return prog;
}

void axis_console()
{
	char str[STR_MAX];
	initialize();
	puts("AI:SHELL");

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
		trims(str);
		if(!strlen(str)) continue;
		if(!strsim(str,"exit")) break;
		run_line(str);
	}
	terminate();
}

/**************************.
| Memory Management System |
`**************************/

struct double_list {
	char *id;
	num32 size;
	double *value;
	struct double_list * link;
} glob_dbl_list;

void init_memory()
{
	init_dbl_list(&glob_dbl_list);
}

num32 free_memory()
{
	num32 freed = 0;
	freed += free_dbl_list(&glob_dbl_list);
	free_prog_table();
	free_goto_table();
	return freed;
}

double * new_dbl(const char *id, struct double_list *dbl_list)
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

	return new_dbl_var(var_id, dbl_list, size);
}

double * get_dbl(const char *id, struct double_list *dbl_list)
{
	/* find out request details */
	char *var_id = allocs(strlen(id));
	int32 index = lex_subscript(id, var_id);

	if(id == NULL || !strcmp(id,"_"))
		return dbl_list ->value;

	return get_dbl_var(var_id, dbl_list, index);
}

double * new_dbl_var(const char *var_id, struct double_list *dbl_list, num32 size)
{
	// this functions constructs a variable/array 
	// entry in the given double_list linked list

	struct double_list * new_var = NULL;
	register num32 i = 0;

	/* malloc a new double_list */
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

	/* initialize the variable */
	for(;i < new_var ->size; i++)
		*((new_var ->value)+i) = 0;

	/* link and return the variable */
	new_var ->link = dbl_list ->link;
	dbl_list ->link = new_var;

	return new_var ->value;
}

double * get_dbl_var(const char *var_id, struct double_list *dbl_list, int32 index)
{
	// this functions retrieves a variable from a double_list

	/* get the double from the list & return it's pointer. */
	struct double_list *next = dbl_list;
	while(next ->link != NULL)
	{
		next = next ->link;
		if(!strsim(next ->id, var_id)) {
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

void resize_dbl_var(const char *id, struct double_list *dbl_list, 
					num32 new_size)
{
	// this functions dynamically resizes 
	// a variable stored in a double_list

	struct double_list *next = dbl_list;
	num32 diff;

	if(id == NULL || !strcmp(id,"_"))
		return;
	if(new_size < 1)
		new_size = 1;

	/* get the double from the list & return it's pointer. */
	while(next ->link != NULL)
	{
		next = next ->link;
		if(!strsim(next ->id, id)) {
			next ->value = (double *)
			(realloc(next ->value, new_size * sizeof(double)));

			diff = 0;
			if(new_size > next ->size)
				diff = new_size - next ->size;
			if(diff > 0) {
				register num32 i = next ->size;
				for(;i <= new_size; i++)
					*((next ->value)+i) = 0;
			}
			next ->size = new_size;
		}
	}
}

char del_dbl_var(const char *id, struct double_list *dbl_list)
{
	/* delete the variable 'id' from the 
	   double list, if ok; return 1 else 0. */

	struct double_list *old_var = NULL;
	struct double_list *next = dbl_list;

	while(next ->link != NULL)
	{
		if(!strcmp(next ->link ->id, id))
		{
			old_var = next ->link;
			next ->link = old_var ->link;
			free(old_var ->value);
			old_var ->value = NULL;
			free(old_var);
			old_var = NULL;
			return 1; /* ok */
		}
		next = next ->link;
	}
	return 0;  /* failed */
}

void mov_dbl_str(const char *id, struct double_list *dbl_list, const char *str)
{
	// tranfer a string to a double array

	num32 size = strlen(str);
	resize_dbl_var(id, dbl_list, size);

	double *dp = NULL;
	register num32 i = 0;

	for(; i<size ; i++) {
		dp = get_dbl_var(id, dbl_list, i);
		*dp = str[i];
	}
}

void show_dbl_str(const char *id, struct double_list *dbl_list, FILE *buf)
{
	double *dp = NULL;
	dp = get_dbl_var(id, dbl_list, -1);
	if(dp == NULL) {
		ai_error = allocs(strlen(id)+STR_MAX);
		sprintf(ai_error,"Variable \"%s\" does not exist.",id);
		ai_error = reallocstr(ai_error);
		return;
	}
	num32 size = abs((int32)(floor(*dp)));

	register num32 i = 0;
	register char out = 0;

	for(; i<size ; i++) {
		dp = get_dbl_var(id, dbl_list, i);
		out = (char)(abs((int32)(floor(*dp))));
		fprintf(buf,"%c",out);
	}
}

void show_dbl_list(struct double_list *dbl_list, FILE *buf)
{
	/* dumps the content of a double_list
	   into the specified file pointer */

	struct double_list *next = dbl_list;
	while(next ->link != NULL)
	{
		next = next ->link;
		if(next ->size > 1) {
			register num32 i = 0;
			for(;i < next ->size; i++)
			fprintf(buf,"%s[%lu] = %lf\n",
			next->id,i,*(next->value+i));
		}
		else fprintf(buf,"%s = %lf\n",
		next->id,*(next->value));
	}
}

void init_dbl_list(struct double_list *dbl_list)
{
	dbl_list ->size = 1;
	dbl_list ->link = NULL;
	dbl_list ->value = (double *)(malloc(sizeof(double)));

	if(dbl_list ->value == NULL)
		msg(system_no_mem, 4);
	*(dbl_list ->value) = 0;
}

num32 free_dbl_list(struct double_list *dbl_list)
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

struct hash_table_struct
{
	char *ids;  /* key identifier */
	int32 key;  /* key value */
};

int32 * hash_newkey(const char *id, 
		struct hash_table_struct **hash_table,
		num32 *hash_table_size)
{
	/*  This function creates a new key in the 
	 *  hash table whose name is defined by the 
	 *  string *id and returns a pointer to it.
	 */

	/*  --- WARNING --- [ CHECK *id VALIDITY BEFORE PASSING ]
	 * newkey() does not check the validity of key names;
	 * even null key names, are accepted. Before calling
	 * newkey check the validity of the string being passed.
	 */

	/* check if a key with the same name exists */
	if((hash_getkey(id, hash_table, hash_table_size))!=NULL)
		return NULL;
	struct hash_table_struct * table_ptr;

	/* increment table size by 1 for 1 new key */
	*hash_table_size += 1;

	/* reallocate table structure with new size */
	table_ptr = (struct hash_table_struct *) 
	(realloc(*hash_table, 
	sizeof(struct hash_table_struct)*
	(*hash_table_size)));

	if(table_ptr == NULL)
		msg(system_no_mem,4);
	else /* set the key */
		*hash_table = table_ptr;

	/* set the key name */
	(*hash_table)[*hash_table_size-1].ids = clones(id);

	/* initiaze the key to 0 */
	(*hash_table)[*hash_table_size-1].key = 0;

	/* return a pointer to the key */
	return &(*hash_table)[*hash_table_size-1].key;
}

int32 * hash_getkey(const char *id,
		struct hash_table_struct **hash_table,
		num32 *hash_table_size)
{
	/*  This function returns a pointer to an
	 *  existing key whose is name is string *id
	 *  and is in the hash table.
	 */

	/* check if hash table is empty */
	if(!*hash_table_size)
		return NULL;

	/* loop counter to find position in array */
	num32 table_pos = 0;

	/* find out the location of the key */
	for(;table_pos<*hash_table_size;table_pos++)
		if(!strcmp((*hash_table)++ ->ids,id))
			break;

	/* reverse hash table pointer to origin  */
	*hash_table -= table_pos+1;

	/* check if the key exists */
	if(table_pos>=*hash_table_size) {
		(*hash_table)++;
		return NULL;
	}

	/* return a pointer to the key */
	return &(*hash_table)[table_pos].key;
}

LIST make_list(const num32 size, const num32 length)
{
	/* creates a list of strings */
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
	if(!list)
		return;
	num32 i = 0;
	for(i=0; i<size; i++)
	{
		if(list[i]) {
			free(list[i]);
			list[i] = NULL;
		}
	}
}

void store_str(char *str, LIST* str_array, num32 *array_size)
{
	/* writes a string to a list of strings */
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
	/* fetches a string from a list of strings */
	if(index <= array_size)
		return (*str_array)[index];
	else return NULL;
}

void free_str_array(LIST* str_array, num32 array_size)
{
	/* destroys a list of strings */

	if(str_array == NULL)
		return;
	if(*str_array == NULL)
		return;

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

static LIST prog_table = NULL;
num32 prog_table_size = 0;

byte prog_write(char *str)
{
	if(!strlen(str) || *str == '\n')
		return 0; /* string rejected */
	store_str(str, &prog_table, &prog_table_size);
	return 1; /* success */
}

const char * prog_read(num32 index)
{
	if(index > 0 && index <= prog_table_size) 
		return fetch_str(index-1, 
		&prog_table, prog_table_size); 
	else
		return NULL;
}

void prog_show(FILE *buf)
{
	register num32 i = 0;
	for(i=0; i<prog_table_size; i++)
		fprintf(buf, "%s\n", prog_table[i]);
}

void free_prog_table()
{
	free_str_array(&prog_table, prog_table_size);
	prog_table = NULL;
	prog_table_size = 0;
}

hash_table_struct * goto_table =  NULL;
num32 goto_table_size = 0;
char *future_goto = NULL;

void set_goto_line(const char *str, num32 line)
{
	if(!strlen(str) || *str == '\n' || isvalid_id(str)) {
		ai_error = clones("Invalid Label Syntax.");
		return;
	}
	int32 * next_line = 
	hash_newkey(str, &goto_table, &goto_table_size);
	if(next_line)
		*next_line = linenum;
}

num32 get_goto_line(const char *str)
{
	num32 next_line = 0;
	int32 *next = 0;
	next = hash_getkey(str, &goto_table, &goto_table_size);

	if(next)
	{
		next_line = (num32)(abs(*next));
		return next_line;
	}
	else
	{
		if(!isvalid_id(str)) {
			future_goto = clones(str);
			return linenum+1;
		}
		/* evaluate line number */
		char *tmp = clones(str);
		double val=0;
		eval_dbl(tmp,&val);

		next_line = (num32)(abs((int32)(floor(val))));
		free(tmp);
		return next_line;
	}
}

void free_goto_table()
{
	if(goto_table)
		free(goto_table);
	goto_table = NULL;
}

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

FILE *file_io = NULL;

void initialize()
{
	init_memory();
}

void terminate()
{
	linenum = 0;
	goto_line = 0;
	if(file_io)
		fclose(file_io);
	printf("\n%lu variable(s) freed\n", free_memory());
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
	trims(code);

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
	code = reallocstr(code);

	if(prog_write(code))
		linenum++;
		/* number of lines executed */
	else
		return;
	//printf("\n%d = %d\n",linenum,prog_table_size);
	num32 next_line;

	/* goto controller */
	if(goto_line > linenum) {
		//printf("%s skipped.\n",code);
		if(future_goto) {
			if(strlen(future_goto)) {
				goto_line++;
				if(str[strlen(str)-1]==':') {
					if(!strncmp(str,future_goto,
						strlen(future_goto))) {
							goto_line--;
						goto okay;
					}
				}
			}
		}
		goto quit;
	}
	okay:

	/* now execute code */
	ai_error = NULL;
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

		while(i <= linenum) {
			const char *ns = NULL;
			ns = prog_read(i);
			if(ns == NULL)
				break;

			/* now execute code */
			//puts(ns);
			ai_error = NULL;
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
		if(future_goto)
			if(strlen(future_goto))
				goto_line++;
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

	ai_error = NULL;

	//////////////////
	// VERIFY SYNTAX
	//////////////////

	verify_brackets(str);

	if(ai_error)
		if(strlen(ai_error))
			return 0;

	const num32 str_sz = strlen(str);

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

	////////////////////
	// INVOKE KEYWORDS
	////////////////////

	/* invoke keyword if present */
	byte cont = 0;
	int32 ret = invoke_keywords(str, &loop, &cont);
	if(!cont)
		return ret;

	/* there is no keyword... */

	/////////////////////
	// SPLIT EXPRESSION
	/////////////////////

	char *lhs = allocs(str_sz);
	char *rhs = allocs(str_sz);

	lex_equation(str, lhs, rhs);

	////////////////////////
	// EXECUTE INSTRUCTION 
	////////////////////////

	if(*rhs=='\0') /* <- empty rhs, so no assignment involved. */
	{
		/* check if lhs is a variable */
		double *value = NULL;
		if(*lhs == '$')
			value = get_dbl(&lhs[1], &glob_dbl_list);
		else
			value = get_dbl(lhs, &glob_dbl_list);
		if(ai_error)
			if(strlen(ai_error))
				return 0;

		if(value==NULL) {
			// lhs is NOT a variable

			/* check for label */
			if(lhs[strlen(lhs)-1]==':') {
				lhs[strlen(lhs)-1] = '\0';
				trims(lhs);
				set_goto_line(lhs, linenum);
				return 0;
			}

			/* evaluate & store in _ */
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
					if(val>0) puts("\ntrue");
					else puts("\nfalse");
				}
			}
		}
		else {
			printf("\n%s = ",lhs);
			operation_print(lhs, 1);
		}
	}
	else /* rhs exists  */
	{
		/* check for [=>] exchange directive */
		if(*rhs=='>')
			exchange_vars(lhs,rhs);

		/* variable creation/assignment */
		else
			asgn(rhs,lhs);
	}

	/* free memeory */
	free(lhs);
	lhs = NULL;
	free(rhs);
	rhs = NULL;
	return 0;
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
	trims(rhs);

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
			ai_error = allocs(strlen(lhs)+STR_MAX);
			sprintf(ai_error,
			"LHS Variable '%s' does not exist.",lhs);
			ai_error = reallocstr(ai_error);
		}
	}
	else {
		ai_error = allocs(strlen(rhs)+STR_MAX);
		sprintf(ai_error,
		"RHS Variable \"%s\" does not exist.",rhs);
		ai_error = reallocstr(ai_error);
	}
}

void asgn(char *rhs, char *lhs)
{
	/*
	 * Variable Assignment Function:
	 * asgn() takes two strings lhs
	 * and rhs as arguments. lhs is
	 * usually a variable name, and
	 * rhs the data to assign to it.
	 * asgn() tries to do this, and if
	 * it can't it dumps an description
	 * about the error in *err.
	 */

	double *value = NULL;
	double rvalue = 0;
	byte string_flag = 0;

	trims(lhs);
	trims(rhs);

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
	trims(lhs);
	
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
			if(check_format(rhs_list[i]))
				eval_dbl(rhs_list[i],&rvalue);
			else string_flag = 1;

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

			/* resize array */
			char *tmp = clones(lhs_list[i]);
			if(lex_subscript(lhs_list[i], tmp) == -1) {
				mov_dbl(value,&rvalue,esymbol);
				resize_dbl_var(tmp, &glob_dbl_list, 
					(num32)(abs((int32)(floor(*value)))));
			}

			/* set the value */
			else if(value!=NULL) {
				if( !string_flag )
					mov_dbl(value,&rvalue,esymbol);
				else {
					strip_quotes(rhs_list[i]);
					mov_dbl_str(lhs_list[i], 
						&glob_dbl_list, rhs_list[i]);
				}
			}

			/* getint32() hasn't worked properly */
			else ai_error = clones("Fatal Data Transfer Error.");

			if(debug_mode) {
				if( !string_flag ) printf("\n%s = %lf\n",
					lhs_list[i],*value);
				else printf("\n%s = \"%s\"\n",
					lhs_list[i],rhs_list[i]);
			}
			free(tmp);
			tmp = NULL;
		}
	}
	else
	{
		/* evaluate rhs */
		if(check_format(rhs))
			eval_dbl(rhs,&rvalue);
		else string_flag = 1;

		/* create the variable */
		value = new_dbl(lhs, &glob_dbl_list);
		if(ai_error)
			if(strlen(ai_error))
				return;

		/* if variable already exists, then get it. */
		if(value==NULL)
		value = get_dbl(lhs, &glob_dbl_list);
		if(ai_error)
			if(strlen(ai_error))
				return;

		/* set the value */	
		if(value!=NULL) {
			if( !string_flag )
				mov_dbl(value,&rvalue,esymbol);
			else {
				strip_quotes(rhs);
				mov_dbl_str(lhs, &glob_dbl_list, rhs);
			}
		}

		/* getint32() hasn't worked properly */
		else ai_error = clones("Fatal Data Transfer Error.");
	}
}

void mov_dbl(double *value, double *rvalue, char pop_symbol)
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
	*value  = *rvalue; /* this simply sets the same value */
}

void eval_dbl(const char *expr, double *value)
{
	/*    Expression Evaluation Function (int32)
	 *  ==========================================
	 *  This function evaluates an infix expression
	 *  for an int32 result. First, it translates
	 *  the given expression to the postfix
	 *  form, and then evaluates the postfix
	 *  expression using the peval function.
	 *  The result is stored in double *value.
	 */

	char *str = clones(expr);
	char *str_ptr = str;
	trims(str);

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
	 *  Based on these rules the postfix expression is designed.
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
			trims(tmp);

			for(cur=0;tmp[cur]!='\0';cur++)
				/* go to end of cur*/;

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
			if(*str=='=' && str[1]=='=')
				/* logical == */
				*++str='E';

			else if(*str=='<' && str[1]=='>')
				/* logical != */
				*++str='N';

			else if(*str=='>' && str[1]=='=')
				/* logical >= */
				*++str='F';

			else if(*str=='<' && str[1]=='=')
				/* logical <= */
				*++str='G';

			else if(*str=='&' && str[1]=='&')
				/* logical AND */
				*++str='A';

			else if(*str=='|' && str[1]=='|')
				/* logical OR */
				*++str='O';
			else if(*str=='*' && str[1]=='*')
				/* power */
				*++str='P';

			else if(*str=='/' && str[1]=='/')
				/* power */
				*++str='I';

			else if(*str=='>' && str[1]=='>')
				/* bitwise right shift (>>) */
				*++str='R';

			else if(*str=='<' && str[1]=='<')
				/* bitwise left shift (<<) */
				*++str='L';

			else if(*str=='>' && str[1]=='<')
				/* maximum value */
				*++str='M';

			else if(*str=='+' && str[1]=='&')
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

		///////////////////////////////
		/// MATHEMATICAL EVALUATION ///
		///////////////////////////////
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
		          stack[ival-1] ==  /* == */
		          stack[ival-2] ? 1 : 0; break;

		case 'N': stack[ival---2] = 
		          stack[ival-1] ==  /* <> */
		          stack[ival-2] ? 0 : 1; break;

		case '>': stack[ival---2] = 
		          stack[ival-1] <  /* > */
		          stack[ival-2] ? 1 : 0; break;

		case 'F': stack[ival---2] = 
		          stack[ival-1] <=  /* >= */
		          stack[ival-2] ? 1 : 0; break;

		case '<': stack[ival---2] = 
		          stack[ival-1] > /* < */
		          stack[ival-2] ? 1 : 0; break;

		case 'G': stack[ival---2] = 
		          stack[ival-1] >=  /* <= */
		          stack[ival-2] ? 1 : 0; break;

		case 'A': stack[ival---2] =
		        ((stack[ival-1]>0)&&(  /* && */
		          stack[ival-2]>0)) ? 1 : 0; break;

		case 'O': stack[ival---2] =
		        ((stack[ival-1]>0)||(  /* || */
		          stack[ival-2]>0)) ? 1 : 0; break;

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
	free(str_ptr);
}

byte eval_prior(char e)
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
	/* hexadecimal */ sscanf(str,"%lfx",value);

	else if((*str>='0'&&*str<='9')||
	(*str=='-'&&str[1]>='0'&&str[1]<='9'))
	/* decimal */ *value = atof(str);

	else if(*str==39)  /* ['] */
	*value = (int32)(*++str);

	else if(!strsim(str, "true"))
		*value = 1;
	else if(!strsim(str, "false"))
		*value = 0;

	else {
		/* get the value */
		xval_dbl(str, value);
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

void xval_dbl(const char *str, double *value)
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
		if(n == 1)
		{
			if(!strcmp("_",str))
				/* default variable _ */
			{
				*value = *get_dbl(0, &glob_dbl_list);
				free_list(list,n);
				return;
			}
			else if(get_dbl(str, &glob_dbl_list) != NULL)
				/* extract variable */
			{
				*value = *get_dbl(str, &glob_dbl_list);
				free_list(list,n);
				return;
			}
			else
				/* variable does not exist */
				call_proc(list[0]);
		}
		else /* call function */
		{
			*value = call_func(list,n);
			free_list(list,n);
		}
	}
	else {
		ai_error = allocs(strlen(str)+STR_MAX);
		sprintf(ai_error,"Invalid Synatax: %s",str);
		ai_error = reallocstr(ai_error);
	}
}

/**************************.
| Core Language Executives |
`**************************/

num32 invoke_keywords(const char *str, int32 *loop, byte *cont)
{
	/*
	 * Invoke Keywords: This function takes 
	 * a string as argument and searches
	 * it for any keywords, if found; they
	 * are invoked/executed. The *loop allows
	 * this function to configure up loops
	 * defined using keywords. The pointer
	 * cont tells the caller wether to continue;
	 * ie : if a keyword is found cont is 0, and
	 * if not found, then cont is 1.
	 */
	
	char *tmp = allocs(strlen(str));

	///////////////////////////
	// INVOKE KEY SUBROUTINES
	///////////////////////////

	*cont = 0;

	/* invoke << pass >> */

	if(!strlft(tmp,str,4) && !strsim(tmp,"pass"))
	{
		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << term >> */

	else if(!strlft(tmp,str,4) && !strsim(tmp,"term"))
	{
		operation_term();
		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << delete >> */

	else if(!strlft(tmp,str,4) && !strsim(tmp,"del "))
	{
		postchar(tmp,str," ");
		trims(tmp);
		operation_del(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << dump >> */

	else if(!strlft(tmp,str,5) && !strsim(tmp,"dump "))
	{
		postchar(tmp,str," ");
		trims(tmp);
		operation_dumpf(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	else if(!strlft(tmp,str,4) && !strsim(tmp,"dump"))
	{
		operation_dump();
		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << debug >> */

	else if(!strlft(tmp,str,6) && !strsim(tmp,"debug "))
	{
		postchar(tmp,str," ");
		trims(tmp);
		operation_debug(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	else if(!strlft(tmp,str,5) && !strsim(tmp,"debug"))
	{
		operation_debugm();
		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << pause >> */

	else if(!strlft(tmp,str,5) && !strsim(tmp,"pause"))
	{
		operation_pause();
		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << exec >> */

	else if(!strlft(tmp,str,5) && !strsim(tmp,"exec "))
	{
		postchar(tmp,str," ");
		trims(tmp);
		operation_exec(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << write >> */

	else if(!strlft(tmp,str,6) && !strsim(tmp,"write "))
	{
		postchar(tmp,str," ");
		trims(tmp);
		operation_write(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << println >> */

	else if(!strlft(tmp,str,8) && !strsim(tmp,"println "))
	{
		postchar(tmp,str," ");
		trims(tmp);
		operation_print(tmp, 1);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	else if(!strlft(tmp,str,7) && !strsim(tmp,"println"))
	{
		postchar(tmp,str,"( ");
		trims(tmp);
		trim(tmp,'(');
		trim(tmp,')');
		operation_print(tmp, 1);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << print >> */

	else if(!strlft(tmp,str,6) && !strsim(tmp,"print "))
	{
		postchar(tmp,str," ");
		trims(tmp);
		operation_print(tmp, 0);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	else if(!strlft(tmp,str,5) && !strsim(tmp,"print"))
	{
		postchar(tmp,str,"( ");
		trims(tmp);
		trim(tmp,'(');
		trim(tmp,')');
		operation_print(tmp, 0);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	else if(!strlft(tmp,str,7) && !strsim(tmp,"newline"))
	{
		postchar(tmp,str," ");
		printf("\n");

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << repeat >> */

	else if(!strlft(tmp,str,7) && !strsim(tmp,"repeat "))
	{
		postchar(tmp,str," ");
		operation_repeat(tmp, loop);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << goto >> */

	else if(!strlft(tmp,str,5) && !strsim(tmp,"goto "))
	{
		postchar(tmp,str," ");
		trims(tmp);
		num32 next_line = get_goto_line(tmp);

		free(tmp);
		tmp = NULL;
		return next_line;
	}
	else if(*str == '>')
	{
		postchar(tmp,str,">");
		trims(tmp);
		num32 next_line = get_goto_line(tmp);

		free(tmp);
		tmp = NULL;
		return next_line;	
	}

	/* invoke << prog >> */

	else if(!strlft(tmp,str,5) && !strsim(tmp,"prog "))
	{
		postchar(tmp,str," ");
		trims(tmp);
		operation_progl(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	else if(!strlft(tmp,str,4) && !strsim(tmp,"prog"))
	{
		printf("\n");
		operation_prog();

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << lisp >> */

	else if(!strlft(tmp,str,5) && !strsim(tmp,"lisp "))
	{
		postchar(tmp,str," ");
		operation_lisp(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << file >> */

	else if(!strlft(tmp,str,5) && !strsim(tmp,"file "))
	{
		postchar(tmp,str," ");
		trims(tmp);
		operation_file(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << release >> */

	else if(!strlft(tmp,str,7) && !strsim(tmp,"release"))
	{
		operation_release();
		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << scan >> */

	else if(!strlft(tmp,str,5) && !strsim(tmp,"scan "))
	{
		postchar(tmp,str," ");
		trims(tmp);
		operation_scan(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << store >> */

	else if(!strlft(tmp,str,6) && !strsim(tmp,"store "))
	{
		postchar(tmp,str," ");
		trims(tmp);
		operation_store(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << scribble >> */

	else if(!strlft(tmp,str,9) && !strsim(tmp,"scribble "))
	{
		postchar(tmp,str," ");
		trims(tmp);
		operation_scribble(tmp);

		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* invoke << exit >> */

	else if(!strlft(tmp,str,4) && (
	!strsim(tmp,"exit") || !strsim(tmp,"quit")))
	{
		operation_exit();
		free(tmp);
		tmp = NULL;
		return 0;
	}

	/* ! nothing works ! */
	else
	{
		free(tmp);
		tmp = NULL;
		*cont = 1;
		return 0;
	}
}

void operation_print(char *str, char newline)
{
	LIST segment = NULL;
	num32 n = 0;
	segment = lex_expr(str,&n);

	register num32 i;
	for(i = 0; i < n; i++)
	{
		if(!strsim(segment[i],"nl"))
			printf("\n");
		else if(!strsim(segment[i],"newline"))
			printf("\n");
		else if(!strsim(segment[i],"prog"))
			prog_show(stdout);
		else if(!strsim(segment[i],"dump"))
			show_dbl_list(&glob_dbl_list, stdout);
		else if(*segment[i] == '\"') {
			trim(segment[i],'\"');
			printf(segment[i]);
		}
		else if(*segment[i] == '$') {
			show_dbl_str((segment[i]+1), &glob_dbl_list, stdout);
		}
		else {
			double val=0;
			eval_dbl(segment[i],&val);
			printf("%lf",val);
		}
	}
	if(newline)
		printf("\n");
}

void operation_write(char *str)
{
	puts(str);
}

void operation_term()
{
	terminate();
	initialize();
}

void operation_del(char *str)
{
	if(!del_dbl_var(str,&glob_dbl_list)) {
		ai_error = allocs(strlen(str)+STR_MAX);
		sprintf(ai_error,
		"Variable \"%s\" does not exist.",str);
		ai_error = reallocstr(ai_error);
	}
}

void operation_dump()
{
	puts("\nCore dump: ");
	show_dbl_list(&glob_dbl_list, stdout);
}

void operation_dumpf(char *str)
{
	if(!strlen(str))
		strcpy(str,"dump.txt");

	FILE *dump_fp = fopen(str,"w");
	if(dump_fp != NULL)
		show_dbl_list(&glob_dbl_list, dump_fp);

	printf("\nCore dumped in file: %s\n",str);
	fclose(dump_fp);
}

void operation_debug(char *str)
{
	if((!strsim(str,"1"))||
	   (!strsim(str,"on"))||
	   (!strsim(str,"true")))
	{
		debug_mode = 1;
		puts("\nDebug switched on.");
	} else 
	if((!strsim(str,"0"))||
	   (!strsim(str,"off"))||
	   (!strsim(str,"false")))
	{
		debug_mode = 0;
		puts("\nDebug switched off.");
	} else
		printf("Error: Debug mode \'%s\' does not exist.",str);

}

void operation_debugm()
{
	if(debug_mode)
		puts("\nDebug mode is on.");
	else
		puts("\nDebug mode is off.");
}

void operation_pause()
{
	printf("\nPress any key to continue . . . ");
	pause_key();
}

void operation_exec(char *str)
{
	if(debug_mode)
		printf("\nExecuting: %s\n",str);
	printf("\n");
	system(str);
}

void operation_lisp(char *str)
{
	//
}

void operation_scan(char *str)
{
	LIST seg = NULL;
	num32 n = 0;
	seg = lex_expr(str,&n);

	if(n==1) {
		ai_error = allocs(128);
		strcpy(ai_error,"Invalid Syntax, where are the strings?");
		return;
	}

	FILE *file = NULL;
	if(file != NULL)
		fclose(file);
	file = NULL;
	file = fopen(seg[0],"r");

	if(file == NULL) {
		ai_error = allocs(128);
		strcpy(ai_error,"Target file not found.");
		return;
	}

	register num32 i;
	char rtmp[STR_MAX];
	for(i = 1; i < n; i++)
	{
		if(fgets(rtmp,1024,file)==NULL)
			break;
		double *var_ptr = NULL;
		var_ptr = get_dbl(seg[i], &glob_dbl_list);
		if(ai_error)
			if(strlen(ai_error))
				return;
		if(var_ptr == NULL) {
			new_dbl(seg[i], &glob_dbl_list);
			if(ai_error)
				if(strlen(ai_error))
					return;
		}
		string(rtmp);
		mov_dbl_str(seg[i], &glob_dbl_list, rtmp);
	}

	fclose(file);
}


void operation_store(char *str)
{
	if(file_io == NULL) {
		ai_error = allocs(128);
		strcpy(ai_error,"Please specify a target file for writing.");
		return;
	}

	LIST segment = NULL;
	num32 n = 0;
	segment = lex_expr(str,&n);

	register num32 i;
	for(i = 0; i < n; i++)
	{
		if(!strsim(segment[i],"nl"))
			fprintf(file_io, "\n");
		else if(!strsim(segment[i],"newline"))
			fprintf(file_io, "\n");
		else if(!strsim(segment[i],"prog"))
			prog_show(file_io);
		else if(!strsim(segment[i],"dump"))
			show_dbl_list(&glob_dbl_list, file_io);
		else if(*segment[i] == '\"') {
			trim(segment[i],'\"');
			fputs(segment[i],file_io);
		}
		else if(*segment[i] == '$') {
			show_dbl_str((segment[i]+1), &glob_dbl_list, file_io);
		}
		else {
			double val=0;
			eval_dbl(segment[i],&val);
			fprintf(file_io,"%lf",val);
		}
	}
}

void operation_scribble(char *str)
{
	if(file_io == NULL) {
		ai_error = allocs(128);
		strcpy(ai_error,"Please specify a target file for writing.");
		return;
	}

	if(!strsim(str,"nl"))
		fprintf(file_io, "\n");
	else if(!strsim(str,"newline"))
		fprintf(file_io, "\n");
	else if(!strsim(str,"prog"))
		prog_show(file_io);
	else if(!strsim(str,"dump"))
		show_dbl_list(&glob_dbl_list, file_io);
	else if(*str == '\"') {
		trim(str,'\"');
		fputs(str,file_io);
	}
	else {
		fputs(str,file_io);
	}
}

void operation_repeat(char *str, int32 *loop)
{
	double val=0;
	eval_dbl(str,&val);
	*loop = abs((int32)(floor(val)));
}

void operation_progl(char *str)
{
	int line_no = atoi(str);
	const char *line = prog_read(line_no);
	printf("\n");

	if(line != NULL)
		puts(line);
	else
		puts("Line Number not found.");
}

void operation_prog()
{
	prog_show(stdout);
}

void operation_file(char *str)
{
	if(file_io != NULL)
		fclose(file_io);
	file_io = NULL;
	file_io = fopen(str,"w");
}

void operation_release()
{
	if(file_io != NULL)
		fclose(file_io);
	file_io = NULL;
}

void operation_exit()
{
	terminate();
	exit(0);
}

/****************************.
| Lexical Analysis Functions |
`****************************/

LIST lex_expr(const char *str, num32 *args)
{
	/*   Common Lexical Analyzer
	 *  -------------------------
	 *  lex_command() splits a an expression into
	 *  several component strings and allocates
	 *  them into a string array which is returned.
	 *  This expression can be a function or command.
	 */

//	 printf("Lexing: %s\n\n",str);

	if(strlen(str))
		*args = 1;
	else {
		*args = 0;
		return 0;
	}

	/* we should find out if its a 
	   function or command */
	const num32 str_sz = strlen(str);
	num32 strn = 0;

	/* go past the first opening bracket */
	for(;*str!='\0'&&*str!='(';str++,strn++);

	if(*str=='\0')
		/* there is no opening bracket */
		return lex_command(str, args, strn, str_sz);
	else
		/* opening bracket exists */
		return lex_function(str, args, strn, str_sz);
}

LIST lex_command(const char *str, num32 *args, 
				 num32 strn, num32 str_sz)
{
	/*  lex_command() splits a an command into
	 *  several component strings and allocates
	 *  them into a string array which is returned.
	 */

	num32 i = 0;
	char *tmp = allocs(str_sz);
	struct bools bits = { 0, 0, 0, 0, 0, 0, 0, 0 };

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
				trims(list[i]);
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
			trims(tmp);
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

LIST lex_function(const char *str, num32 *args, 
				  num32 strn, num32 str_sz)
{
	/*  lex_function() splits a an function into
	 *  several component strings and allocates
	 *  them into a string array which is returned.
	 */

	num32 i = 0;
	char *tmp = allocs(str_sz);
	struct bools bits = { 0, 0, 0, 0, 0, 0, 0, 0 };
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
	trims(tmp);
	list[i++] = clones(tmp);

	/* retrieve function arguments */
	for(str++,strn=0;*str!='\0';str++)
	{
		if((*str==','||*str==')')&&!bits.x1)
		{
			*tmp = '\0';
			tmp -= strn;
			trims(tmp);

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

void lex_equation(const char *str, char *lhs, char *rhs)
{
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

		trims(lhs);
		trims(rhs);
	}
}

int32 lex_subscript(const char *str, char *var_name)
{
	/* here the subscript of a variable is obtained */

	char *tmp = allocs(strlen(str));
	double val=0;

	if(prechar(var_name,str,"[") != '[')
		goto not_found;
	trims(var_name);

	postchar(tmp,str,"[");
	if(prechar(tmp,tmp,"]") != ']')
		goto not_found;
	trims(tmp);

	if(!strlen(tmp))
		return -1;
	eval_dbl(tmp,&val);
	return abs((int32)(floor(val)));

not_found:
	strcpy(var_name, str);
	return 0;
}

char check_format(const char *str)
{
	// checks the format of a string
	// and returns a number:
	// 0 -> "abc",    ...  string
	// 1 -> 23, 0xA,  ...  number 
	// 2 -> 'x','\0', ...  char
	// 3 ->  abc, i   ...  words
	// 4 -> some other symbol
	// -1 -> illegal char (>128)

	if(*str == '\"')
		return 0;
	else if(*str>='0'&&*str<='9')
		return 1;
	else if(*str == '\'')
		return 2;
	else if(*str>'A'&&*str<'Z')
		return 3;
	else
		return 4;

	return -1;
}

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

void verify_brackets(const char *str)
{
	struct bools bits = { 0, 0, 0, 0, 0, 0, 0, 0 };

	int32 brackets = 0;
	for(;*str!='\0';str++)
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

	if(brackets!=0) {
		ai_error = clones(
		"Invalid Syntax, incorrect number of parentheses.");
		return;
	}
}

/******************.
| Functions System |
`******************/

double call_func(char ** list, num32 args)
{
	if(!list)
		return 0;
	if(!list[0])
		return 0;

	#define FN_CALL(FN) \
	else if(!strsim(list[0],FN)) return

	if(!strlen(list[0]))
			return 0;

	/* interface & language functions router */
	FN_CALL("if")       clib_if(list[1]);
	FN_CALL("not")      clib_not(list[1]);
	FN_CALL("del")      clib_del(list[1]);
	FN_CALL("print")    clib_print(list[1]);
	FN_CALL("input")    clib_input(0,list[1]);
	FN_CALL("inpute")   clib_input(1,list[1]);
	FN_CALL("read")     clib_read(list[1],1);
	FN_CALL("read!")    clib_read(list[1],0);

	/* mathematical functions router */
	FN_CALL("abs")      clib_abs(list[1]);
	FN_CALL("acos")     clib_acos(list[1]);
	FN_CALL("asin")     clib_asin(list[1]);
	FN_CALL("atan")     clib_atan(list[1]);
	FN_CALL("atan2")    clib_atan2(list[1],list[2]);
	FN_CALL("ceil")     clib_ceil(list[1]);
	FN_CALL("cos")      clib_cos(list[1]);
	FN_CALL("cosh")     clib_cosh(list[1]);
	FN_CALL("exp")      clib_exp(list[1]);
	FN_CALL("floor")    clib_floor(list[1]);
	FN_CALL("ldexp")    clib_ldexp(list[1],list[2]);
	FN_CALL("log")      clib_log(list[1]);
	FN_CALL("log10")    clib_log10(list[1]);
	FN_CALL("sin")      clib_sin(list[1]);
	FN_CALL("sinh")     clib_sinh(list[1]);
	FN_CALL("sqrt")     clib_sqrt(list[1]);
	FN_CALL("tan")      clib_tan(list[1]);
	FN_CALL("tanh")     clib_tanh(list[1]);
	FN_CALL("golden")   clib_golden(list[1]);
	FN_CALL("goldens")  clib_goldens(list[1]);
	FN_CALL("decimal")  clib_decimal(list[1]);

	#undef FN_CALL

	else {
		ai_error = allocs(strlen(list[0])+STR_MAX);
		sprintf(ai_error,
		"Function \"%s\" does not exist.",list[0]);
		ai_error = reallocstr(ai_error);
		return 0;
	}
}

void call_proc(char *str)
{
	#define PR_CALL(FN) \
	else if(!strsim(str,FN))

	if(!str)
		goto bad_call;
	else if(!strlen(str))
		goto bad_call;

	else {
		ai_error = allocs(strlen(str)+STR_MAX);
		sprintf(ai_error,
		"Variable \"%s\" does not exist.",str);
		ai_error = reallocstr(ai_error);
		return;
	}

bad_call:
	ai_error = clones("Fatal object processing error.");

	#undef PR_CALL
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
	if(A1 < 0)
		return 1;
	else return 0;
}

double clib_del(char *S1)
{
	if(del_dbl_var(S1, &glob_dbl_list)) {
		ai_error = allocs(strlen(S1)+STR_MAX);
		sprintf(ai_error,"Variable \"%s\" does not exist.",S1);
		ai_error = reallocstr(ai_error);
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

double clib_read(char *S1, byte create)
{
	double *var_ptr = NULL;
	var_ptr = get_dbl(S1, &glob_dbl_list);
	if(ai_error)
		if(strlen(ai_error))
			return 0;
	if(var_ptr == NULL) {
		if(!create) {
			ai_error = allocs(strlen(S1)+STR_MAX);
			sprintf(ai_error,"Variable \"%s\" does not exist.",S1);
			ai_error = reallocstr(ai_error);
			return 0;
		}
		else {
			new_dbl(S1, &glob_dbl_list);
			if(ai_error)
				if(strlen(ai_error))
					return 0;
		}
	}

	char str[STR_MAX];
	fgets(str,STR_MAX,stdin);
	string(str);
	mov_dbl_str(S1, &glob_dbl_list, str);
	return 0;
}

double clib_input(byte evaluative, char *S1)
{
	double *var_ptr = NULL;
	var_ptr = get_dbl(S1, &glob_dbl_list);

	if(ai_error)
		if(strlen(ai_error))
			return 0;
	if(var_ptr == NULL) {
		ai_error = allocs(strlen(S1)+STR_MAX);
		sprintf(ai_error,"Variable \"%s\" does not exist.",S1);
		ai_error = reallocstr(ai_error);
		return 0;
	}

	if(!evaluative)
		printf("\n? ");
	else
		printf("\nE? ");
	char str[STR_MAX];
	fgets(str,STR_MAX,stdin);
	string(str);

	if(!evaluative)
		*var_ptr = atof(str);
	else
		eval_dbl(str, var_ptr);
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

double clib_goldens(char *S1)
{
	puts("");

	double E = 0;
	eval_dbl(S1,&E);
	int n = abs((int32)(floor(E)));

	for(int i=1;i<=n;i++)
	{
		char *str = allocs(STR_MAX);
		sprintf(str,"%d",i);
		printf("%lf\n",clib_golden(str));
		free(str);
	}

	return 0;
}

double clib_golden(char *S1)
{
	double E = 0;
	eval_dbl(S1,&E);

	int n = abs((int32)(floor(E)));

	if(n<=1)
		return 0;
	else
		return ((double)(fibonacci(n+1))
				/(double)(fibonacci(n)));
}

double clib_fibonacci(char *S1)
{
	double A1 = 0;
	eval_dbl(S1,&A1);

	return fibonacci(abs((int32)(floor(A1))));
}

double clib_fibonaccis(char *S1)
{
	puts("");

	double E = 0;
	eval_dbl(S1,&E);
	const int n = abs((int32)(floor(E)));

	for(int i=1;i<=n;i++)
		printf("%ld\n",fibonacci(i));
	return 0;
}

double clib_decimal(char *S1)
{
	// this function translates a double
	// into a string & stores it in *str.
	// it prints the 2 parts seperately.

	double f = 0;
	eval_dbl(S1,&f);

	char *str = allocs(STR_MAX);
	char *sp = str;

	if(f==0)
	{
		*str='\0';
		return f;
	}

	double dreal=f;
	double dint;
	dint=(int)(dreal);
	int cdec=0;

	while(1)
	{
		if(dreal==dint)
		break;
		else
		{
			dreal*=10;
			dint=(int)(dreal);
			cdec+=1;
		}
	}

	/* finds out the position 
	   of the decimal point. */
	int number = (int)(dreal);
	int digits = 0;
	while(number)
	{
		number /= 10;
		digits += 1;
	}
	const int dot = digits - cdec;
	
	number = (int)(dreal);
	int x = 0;
	short int d = 0;

	/* reverse the number into x */
	while(number!=0)
	{
		d = number % 10;
		x = (x*10) + d;
		number = number / 10;
	}

	/* reverse x & store in str, 
	  since x is already reversed,
	  so str has the original number */
	int pos = 0;
	d = 0;
	char *dp = 0;

	while(x!=0)
	{
		d = x % 10;
		x = x / 10;
		*str = d+48;
		str++;

		/* sets the decimal point */
		pos++;
		if(dot==pos) {
		*str = '\0';
		printf("\nIntegral: %s\n", sp);
		*str = '.';
		str++;
		dp = str;
		}
	}
	*str='\0';

	printf("Fractional: %s\n", dp);
	printf("\nDecimal: %s\n", sp);
	return f;
}

/************************.
| Mathematical Functions |
`************************/

int32 concatenate(int32 lhs, int32 rhs)
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

num32 factorial(num32 value)
{
	// finds factorial(!) of an integer
	int32 factorial=1;

	for(;value>0;value--)
		factorial = factorial * value;
	return factorial;
}

int32 fibonacci(int32 n)
{
	if(n<=1)
		return 0;
	else if(n==2)
		return 1;
	else
	{
		n -= 2;
		int32 a=0, b=1;
		for(int i=0; i<n; i++)
		{
			int t = b;
			b += a;
			a = t;
		}
		return b;
	}
}

/*********************.
| String Manupilation |
`*********************/

void trims(char *str)
{
	trim(str,' ');
	trim(str,'\t');
}

void trim(char *str, const char T)
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

void rtrim(char *str, const char T)
{
	/* removes a character(T) from 
	   right side of a string. */

	char *sp = str + strlen(str);

	for( --sp; sp > str ; sp-- )
		if(*sp != T)
			break;

	if(*sp != T)
		*++sp = '\0';
	else
		*sp = '\0';
}

void ltrim(char *str, const char T)
{
	/* removes a character(T) from 
	   left side of a string. */

	char *sp = str;

	while( *sp == T )
		sp++;

	strcpy(str,sp);
}

void strclean(char *str, const char T)
{
	/*  removes a character(T)
	   completely from a string. */

	char *tmp = allocs(strlen(str));
	char *ptr = str;
	char *mov = tmp;

	for(; *ptr ; ptr++ )
		if( *ptr != T )
			*(mov++) = *ptr;
		ptr ++;
	*mov = '\0';

	strcpy(str, tmp);
	free(tmp);
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

int32 strsim(const char *str, const char *cmp)
{
	char *t1 = clones(str);
	char *t2 = clones(cmp);
	int32 diff = 0;

	trims(t1);
	trims(t2);
	strlower(t1);
	strlower(t2);
	diff = strcmp(t1, t2);

	free(t1);
	free(t2);

	return diff;
}

void strip_quotes(char *str)
{
	if(*str == '\'')
		trim(str, '\'');
	if(*str == '\"')
		trim(str, '\"');
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

num32 charcount(const char *str, const char ch)
{
	// counts & returns the number of char ch ins str

	register int n=0;
	for(;*str!='\0';str++)
		if(*str==ch)
			n++;
	return n;
}

char strisc(const char *str, const char chr)
{
	// searches a string for char chr & returns it, else 0.

	while(*str!='\0')
		if(*str++==chr)
			return *--str;
	return 0;
}

void strcatc(char *str, const char chr)
{
	// add a single character chr to the string *str.

	while(*str!='\0')
		str++;

	*str++=chr; 
	*str='\0';
}

void strlower(char *str)
{
	for(; *str != '\0' ; str++ )
		*str = tolower(*str);
}

void strupper(char *str)
{
	for(; *str != '\0' ; str++ )
		*str = toupper(*str);
}

void strbluff(char *str, const char *bluff, const num32 n)
{
	// fills up a string with a another string 'n' times.

	for(register num32 i=0; i<n; i++)
		strcat(str, bluff);
}

void strnully(char *str, const num32 n)
{
	register num32 i = 0;
	for(; i<n; i++ ) 
		*str = '\0';
}

void strchar(char *str, const char ch)
{
	// convert a char to a string.

	*str++ = ch;
	*str = '\0';
}

void strnull(char *str)
{
	*str = '\0';
}

/***************************.
| Dynamic String Allocation |
`***************************/

char * allocs(num32 len)
{
	/* allocates an empty string 
	   of length (num32 len). */

	char *str = (char *) malloc(len+1);
	if(str == NULL)
	  msg(system_no_mem, 4);

	*str = '\0';
	return str;
}

char * reallocstr(char *str)
{
	/* shrink a string's size in memory 
	   according to it's length. */

	char *sp = NULL;
	const num32 len = strlen(str) + 1;
	sp = (char *) realloc(str, len);
	if(sp == NULL)
		msg(system_no_mem, 4);
	return sp;
}

char * clones(const char *str)
{
	/* allocates a copy of string *str & returns it. */

	const num32 len = strlen(str) + 1;
	char *strc = (char *) malloc(len);

	if(strc == NULL)
		msg(system_no_mem, 4);

	strcpy(strc,str);
	return strc;
}

char * twins(const char *str, num32 len)
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

void pause_key(void)
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
	case 2: printf("\nLine %lu Error: %s\n",linenum,str  ); break;
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

/*****************************************/
