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

void msg(const char *str, const uchar mode);
/* string-related: */
void trims(char *str, const char T);
void * allocs(num32 len);
void reallocstr(char *str);
char * clones(const char *str);
char * twins(const char *str, num32 len);
/* memory management: */
void free_memory();
int32 * newint(const char *id);
int32 * getint(const char *id);
inline uchar esnat(const char *str);

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

/***************************.
| Core Language Interpreter |
`***************************/

/* global variables */
num32 linenum = 0;
struct common_bits
{ /* global booleans */
	uchar qt:1;
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
	register int32 loop = 0;
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
	char *lhs = (char *) allocs(strlen(str));
	char *rhs = (char *) allocs(strlen(str));
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
	printf("%s",lhs);
	/*
	 * sptr is no longer used to point at str,
	 * so it is used as an error descrptor string.
	 */
	sptr = (char *) allocs(256);

	int32 *value = NULL;
	if(*rhs=='\0') /* rhs is empty */
	{
		/* no assignment involved */
		value = getint(lhs);
		if(value!=NULL)
		printf(" = %ld\n",*value);
		else {
		sprintf(sptr,"Variable '%s' does not exist.",lhs);
		msg(sptr,2); }
	}
	else  /* rhs exists */
	{
		trims(rhs,' ');
		int32 rvalue;
		/* check for [=>] exchange directive */
		if(*rhs=='>')
		{
			rhs++;
			trims(rhs,' ');
			/* now exchange */
			value = getint(rhs);
			printf("(%s)",rhs);
			if(value!=NULL)  /* okay, rhs is valid */
			{
				rvalue = *value;
				value = getint(lhs);
				if(lhs!=NULL)
				{
					/* now, exchange */
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
			rvalue = (int32)esnat(rhs);
			printf("[%ld]",rvalue);
			rvalue = atoi(rhs);
			printf(" = %ld\n",rvalue);

			value = newint(lhs);
			/* if variable elready exists */
			if(value==NULL)
			value = getint(lhs);
			if(value!=NULL)
			*value = rvalue;
			else
			msg("",2);
		}
	}
	/* free memeory */
	free(sptr);
	free(lhs);
	free(rhs);
}

/*******************************.
| Data Storage/Retrieval System |
`*******************************/

struct int_table_struct
{
	char *ids;  /* variable identifier */
	int32 var;  /* variable value pointer */
};
struct int_table_struct * int_table = NULL;
static num32 int_table_size = 0;

int32 * newint(const char *id)
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

int32 * getint(const char *id)
{
	/* int table is empty */
	if(!int_table_size) return NULL;
	num32 table_pos = 0; /* position count */
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

/*********************.
| String Manupilation |
`*********************/

void trims(char *str, const char T)
{
	num16 rt,lt;
	for(rt=0;;str++)
	if(*str==T) rt++;
	else break;
	for(lt=0;*str;str++) {
	if(*str==T) lt++;
	else if(*str) lt=0;
	str[-rt]=*str; }
	str[-rt-lt]='\0';
}

void * allocs(num32 len)
{
	char *str = (char *) malloc(len+1);
	if(str==NULL)
	msg(no_mem,3);
	return str;
}

void reallocstr(char *str)
{
	char *cp = NULL;
	const num32 len = strlen(str) + 1;
	str = (char *) realloc(str,len);
	if((cp = (char *)(realloc(str,
	(len))))!=NULL) { str = cp; }
	else msg(no_mem,3);
}

char * clones(const char *str)
{
	const num32 len = strlen(str) + 1;
	char *strc = (char *) malloc(len);
	if(strc==NULL)
	msg(no_mem,3);
	strcpy(strc,str);
	return strc;
}

char * twins(const char *str, num32 len)
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

void msg(const char *str, const uchar mode)
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

inline uchar esnat(const char *str)
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
	uchar esnat = 0;

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
