/*******************************************
  Name: Mathematical Expression Evaluator
  Author: Arjun Menon  |  Date: 12/09/05
*******************************************/

/* include headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* definitions */
#define ID 32			//  identifier size  (  5th power of 2 )
#define MB 1048576	//  statement size: 1KB  ( 20th power of 2 )
#define KB 1024		//  message size: 1MB    ( 10th power of 1 )

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
inline void stradd(char *str, const char *add);
inline void straddc(char *str, const char add);
inline void charstr(char *str, const char ch);
inline char ncheck(const char *str);
inline char bcheck(const char *str);
inline char bcount(const char *str);
inline void charstr(char *str, const char ch);
char purity_check(const char *str);
char id_check(const char *str);
void allocerr(void);

/* global constants */
const char _eval_version[]="EVAL 0.0.1";

/* global variables */
char *cp = NULL;
int *ip = NULL;
float *fp = NULL;
double *dp = NULL;

/* global structures */
struct num {
	int i;
	double d;
};

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
	void runline(const char *cmd, char *err);
	char err[KB]="";

	/* shell system */
	if((cmd=(char *)(malloc(1)))==NULL) allocerr();
	while(strcmp(cmd,_shell_exit))
	{
		strcpy(err,""); /* empty err string */
		/* allocate 1MB to str for input */
		if((cp=(char *)(realloc(str,sizeof(char)*(MB))))!=NULL)
		{ str = cp; } else { allocerr(); }
		printf("\n%s",_shell_arrow);
		gets(str);
		/* re-allocate cmd for reuse */
		if((cp=(char *)(realloc(cmd,sizeof(char)*(strlen(str)+1))))!=NULL)
		{ cmd = cp; } else { allocerr(); }
		strcpy(cmd,str);
		/* destroy str to free memory space */
		free(str); str = NULL;
		/* truncuate the string cmd */
		if(strtrims(cmd,' '))
		continue;
		if(!strlen(cmd))
		continue;
		/* cmd should not be exit */
		if(strcmp(cmd,_shell_exit))
		/* now evaluate string cmd */
		runline(cmd,err);
		/* check for any errors */
		if(strlen(err))
		printf("\nError: %s\n",err);
	}
	/* while exiting, free used memeory */
	free(cmd); cmd=NULL;
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
	// an extensive trimming function. this function omits
	// the trim char from str and squeezes str to close holes.

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
			for(j=0;j<=i;j++) str--;
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

inline void stradd(char *str, const char *add)
{
	for(;*str!='\0';str++);
	strcpy(str,add);
}

inline void straddc(char *str, const char add)
{
	for(;*str!='\0';str++);
	*str=add;
	*++str='\0';
}

inline void charstr(char *str, const char ch)
{
	*str++ = ch;
	*str = '\0';
}

inline int charcount(const char *str, const char ch)
{
	auto int n=0;
	for(;*str!='\0';str++)
	if(*str==ch) n++;
	return n;
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
		stradd(err,"Character [");
		straddc(err,impurity);
		stradd(err,"] is not allowed.");
		return;
	}
}

void exec(const char *str, char *err)
{
	// this function executes a given instruction.
	// all important functions are called from here.

	void eval(const char *str, char *err, register struct num *eval);
	char asgn[ID]="";  /* assignment variable name container*/
	strcpy(err,"");  /* empty error container string */

	/* data structure to store evaluation results */
	auto struct num dat;
	dat.d=0;
	dat.i=0;

	if(prechar(asgn,str,"=")=='=') /* check for = operator */
	{
		if(strtrims(asgn,' '))
		{  // if assignment string is empty
			strcpy(err,"Assignment variable is not specified.");
			return;
		}
		char isid = id_check(asgn);
		if(!isid)
		{
			// assign ***************************************************
			printf("\nassgin: %s\n",asgn);
			return;
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
				stradd(err,"Variable cannot contain [");
				straddc(err,isid);
				stradd(err,"] character");
			}
			return;
		}
	}
	else  /* there is no assigment involved */
	{
		// if there is no assignment then string is considered
		// to be the default assignment data type for evaluation.

		eval(str,err,&dat);
		if(!strlen(err))
		{
			printf("\nint: %d\ndbl: %f\n",dat.i,dat.d);
			return;
		}
		else
		{
			return;
		}
	}
	return;
}

void eval(const char *str, char *err, register struct num *eval)
{
	//    Mathematical Expression Evaluation Function
	//  -----------------------------------------------
	//   This functions solves mathematical equations.
	//   When a complex equation is passed via *str,
	//   the equation is broken into parts, and the
	//   simplest part is passed on recursively onto
	//   the function itself. This recurvise process is 
	//   repeated until the whole equation has been solved.
	//   Results of the evaluation are stored in struct eval.

	if(!bcheck(str))
	{
		/* this is the core of eval where the calculations are done.
		at this level, the equation does not have any brackets. */

		const char symbols[]="^*/%+-&";
		const char se[]="Invalid Syntax";

		char *tmp = NULL;
		if((tmp=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *tmp='\0';
		/* check wether str has reached the absolute stage */
		if(prechar(tmp,str,symbols)==0)
		{
			printf("\n[simple]");
			if(!ncheck(str))
			{
				printf("\nequation solved!");
				eval ->i = strint(str);
				eval ->d = strdouble(str);
				free(tmp);
				tmp = NULL;
				return;
			}
			else if(!id_check(str))
			{
				printf("\nit's a variable!");
				free(tmp);
				tmp = NULL;
				return;
			}
			else
			{
				strcpy(err,se);
				stradd(err,": ");
				stradd(err,str);
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
		printf("\n[complex]");
		char *pre = NULL; /* string preceding of operator */
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
		printf("\nsym: %c",symbol);
		printf("\npre: %s",pre);
		printf("\npos: %s",pos);
		char *ax = NULL; /* value preceding of operator */
		char *bx = NULL; /* value succeding the operator */
		char *cx = NULL; /* value of ax and bx processed */
		/* now allocate the variables */
		if((ax=(char *)(malloc(sizeof(char)*
		(strlen(pre)+1))))==NULL) allocerr(); *ax='\0';
		if((bx=(char *)(malloc(sizeof(char)*
		(strlen(pos)+1))))==NULL) allocerr(); *bx='\0';
		if((cx=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *cx='\0';
		/* find out ax & bx */
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
				break;
			}
			else
			strcpy(ebx,bx);
		}
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
				break;
			}
			else
			strcpy(eax,ax);
		}
		free(pre); pre = NULL;
		free(pos); pos = NULL;
		/* process ax and bx */
		//
		printf("\n*ax: %s",ax);
		printf("\n*bx: %s",bx);
		printf("\n*cx: %s",cx);
		free(ax); ax = NULL;
		free(bx); bx = NULL;
		/* evaluate using cx */
		//
		free(cx); cx = NULL;
		return;
	}
	else
	{
		if(!bcount(str))
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

inline char ncheck(const char *str)
{
	// checks wether a string consists of real/integer numbers.
	// return 1 when a a non-numerical is found & vice-versa.
	for(;*str!='\0';str++)
	{
		if(*str>='0'&&*str<='9'||*str=='.')
		continue;
		else
		return 1;
	}
	return 0;
}

inline char bcheck(const char *str)
{
	// checks if there are brackets in an equation.
	// return 0 if no brackets & vice-versa.
	for(;*str!='\0';str++)
	if(*str=='('||*str==')')
	return 1;
	return 0;
}

inline char bcount(const char *str)
{
	// check if an equation is correct in no. of brackets.
	// return 0 if good, and 1 if bad.
	int brackets[2]={0,0};
	for(;*str!='\0';str++)
	{
		/* find the no. of brackets */
		if(*str=='(')
		brackets[0]+=1;
		if(*str==')')
		brackets[1]+=1;
	}
	if(brackets[0]==brackets[1])
	return 0; /* no errors */
	else
	return 1; /* yes, errors */
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

void allocerr(void)
{
	exit(1);
}
