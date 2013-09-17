/*******************************************
  Name: Mathematical Expression Evaluator
  Author: Arjun Menon  |  Date: Nov 2005
*******************************************/

#include "arjun.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char _progname[] = "EVAL";

PROTOTYPE (void trims,(char *str, const char T));
PROTOTYPE (char * clones,(const char *str));
PROTOTYPE (void msg,(const char *str, const num32 line, const uchar type));
PROTOTYPE (void reallocstr, (char *str));

int main()
{
	void shell();
	shell();
	return 0;
}

void shell()
{
	PROTOTYPE (void runs,(const char *str));
	char str[KB_SIZE];
	puts(_progname);
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

////////////////////////
// Program Begins Here
////////////////////////

/* global variables */
char no_mem[] = "Not enough memory.";
struct common_bits
{ /* global booleans */
	unsigned char qt:1;
} bits;

void runs(const char *str)
{
	if(*str==';'||*str==' '||*str=='\t')
	return;
	/*
	 *  here str is trimmed of all spaces
	 *  and tabs and stored in string and.
	 */
	char *and = clones(str);
	trims(and,' ');
	trims(and,'\t');
	register int loop = 0;
	for(;and[loop]!='\0';loop++)
	{
		if(and[loop]==34||and[loop]==39)
		bits.qt += 1;
		else if(and[loop]==';'&&!bits.qt)
		and[loop] = '\0';
	}
	/* now execute and */
	PROTOTYPE (void exec, (char *));
	reallocstr(and);
	exec(and);
	free(and);
}

void exec(char *str)
{
	printf("[%s]",str);
}

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

void reallocstr(char *str)
{
	char *cp = NULL;
	const num32 len = strlen(str) + 1;
	str = (char *) realloc(str,len);
	if((cp = (char *)(realloc(str,
	(len))))!=NULL) { str = cp; }
	else msg(no_mem,0,3);
}

char * clones(const char *str)
{
	const num32 len = strlen(str) + 1;
	char *strc = (char *) malloc(len);
	if(strc==NULL)
	msg(no_mem,0,3);
	strcpy(strc,str);
	return strc;
}

void msg(const char *str, const num32 line, const uchar mode)
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
	 *  2 - Program Error  (exit:1)
	 *  3 - System Error   (exit:2)
	 *  4 - Critical Error (abort)
	 *  ? - [ return ]
	 */
	switch(mode)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		exit(1);
		break;
	case 3:
		exit(2);
		break;
	case 4:
		abort();
		break;
	default:
		return;
	}
}
