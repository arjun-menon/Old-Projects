/*******************************************
  Name: Mathematical Expression Evaluator
  Author: Arjun Menon  |  Date: 20/09/05
*******************************************/

/* include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* global functions	*/
char prechar(char *mov,const char *str, const char *key);
char postchar(char *mov,const char *str, const char *key);
int strrht(char *rht, const char *str, const int n);
int strlft(char *lft, const char *str, const int n);
int strmid(char *mid, const char *str, const int m, const int n);
char strtriml(char *str, const char trim);
char strtrimr(char *str, const char trim);
char strtrims(char *str, const char trim);
char strtrim(char *str, const char trim);
int strint(const char *str);
float strfloat(const char *str);
double strdouble(const char *str);
inline void straddc(char *str, const char ch);
inline void strchar(char *str, const char ch);
char purity_check(const char *str);
char id_check(const char *str);
char num_check(const char *str);
void allocerr(void);

/* global constants */
const int max_str_length = 1048576; // 1MB
const char _eval_version[] = "EVAL 0.0.2";

/* global variables */
char *cp = NULL;
int *ip = NULL;
float *fp = NULL;
double *dp = NULL;

////////////////////////
// Program Starts Here
////////////////////////

int main(int argc, char *argv[])
{
	printf("------------------------------------------\n");
	printf(" EVAL - Mathematical Expression Evaluator \n");
	printf("==========================================\n");
	printf("\n%s\n",_eval_version);
	void shell(void);
	shell();
	printf("\nGoodbye!\n");
	return 0;
}

void shell(void)
{
	/* shell constants */
	const char _shell_arrow[] =">>";
	const char _shell_exit[]="exit";

	/* shell variables */
	char *cmd = NULL;
	char *str = NULL;
	char *err = NULL;
	void runline(const char *cmd, char *err);

	/* shell system */
	if((cmd=(char *)(malloc(1)))==NULL) allocerr();
	if((err=(char *)(malloc(1)))==NULL) allocerr();
	while(strcmp(cmd,_shell_exit))
	{
		strcpy(err,""); /* empty err string */
		/* allocate (max_str_length) bytes to str for input */
		if((cp=(char *)(realloc(str,sizeof(char)*(max_str_length))))!=NULL)
		{ str = cp; } else { allocerr(); }
		printf("\n%s",_shell_arrow);
		gets(str);
		/* re-allocate cmd for reuse */
		if((cp=(char *)(realloc(cmd,sizeof(char)*(strlen(str)+1))))!=NULL)
		{ cmd = cp; } else { allocerr(); }
		strcpy(cmd,str);
		/* destroy str to free (max_str_length) bytes of memory space */
		free(str); str = NULL;
		/* truncuate the string cmd */
		if(strtrims(cmd,' ')) continue;
		/* if cmd is empty then continue */
		if(!strlen(cmd)) continue;
		/* allocate variable to store error information */
		if((cp=(char *)(realloc(err,sizeof(char)*(strlen(cmd)+256))))!=NULL)
		{ err = cp; } else { allocerr(); }
		/* cmd should not be exit */
		if(strcmp(cmd,_shell_exit))
		/* now run line string cmd */
		runline(cmd,err);
		/* check for any errors */
		if(strlen(err))
		printf("\nError: %s\n",err);
	}
	/* while exiting, free used memeory */
	free(cmd); cmd = NULL;
	free(err); err = NULL;
}

///////////////////////////////////////////
// Extended String Manupilation Functions
///////////////////////////////////////////

char prechar(char *mov, const char *str, const char *key)
{
	// function prechar takes string preceeding key and stores it in *mov
	// key can be a string if multiple comparisns are to be made.
	// the first key to satisfy comparisn is used and returned as char.

	if(*str=='\0')  /* if str is empty return 0 */
	return 0;
	if(*key=='\0')  /* if key is empty return 0 */
	return 0;
	int klen = strlen(key);
	int slen = strlen(str);
	int u=0, i=0, j=0, k=0;  /* loop counters */
	for(u=0;u<klen;u++)  /* to take each char from key for comparisn */
	{
		if(*key=='\0')  /* exit loop if all keys are over */
		break;
		for(i=0;i<slen;i++)  /* to take each char from str for comparisn */
		{
			if(*str=='\0')  /* check if it is the end of str */
			break;
			if(*str==*key)  /* compare *key and *str */
			{
				for(k=0;k<i;k++)  /* reverse str pointer to start  */
				str--;
				for(k=0;k<i;k++)  /* to take char from str for storage */
				{
					*mov=*str;  /* here the new string is made */
					mov++;
					str++;
				}
				*mov='\0';  /* set termination point for string */
				return *key;
			}
			str++;  /* move pointer to next char in str */
		}
		for(j=0;j<i;j++)  /* reverse str pointer to start  */
		str--;
		key++;  /* move pointer to next char in key */
	}
	return 0;
}

char postchar(char *mov,const char *str, const char *key)
{
	// function prechar takes string after key and stores it in *mov
	// key can be a string if multiple comparisns are to be made.
	// the first key to satisfy comparisn is used and returned as char.

	if(*str=='\0')  /* if str is empty return 0 */
	return 0;
	if(*key=='\0')  /* if key is empty return 0 */
	return 0;
	int klen = strlen(key);
	int slen = strlen(str);
	int u=0, i=0, j=0, k=0;  /* loop counters */
	for(u=0;u<klen;u++)  /* to take each char from key for comparisn */
	{
		if(*key=='\0')  /* exit loop if all keys are over */
		break;
		for(i=0;i<slen;i++)  /* to take each char from str for comparisn */
		{
			if(*str=='\0')  /* check if it is the end of str */
			break;
			if(*str==*key)  /* compare *key and *str */
			{
				str++;  /* increment pointer by one block to avoid *key */
				for(k=0;k<(slen-i);k++)  /* to take char from str for storage */
				{
					*mov=*str;  /* here the new string is made */
					mov++;
					str++;
				}
				*mov='\0';  /* set termination point for string */
				return *key;
			}
			str++;  /* move pointer to next char in str */
		}
		for(j=0;j<i;j++)  /* reverse str pointer to start  */
		str--;
		key++;  /* move pointer to next char in key */
	}
	return 0;
}

int strrht(char *rht, const char *str, const int n)
{
	// function strrht takes chosen chars from *str and store it in *rht
	// it start from the right and stores n letters in reverse order.
	// if n exceeds the string length, return will be no.of exceed chars.

	int len = strlen(str);
	int i;  /* loop counter */
	for(i=1;i<=(len-n);i++)
	str++;  /* to move pointer to n blocks before null */
	for(i=1;i<=n;i++)
	{
		if(*str=='\0')  /* in case of premature string termination */
		{ 
			*rht='\0';
			return ((n-i)+1);  /* calculates how bad the error was */
		}
		*rht=*str;  /* here the new string is made */
		rht++;
		str++;
	}
	*rht='\0';  /* set termination point for string */
	return 0;
}

int strlft(char *lft, const char *str, const int n)
{
	// function strlft takes chosen chars from *str and store it in *lft
	// pointer starts from the left and stores n letters into *str
	// if n exceeds the string length, return will be no.of exceed chars.

	int i;  /* loop counter */
	for(i=1;i<=n;i++)
	{
		if(*str=='\0')  /* in case of premature string termination */
		{
			*lft='\0';
			return ((n-i)+1);  /* calculates how bad the error was */
		}
		*lft=*str;  /* here the new string is made */
		lft++;
		str++;
	}
	*lft='\0';  /* set termination point for string */
	return 0;
}

int strmid(char *mid, const char *str, const int m, const int n)
{
	// function strmid takes chosen chars from *str and store it in *rht
	// first, the pointer moves forward by m no. of chars.
	// then, it start from the left and stores n letters into *str
	// if n or m exceeds the string length, return will be no.of exceed chars.

	int i;  /* loop counter */
	for(i=1;i<=m;i++)
	{
		if(*str=='\0')  /* in case of premature string termination */
		{
			*mid='\0';
			return (((n+m)-i)+1);  /* calculates how bad the error was */
		}
		str++;  /* to move pointer to m blocks forward */
	}
	for(i=1;i<=n;i++)
	{
		if(*str=='\0')  /* in case of premature string termination */
		{
			*mid='\0';
			return ((n-i)+1);  /* calculates how bad the error was */
		}
		*mid=*str;  /* here the new string is made */
		mid++;
		str++;
	}
	*mid='\0';  /* set termination point for string */
	return 0;
}

char strtriml(char *str, const char trim)
{
	// function strtriml removes char trim selectively from str
	// starting from left, it removes trim until a non-trim char.
	// if modified str is '\0', then return is 1, otherwise is 0

	if(trim=='\0')  /* if trim is null then return */
	return 1;
	int len = strlen(str);
	int w = 0;  /* to store no.of trim chars */
	int l = 0;  /* to store real length */
	char wfound=0;  /* boolean to verify if w is found */
	for(;*str!='\0';str++)
	{
		if(*str!=trim)
		wfound=1;  /* now w is found */
		if(!wfound)
		if(*str==trim)
		w++;  /* find out the no. of trim chars. */
	}
	if(w==len) /* check is str is full of trims */
	return 1;
	l=(len - w);  /* find out real length */
	int i = 0;  /* loop counter */
	for(i=0;i<len;i++)
	str--;  /* move str pointer to the starting*/
	char ch; /* temporary character storage variable */
	int u = 0;  /* loop counter */
	for(i=0;i<l;i++)
	{
		for(u=0;u<w;u++)
		str++;  /* moves pointer w block forward */
		ch = *str;  /* stores (w+u)th block of str into ch */
		for(u=0;u<w;u++)
		str--;  /* moves pointer w block backwards */
		*str = ch;  /* stores ch into (u)th block of str */
		str++; /* increments str pointer to the next block */
	}
	*str='\0';  /* set termination point for string */
	return 0;
}

char strtrimr(char *str, const char trim)
{
	// function strtrimr removes char trim selectively from str
	// starting from right, it removes trim until a non-trim char.
	// if modified str is '\0', then return is 1, otherwise is 0

	if(trim=='\0')  /* if trim is null then return */
	return 1;
	int len = strlen(str);
	int i = 0; /* loop counter */
	for(i=0;i<len;i++)
    str++; /* push str poiner to the right end */
	char wfound=0;
	for(i=0;i<len;i++)
	{
		str--;  /* move pointer backward by one block */
		if(*str!=trim)
		wfound=1;  /* reached end of trim chars */
		if(wfound)  /* now reached the end of actual string*/
		{
			str++;  /* move pointer forward by one block */
			*str='\0';    /* set termination point for string */
			return 0;
		}
	}
	if(i==(len-1)) /* check is str is full of trims */
	return 1;
	return 0;
}

char strtrims(char *str, const char trim)
{
	// just a combination of strtriml and strtrimr

	if(strtriml(str,trim))  /* if return is 1 */
	{
		strtrimr(str,trim);
		return 1;
	}
	if(!strtrimr(str,trim))  /* if return is 0 */
	return 0;
	else  /* if return is 1 */
	return 1;
}

char strtrim(char *str, const char trim)
{
	// an extensive trimming function. this function omits all
	// the trim chars from str and squeezes str to close holes.

	char ch; /* temporary character storage variable */
	int i, j;  /* loop counters */
	for(;*str!='\0';str++)
	{
		if(*str==trim)
		{
			for(i=0;*str!='\0';i++,str++)
			{
				str++;
				ch = *str;
				str--;
				*str=ch;
			}
			for(j=0;j<=i;j++)
            str--;
		}
	}
	*str='\0';  /* set termination point for string */
	if(strlen(str)==0)
	return 1;
	else
	return 0;
}

int strint(const char *str)
{
	// this function extracts an integer from a string.
	// note: 48 is '0' and '57' is 9 in ASCII.

	int n = 0;  /* to carry return value */
	short i = 0;  /* to contain a digit */
	for(;*str!='\0';str++)
	{
		if((*str>=48)&&(*str<=57))
		{
			i = *str - 48;  /* translate char to number */
			n = n * 10 + i;  /* mathematical formula */
		}
	}
	return n;
}

float strfloat(const char *str)
{
	// this function extracts a float from a string.
	// note: 48 is '0', '57' is 9 and 46 is '.' in ASCII.

	short int i = 0;  /* to contain a digit */
	long int c = 0; /* loop counter */
	float n = 0, ix = 0;  /* to carry return value & subzero value */
	short int z=0, zp=0;  /* flag & count for decimal point */
	for(;*str!='\0';str++)
	{
		if((*str>=48)&&(*str<=57))
		{
			i = *str - 48;  /* translate char to number */
			if(!z)  /* has not reached decimal point */
			n = n * 10 + i;  /* formula(A) */
			if(z)  /* past decimal point */
			{
				zp++;  /* count no. of digit after point */
				ix = i; /* transfer to float for division */
				for(c=0;c<zp;c++)
				ix /= 10;  /* find subzero value */
				n += ix;  /* add subzero value to the float n */
			}
		}
		else if(*str==46)  /* when a decimal point occurs */
		z=1;
	}
	return n;
}

double strdouble(const char *str)
{
	// this function extracts a double from a string.
	// note: 48 is '0', '57' is 9 and 46 is '.' in ASCII.

	short int i = 0;  /* to contain a digit */
	long int c = 0; /* loop counter */
	double n = 0, ix = 0;  /* to carry return value & subzero value */
	short int z=0, zp=0;  /* flag & count for decimal point */
	for(;*str!='\0';str++)
	{
		if((*str>=48)&&(*str<=57))
		{
			i = *str - 48;  /* translate char to number */
			if(!z)  /* has not reached decimal point */
			n = n * 10 + i;  /* formula(A) */
			if(z)  /* past decimal point */
			{
				zp++;  /* count no. of digit after point */
				ix = i; /* transfer to float for division */
				for(c=0;c<zp;c++)
				ix /= 10;  /* find subzero value */
				n += ix;  /* add subzero value to the float n */
			}
		}
		else if(*str==46)  /* when a decimal point occurs */
		z=1;
	}
	return n;
}

inline void straddc(char *str, const char ch)
{
	str+=strlen(str);
	*str++=ch;*str='\0';
}

inline void strchar(char *str, const char ch)
{
	*str++ = ch;
	*str = '\0';
}

/////////////////////////////////////////
// Core Expression Evaluation Functions
/////////////////////////////////////////

void runline(const char *cmd, char *err)
{
	// this function takes an instruction for execution.
	// it removes all comments and passes the statement to exec

	void exec(const char *str, char *err);
	char impurity = purity_check(cmd);
	if(*cmd==';') /* if statement is a comment. */
	return;
	if(!impurity)  /* if cmd is pure */
	{
		strcpy(err,"");
		char *statement = NULL;
		if((statement=(char *)(malloc(sizeof(char)*
		(strlen(cmd)+1))))==NULL) allocerr(); *statement='\0';
		char *spointer = statement;
		int l = 0;  /* to store length of spointer */
		l = strlen(cmd);
		int i = 0;  /* loop counter */
		char qt=0;
		for(i=0;i<l;i++,cmd++,spointer++)
		{
			if(*cmd==34||*cmd==39)  /* char["] and char['] */
			{
				if(qt==0)
				qt = 1;  /* quotation mark starts */
				else
				qt = 0;  /* quotation mark ends */
			}
			if(qt==0)
			if(*cmd==';')  /* remove comments from the statement */
			break;
			*spointer = *cmd;  /* transfer cmd to str */
		}
		*spointer='\0';  /* set termination point for string */
		strtrims(statement,' ');
		/* now run the statement */
		exec(statement,err);
		if(!strlen(err))    /* if eval was succesfull. */
		{
			free(statement);
			statement = NULL;
			return;
		}
		else    /* if eval was un-succesfull. */
		{
			free(statement);
			statement = NULL;
			return;
		}
	}
	else  /* if cmd is impure */
	{
		strcpy(err,"Character [");
		straddc(err,impurity);
		strcat(err,"] is not allowed.");
		return;
	}
}

void exec(const char *str, char *err)
{
	// this function executes a given instruction.
	// all important functions are called from here.

	void evali(const char *str, char *err, int *eval);
	strcpy(err,"");  /* empty error container string */
	int integer=0;  /* integer to store integer evaluation results */
//	double real=0;  /* double to store double evaluation results */
	char *asgn = NULL;  /* assignment variable name container*/
	if((asgn=(char *)(malloc(sizeof(char)*
	(strlen(str)+1))))==NULL) allocerr(); *asgn='\0';

	if(prechar(asgn,str,"=")=='=') /* check for = operator */
	{
		/* is a variable assignment */
		if(strtrims(asgn,' '))
		{  // if assignment string is empty
			strcpy(err,"Assignment variable is not specified.");
			return;
		}
		char isid = id_check(asgn);
		if(!isid)
		{
			char *estr = NULL;
			if((estr=(char *)(malloc(sizeof(char)*
			(strlen(str)+1))))==NULL) allocerr(); *estr='\0';
			postchar(estr,str,"=");
			evali(estr,err,&integer);
			if(!strlen(err))
			{
				printf("\n%s = %d\n",asgn,integer);
				return;
			}
			else  /* there was an error */
			{
				return;
			}
		}
		else  /* is not an identifier */
		{
			if(isid==1)  /* var name starts with a number */
			{
				strcpy(err,"Variable name cannot start with a number.");
				return;
			}
			else  /* var name has illegal chars*/
			{
				strcpy(err,"Variable cannot contain [");
				straddc(err,isid);
				strcat(err,"] character");
			}
			return;
		}
	}
	else  /* there is no assigment involved */
	{
		// if there is no assignment then integer is considered
		// to be the default assignment data type for evaluation.
		evali(str,err,&integer);
		if(!strlen(err))
		{
			printf("\n%d\n",integer);
			return;
		}
		else  /* there was an error */
		{
			return;
		}
	}
	return;
}

void evali(const char *str, char *err, int *eval)
{
	//  Mathematical Expression Evaluation Function (INT)
	// ---------------------------------------------------
	//  This functions solves mathematical expressions
	//	based on integer numbers. When a complex expression
	//	is passed via *str, the expression is broken 
	//	into parts, and the simplest part in accordance
	//	with the BODMAS theorum is passed on recursively 
	//	onto the function itself. This recurvise process is 
	//  repeated until the whole expression has been solved.
	//  Results of the evaluation are stored in int *eval.

	char * valstr(const char *str);
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
		if((tmp=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *tmp='\0';
		/* check wether str has reached the absolute stage */
		if(prechar(tmp,str,symbols)==0)
		{
			 /* char to store num_check results */
			char ncheck = 0;
			ncheck = num_check(str);
			if(!ncheck)
			{
				/* str is a number (real or integer) */
				*eval = strint(str);
				free(tmp);
				tmp = NULL;
				return;
			}
			else if(!id_check(str))
			{
				free(tmp);
				tmp = NULL;
				char *vals = NULL;
				vals = valstr(str);
				*eval = strint(vals);
				free(vals);
				vals = NULL;
				return;
			}
			else
			{
				/* not a variable, number nor function; hence an error */
				strcpy(err,se);
				strcat(err,": ");
				strcat(err,str);
				free(tmp);
				tmp = NULL;
				return;
			}
		}
		else  /* there are symbols in str */
		{
			free(tmp);
			tmp = NULL;
		}

		/* now the real maths */
		char *pre = NULL; /* string preceding the operator */
		char *pos = NULL; /* string succeding the operator */
		/* now allocate the variables */
		if((pre=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *pre='\0';
		if((pos=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *pos='\0';
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
		straddc(err,symbol);
		return; }
		char *ax = NULL; /* value preceding of operator */
		char *bx = NULL; /* value succeding the operator */
		char *cx = NULL; /* value of ax and bx processed */
		/* now allocate ax and bx */
		if((ax=(char *)(malloc(sizeof(char)*
		(strlen(pre)+1))))==NULL) allocerr(); *ax='\0';
		if((bx=(char *)(malloc(sizeof(char)*
		(strlen(pos)+1))))==NULL) allocerr(); *bx='\0';
		/* find out the contents of bx */
		char *ebx = NULL; /* temp string to build bx */
		if((ebx=(char *)(malloc(sizeof(char)*
		(strlen(pos)+1))))==NULL) allocerr(); *ebx='\0';
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
		if((eax=(char *)(malloc(sizeof(char)*
		(strlen(pre)+1))))==NULL) allocerr(); *eax='\0';
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
		if((prex=(char *)(malloc(sizeof(char)*
		(strlen(pre)+1))))==NULL) allocerr(); *prex='\0';
		if((posx=(char *)(malloc(sizeof(char)*
		(strlen(pos)+1))))==NULL) allocerr(); *posx='\0';
		/* find prex and posx */
		strlft(prex,pre,(strlen(pre)-strlen(ax)));
		strrht(posx,pos,(strlen(pos)-strlen(bx)));
		/* de-allocate pre & pos */
		free(pre); pre = NULL;
		free(pos); pos = NULL;
		/* process ax and bx to find cx */
		printf("\nax=%s",ax);
		printf("\nbx=%s",bx);
		int evala=0;
		int evalb=0;
		int evalc=0;
		if(num_check(ax))
		{
			tmp = valstr(ax);
			if((cp=(char *)(realloc(ax,sizeof(char)*
			(strlen(tmp)+1))))!=NULL) { ax = cp; }
			else { allocerr(); }
			strcpy(ax,tmp);
			free(tmp);
			tmp = NULL;
		}
		if(num_check(bx))
		{
			tmp = valstr(bx);
			if((cp=(char *)(realloc(bx,sizeof(char)*
			(strlen(tmp)+1))))!=NULL) { bx = cp; }
			else { allocerr(); }
			strcpy(bx,tmp);
			free(tmp);
			tmp = NULL;
		}
		evala = strint(ax);
		evalb = strint(bx);
		switch(symbol)
		{
		case '^':	/* power */
			evalc = (int)(pow(evala,evalb));
			break;
		case '*':	/* multiplication */
			evalc = evala * evalb;
			break;
		case '/':	/* division */
			evalc = evala / evalb;
			break;
		case '%':	/* modulus */
			evalc = evala % evalb;
			break;
		case '+':	/* addition */
			evalc = evala + evalb;
			break;
		case '-':	/* subtraction */
			evalc = evala - evalb;
			break;
		case '&':	/* concatenation */
			if((cp=(char *)(realloc(cx,sizeof(char)*
			(strlen(ax)+strlen(bx)+1))))!=NULL)
			{ cx = cp; } else { allocerr(); }
			strcpy(cx,ax);
			strcat(cx,bx);
			evalc = strint(cx);
			break;
		default:
			strcpy(err,ue);
			strcat(err,": ");
			straddc(err,symbol);
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
		if((cx=(char *)(malloc(sizeof(char)*
		((cxlen)+1))))==NULL) allocerr(); *cx='\0';
		itoa(evalc,cx,10);
		printf("\ncx=%s",cx);
		/* de-allocate ax & bx */
		free(ax); ax = NULL;
		free(bx); bx = NULL;
		/* variable to store one-step solved equation */
		char *ex = NULL;
		if((ex=(char *)(malloc(sizeof(char)*
		(strlen(prex)+strlen(cx)+strlen(posx)+1))))==NULL)
		allocerr(); *ex='\0';
		/* find ex using cx in prex and posx */
		strcpy(ex,prex);
		strcat(ex,cx);
		strcat(ex,posx);
		/* now de-allocate cx, prex & posx */
		free(cx); cx = NULL;
		free(prex); prex = NULL;
		free(posx); posx = NULL;
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
		int brackets[2]={0,0};
		for(i=0;*str!='\0';str++,i++)
		{
			/* find the no. of brackets */
			if(*str=='(')
			brackets[0]+=1;
			if(*str==')')
			brackets[1]+=1;
		}
		for(;i>0;i--)
		str--;  /* reverse str pointer to original position */

		if(brackets[0]==brackets[1])
		{
			printf("\nEquation has brackets.");
			return;
		}
		else
		{
			strcpy(err,"Illegal Equation, inequal number of brackets.");
			return;
		}
	}
}

char purity_check(const char *str)
{
	// checks the purity of a statement.
	// allowed characters in a statement are from 32 to 162

	char qt=0;
	for(;*str!='\0';str++)
	{
		if(!qt)
		{
			if(!(*str>=32&&*str<=126))
			return *str;
			if(*str==34||*str==39)
			qt=1;
		}
		else
		{
			if(*str==34||*str==39)
			qt=0;
		}
	}
	return 0;
}

char id_check(const char *str)
{
	// checks the purity of an identifier
	// an identifier should contain only a-z,A-Z,0-9 and '_'.

	if(*str>='0'&&*str<='9')
	return 1;  /* if id starts with a number return 1 */
	for(;*str!='\0';str++)
	{
		if(*str>='0'&&*str<='9')
		continue;
		else if(*str>='A'&&*str<='Z')
		continue;
		else if(*str>='a'&&*str<='z')
		continue;
		else if(*str=='_')
		continue;
		else
		return *str;
	}
	return 0;
}

char num_check(const char *str)
{
	// checks wether a string consists of real/integer numbers.
	// return 1 when a a non-numerical is found & vice-versa.
	for(;*str!='\0';str++)
	{
		if((*str>='0'&&*str<='9')||*str=='.')
		continue;
		else
		return 1;
	}
	return 0;
}

char * valstr(const char *str)
{
	const char nox[] = "Nexus1";
	char *vals = NULL;
	if((vals=(char *)(malloc(sizeof(char)*
	(strlen(nox)+1))))==NULL) allocerr(); *vals='\0';
	// NOT COMPLETED ----------------------------
	strcpy(vals,nox);
	return vals;
}

void allocerr(void)
{
	/* invoked when malloc or realloc fails */
	printf("\nNot enough memory.\a\n");
	exit(1);
}
