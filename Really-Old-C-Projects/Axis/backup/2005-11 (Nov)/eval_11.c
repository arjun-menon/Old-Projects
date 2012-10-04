/*******************************************
  Name: Mathematical Expression Evaluator
  Author: Arjun Menon  |  Date: Nov 2005
*******************************************/

#include "arjun.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/**********************.
| Function Prototypes: |
`**********************/

void msg(const char *str, const unsigned char mode);
/* string-related: */
void trims(char *str, const char T);
char * allocs(unsigned long int len);
void reallocstr(char *str);
char * clones(const char *str);
char * twins(const char *str, unsigned long int len);
/* lexical analysis */
void eval(const char *str, long int *value);
void vals(const char *str, long int *value);
inline unsigned char esnat(const char *str);
/* memory management: */
void free_memory();
long int * newint(const char *id);
long int * getint(const char *id);
/* mathematical functions */
inline long int factorial(long int value);
inline void concatenate(long int *value, long int rvalue);

/************************.
| Command User Interface |
`************************/

int main()
{
	void shell(char *name);
	shell("EVAL");
	return 0;
}

void shell(char *name)
{
	atexit(free_memory);
	PROTOTYPE (void runs,(const char *str));
	char str[KB_SIZE];
	puts(name);
	forever
	{
		printf("\n$ ");
		fflush(stdin);
		fgets(str,KB_SIZE,stdin);
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
	/*
	 *  here str is trimmed of all spaces
	 *  and tabs and stored in string and.
	 */
	linenum += 1;

	char *and = clones(str);
	trims(and,' ');
	trims(and,'\t');
	register long int loop = 0;
	for(;and[loop]!='\0';loop++)
	{
		if(and[loop]==34||and[loop]==39)
		bits.qt += 1;
		else if(and[loop]==';'&&!bits.qt)
		and[loop] = '\0';
	}

	/* now execute "and" */
	PROTOTYPE (void exec, (char *));
	reallocstr(and);
	exec(and);
	free(and);
}

void exec(char *str)
{
	const strn = strlen(str);
	char *lhs = allocs(strn);
	char *rhs = allocs(strn);
	char *sptr = NULL;

	/* here the rhs & lhs are obtained */
	for(sptr=lhs;*str!='\0'&&*str!='=';str++)
	*sptr++ = *str;
	*sptr = '\0';
	if(*str=='=')
	for(sptr=rhs,str++;*str!='\0';str++) {
    *sptr++ = *str;
	*sptr = '\0'; }
	else *rhs = '\0';
	trims(lhs,' ');
//	printf("%s",lhs);
	/*
	 * sptr is no longer used to point at str,
	 * so it is used as an error descrptor string.
	 */
	sptr = (char *) allocs(strn+64);

	long int *value = NULL;
	if(*rhs=='\0') /* rhs is empty */
	{
		printf("%s",lhs);
		/* no assignment involved */
		value = getint(lhs);
		if(value!=NULL)
		printf(" = %ld\n",*value);
//		else {
//		sprintf(sptr,"Variable '%s' does not exist.",lhs);
//		msg(sptr,2); }
		else { long val=0;
		eval(lhs,&val);
		printf(" -> %ld\n",val); }
	}
	else  /* rhs exists */
	{
		trims(rhs,' ');
		long int rvalue;
		/* check for [=>] exchange directive */
		if(*rhs=='>')
		{
			rhs++;
			trims(rhs,' ');
			/* now exchange */
			value = getint(rhs);
//			printf("(%s)",rhs);
			if(value!=NULL)  /* okay, rhs is valid */
			{
				rvalue = *value;
				value = getint(lhs);
				if(lhs!=NULL)
				{
					/* now swap the values */
					* getint(rhs) = * value;
					* value = rvalue;
				}
				else {
				sprintf(sptr,"LHS Variable '%s' does not exist.",lhs);
				msg(sptr,2); }
			}
			else {
			sprintf(sptr,"RHS Variable '%s' does not exist.",rhs);
			msg(sptr,2); }
			/* rhs back to origin for freeing */
			rhs--;
		}
		else
		{
			/* find assignment operator */
			char esymbol=0;
			rvalue=strlen(lhs)-1;
			lhs += rvalue;
			if(*lhs=='*'||*lhs=='/'||
			   *lhs=='+'||*lhs=='-'||
			   *lhs=='%'||*lhs=='^'||
			   *lhs=='~'||*lhs=='@'||
			   *lhs=='|'||*lhs==':'||
			   *lhs=='!'||*lhs=='&')
			{
				esymbol = *lhs;
				*lhs = '\0';
			}
			lhs -= rvalue;

			/* evaluate rhs */
//			printf("[%ld]",rvalue);
			eval(rhs,&rvalue);
//			printf("= %ld\n",rvalue);

			value = newint(lhs);
			/* if variable elready exists, get it. */
			if(value==NULL)
			value = getint(lhs);
			/* set the value */
			if(value!=NULL) {
			if(esymbol) {
				/* perform post-evaluation operations*/
				switch(esymbol) {
				case '*': *value *= rvalue; break;
				case '/': *value /= rvalue; break;
				case '+': *value += rvalue; break;
				case '-': *value -= rvalue; break;
				case '%': *value %= rvalue; break;
				case '~': *value = ~rvalue; break;
				case '@': *value &= rvalue; break;
				case '|': *value |= rvalue; break;
				case ':': *value = labs(rvalue);
				case '!': *value = factorial(rvalue); break;
				case '^': *value = (long int)pow(*value,rvalue); break;
				case '&': concatenate(value,rvalue); break;
				default: *value = rvalue; }

			} else *value  = rvalue;
			} else msg("Fatal Data Transfer Error.",2);
			printf("%s = %ld\n",lhs,*value);
		}
	}
	/* free memeory */
	free(sptr);
	free(lhs);
	free(rhs);
}

void eval(const char *str, long int *value)
{
	// expression evaluation function

	unsigned long int cur;
	unsigned long int stack_size=0;

	for(cur=0;*str!='\0';str++,cur++)
	if(*str=='+'||*str=='-'||
	   *str=='*'||*str=='/')
	   stack_size++;
	str -= cur;

	if(!stack_size) {
	vals(str,value);
	return; }
	/* set the actual stack size */
	stack_size += stack_size + 1;

	unsigned char *ostack = NULL;
	signed long int *estack = NULL;
	unsigned long int stack_pos = 0;
	char *tmp = allocs(strlen(str));
	/* now allocate the stacks */
	ostack = (unsigned char *) malloc
	(stack_size * sizeof(unsigned char));
	estack = (signed long int *) malloc
	(stack_size * sizeof(signed long int));

	for(cur=0,*tmp=0;/*infinite*/;str++)
	{
		if(*str=='+'||*str=='-'||
		   *str=='*'||*str=='/'||
		   *str=='\0')
		{
			ostack[stack_pos] = 0;
			*tmp = '\0';
			tmp -= cur;
			cur = 0;
			vals(tmp,&estack[stack_pos]);
			*tmp = 0;
			ostack[++stack_pos] = *str;
			if(*str!='\0')
			stack_pos += 1;
			else
			break;
		}
		else
		{
			*tmp++ = *str;
			cur++;
		}
	}

	unsigned long int i=0;
	printf("\n---[%ld]---\n",stack_size);
	for(;i<stack_pos;i++)
	if(!ostack[i]) printf("%ld ",estack[i]);
	else printf("%c ",ostack[i]);puts("\n-----");
//	free(ostack);
//	free(estack);
}

void vals(const char *str, long int *value)
{
	register unsigned long int cur=0;
	for(;(*str!='\0'&&*str>'0'&&*str<'9')||*str=='.';str++,cur++);
	if(*str=='\0') {
	  str-=cur;
	  *value = atoi(str);
	} else {
	  str-=cur;
	  *value = (getint(str)==NULL) ? 0:*getint(str);
	}
}

/*******************************.
| Data Storage/Retrieval System |
`*******************************/

struct int_table_struct
{
	char *ids;  /* variable identifier */
	long int var;  /* variable value pointer */
};
struct int_table_struct * int_table = NULL;
static unsigned long int int_table_size = 0;

long int * newint(const char *id)
{
	/* check if the variable already exists */
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
	int_table[int_table_size-1].ids = clones(id);
	int_table[int_table_size-1].var = 0;
	return &int_table[int_table_size-1].var;
}

long int * getint(const char *id)
{
	/* int table is empty */
	if(!int_table_size) return NULL;
	unsigned long int table_pos = 0; /* position count */
	/* find out the location of the variable */
	for(;table_pos<int_table_size;table_pos++)
	if(!strcmp(int_table++ ->ids,id)) break;
	if(table_pos>=int_table_size) {
	/* reverse int_table pointer to origin */
	int_table -= table_pos; return NULL; }
	/* reverse int_table pointer to origin */
	int_table -= table_pos+1;
	return &int_table[table_pos].var;
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
	long int _rvalue;
	_rvalue = rvalue;
	do { _rvalue/= 10;
	*value *= 10; }
	while(_rvalue!=0);
	*value += rvalue;				
}

inline long int factorial(long int value)
{
	value = labs(value);
	long int factorial=1;
	for(;value>0;value--)
	factorial = factorial * value;
	return factorial;
}

/*********************.
| String Manupilation |
`*********************/

void trims(char *str, const char T)
{
	unsigned short int rt,lt;
	for(rt=0;;str++)
	if(*str==T) rt++;
	else break;
	for(lt=0;*str;str++) {
	if(*str==T) lt++;
	else if(*str) lt=0;
	str[-rt]=*str; }
	str[-rt-lt]='\0';
}

char * allocs(unsigned long int len)
{
	char *str = (char *) malloc(len+1);
	if(str==NULL)
	msg(no_mem,3);
	return str;
}

void reallocstr(char *str)
{
	char *cp = NULL;
	const unsigned long int len = strlen(str) + 1;
	str = (char *) realloc(str,len);
	if((cp = (char *)(realloc(str,
	(len))))!=NULL) { str = cp; }
	else msg(no_mem,3);
}

char * clones(const char *str)
{
	const unsigned long int len = strlen(str) + 1;
	char *strc = (char *) malloc(len);
	if(strc==NULL)
	msg(no_mem,3);
	strcpy(strc,str);
	return strc;
}

char * twins(const char *str, unsigned long int len)
{
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
	 *  warning, notices, e.t.c.
	 *  The constant mode defines the
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
	switch(mode)
	{
	case 0:
		printf("\nNotice: %s\n",str);
		break;
	case 1:
		printf("\nWarning: %s\n",str);
		break;
	case 2:
		printf("\nError: %s\n",str);
		break;
	case 3:
		exit(1);
		break;
	case 4:
		exit(2);
		break;
	case 5:
		abort();
		break;
	default:
		return;
	}
}

inline unsigned char esnat(const char *str)
{
	/*
	 *   Eq. string nature (esnat) analyzer
	 *  ------------------------------------
	 *  esnat returns the nature of a string,
	 *  according constituent characters.
	 *  Use these definitions to understand
	 *  the return value.
	 *
	 *  ---------------------------------------
	 *  | Return Value | Meaning of the Value |
	 *  ---------------------------------------
	 *        0        |       Null String
	 *        1        |      Integer String
	 *        2        |       Float String
	 *        4        |      Quoted String
	 *        5        |   Alphanumeric String
	 *        6        |      Complex String
	 */
	unsigned char esnat = 0;

	esnat = (*str>='0'&&*str<='9') ? 1 : esnat;
	esnat = (*str==34||*str==39) ? 4 : esnat;  /* ['] || ["] */
	esnat = ((*str>='a'&&*str<='z')
            ||(*str>='A'&&*str<='Z')) ? 5 : esnat;

	for(;*str!='\0';str++)
	{
		if(*str>='0'&&*str<='9')
		;
		else if(*str=='.')
		;
		else if((*str>='a'&&*str<='z')
               ||(*str>='A'&&*str<='Z'))
		;
		else
		;
	}
	return esnat;
}
