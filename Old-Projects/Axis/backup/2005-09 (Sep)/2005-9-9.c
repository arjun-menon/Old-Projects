/*******************************************
  Name: Mathematical Expression Evaluator
  Author: Arjun Menon  |  Date: 9/09/05
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
void allocerr(void);
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
void intstr(char *str, const int n);
void floatstr(char *str, const float f);
void doublestr(char *str, const double d);
inline char ncheck(const char *str);
inline char bcheck(const char *str);
inline char bcount(const char *str);
char purity_check(const char *str);
char id_check(const char *str);

/* global constants */
const char _eval_version[]="EVAL 0.0.1";

/* global variables */
char *cp = NULL;
int *ip = NULL;
float *fp = NULL;
double *dp = NULL;

/* global structures */
struct data {
	int i;
	float f;
	double d;
	char *s;
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
	char runline(const char *cmd, char *err);
	char err[KB];

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
		if(strlen(cmd)==0)
		continue;
		/* cmd should not be exit */
		if(strcmp(cmd,_shell_exit))
		/* check if eval return is 0 */
		if(runline(cmd,err))
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
	// an extensive trimming function. this function...
	// omits the trim char from str and squeezes str to close holes.

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
	// this function extracts a float from a string.
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
	str++;
	*str='\0';
}

inline void charstr(char *str, const char ch)
{
	*str++ = ch;
	*str = '\0';
}

void intstr(char *str, const int n)
{
	if(n==0)
	{
		*str='\0';
		return;
	}
	int num = n;
	int x = 0;
	short int d = 0;
	while(num!=0)
	{
		d = num % 10;
		x = (x*10) + d;
		num = num / 10;
	}
	d = 0;
	while(x!=0)
	{
		d = x % 10;
		x = x / 10;
		*str = d+48;
		str++;
	}
	*str='\0';
}

void floatstr(char *str, const float f)
{
/*	if(f==0)
	{
		*str='\0';
		return;
	}
	float freal=f;
	float fint=(int)(freal);
	int cdec=0;
	for(;;)  // infinite loop
	{
		if(freal==fint)
		break;
		else
		{
			freal*=10;
			fint=(int)(freal);
			cdec+=1;
		}
	}
	printf("no. of digits after the point=%d",cdec);
	*str='\0';
*/
}

void doublestr(char *str, const double d)
{}

/////////////////////////////////////////
// Core Expression Evaluation Functions
/////////////////////////////////////////

char runline(const char *cmd, char *err)
{
	printf("\nlen=%d",strlen(cmd));
	// this function takes an instruction for execution.
	// it removes all comments and passes the statement to exec

	char exec(const char *str, char *err);
	char impurity = purity_check(cmd);
	if(*cmd==';') /* if statemnt is a comment. */
	return 0;
	if(!impurity)  /* if cmd is pure */
	{
		char error[KB];  /* error information string */
		char *statement = NULL;
		if((statement=(char *)(malloc(sizeof(char)*
		(strlen(cmd)+1))))==NULL) allocerr(); *statement='\0';
		char *spointer = statement;
		int l = 0;  /* to store length of spointer */
		l = strlen(cmd);
		int i = 0;  /* loop counter */
		char qt=0;
		for(i=0;i<l;i++)
		{
			if(*cmd==34||*cmd==39)
			{
				if(qt==0)
				qt = 1;
				else
				qt = 0;
			}
			if(qt==0)
			if(*cmd==';')  // remove comments from the statement
			break;
			*spointer = *cmd;
			cmd++;
			spointer++;
		}
		*spointer='\0';
		strtrims(statement,' ');
		// now run the statement
		if(!exec(statement,error))  /* if eval was succesfull. */
		{ free(statement);
		  return 0; }
		else  /* if eval was un-succesfull. */
		strcpy(err,error);  /* transfer eval's err to err */
		free(statement);
		return 1;
	}
	else  /* if cmd is impure */
	{
		stradd(err,"Character [");
		straddc(err,impurity);
		stradd(err,"] is not allowed.");
		return 1;
	}
}

char exec(const char *str, char *err)
{
	// this function executes a given instruction.
	// all important functions are called from here.

	char eval(const char *str, char *err, register struct data *eval);
	char asgn[ID]="";  /* assignment variable name container*/
	char error[KB];  /* error information string */

	// data structure to store evaluation results
	auto struct data dat;
	dat.d=0;
	dat.f=0;
	dat.i=0;
	dat.s=NULL;
	if((dat.s=(char *)(malloc(sizeof(char)*
	(strlen(str)+1))))==NULL) allocerr(); *dat.s='\0';

	if(prechar(asgn,str,"=")=='=') /* check for = operator */
	{
		if(strtrims(asgn,' '))
		{  // if assignment string is empty
			strcpy(err,"Assignment variable is not specified.");
			free(dat.s);
			return 1;
		}
		char isid = id_check(asgn);
		if(!isid)
		{
			// assign ***************************************************
			printf("\nassgin: %s\n",asgn);
			free(dat.s);
			return 0;
		}
		else  /* is not an identifier */
		{
			if(isid==1)  /* var name starts with a number */
			{
				strcpy(err,"Variable name cannot start with a number.");
				free(dat.s);
				return 1;
			}
			else  /* var name has illegal chars*/
			{
				stradd(err,"Variable cannot contain [");
				straddc(err,isid);
				stradd(err,"] character");
			}
			free(dat.s);
			return 1;
		}
	}
	else  /* there is no assigment involved */
	{
		// if there is no assignment then string is considered
		// to be the default assignment data type for evaluation.

		if(!eval(str,error,&dat))
		{
			printf("\nint=%d\nflt=%f\ndbl=%f\nstr=%s\n",dat.i,dat.f,dat.d,str);
			free(dat.s);
			return 0;
		}
		else
		{
			strcpy(err,error);
			free(dat.s);
			return 1;
		}
	}
	free(dat.s);
	return 0;
}

char eval(const char *str, char *err, register struct data *eval)
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

	strcpy(eval ->s,"");
	if(!bcheck(str))
	{
		/* this is the core of eval where the calculations are done.
		   at this level, the equation is purely mathematical. */

		const char symbols[]="*/+-%^&";
		const char se[]="Invalid Syntax";
		
		/* check wether str has the absolute stage */
		char *tmp = NULL;
		if((tmp=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *tmp='\0';
		if(prechar(tmp,str,symbols)==0)
		{
			printf("\n[core]");
			if(!ncheck(str))
			{
				eval ->i = strint(str);
				eval ->f = strfloat(str);
				eval ->d = strdouble(str);
				strcpy(eval ->s,str);
				return 0;
			}
			else if(!id_check(str))
			{
				printf("\nit's a variable");
				return 0;
			}
			else
			{
				strcpy(err,se);
				stradd(err,": ");
				stradd(err,str);
				return 1;
			}
			free(tmp);
			return 0;
		}
		else { free(tmp); }

		/* now the real maths */
		printf("\n[mantle]"); 
		char symbol = '\0';
		char *ex = NULL; if((ex=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *ex='\0';
		char *ps = NULL; if((ps=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *ps='\0';
		char *px = NULL; if((px=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *px='\0';

		symbol = prechar(ex,str,symbols);
		postchar(ps,str,symbols);
		if(prechar(px,ps,symbols)==0)
		strcpy(px,ps);
		char *ax = NULL;
		switch(symbol) {
		case '*':	/* multiplication */
		eval ->i = strint(ex) * strint(px);
		eval ->f = strfloat(ex) * strfloat(px);
		eval ->d = strdouble(ex) * strdouble(px);
		break;
		case '/':	/* division */
		eval ->i = strint(ex) / strint(px);
		eval ->f = strfloat(ex) / strfloat(px);
		eval ->d = strdouble(ex) / strdouble(px);
		break;
		case '+':	/* addition */
		eval ->i = strint(ex) + strint(px);
		eval ->f = strfloat(ex) + strfloat(px);
		eval ->d = strdouble(ex) + strdouble(px);
		break;
		case '-':	/* subtraction */
		eval ->i = strint(ex) - strint(px);
		eval ->f = strfloat(ex) - strfloat(px);
		eval ->d = strdouble(ex) - strdouble(px);
		break;
		case '%':	/* modulus */
		eval ->i = strint(ex) % strint(px);
		eval ->f = (float)(fmod(strfloat(ex),strfloat(px)));
		eval ->d = fmod(strdouble(ex),strdouble(px));
		break;
		case '^':	/* power */
		eval ->i = (int)(pow(strint(ex),strint(px)));
		eval ->f = (float)(pow(strfloat(ex),strfloat(px)));
		eval ->d = pow(strdouble(ex),strdouble(px));
		break;
		case '&':	/* concatenation */
        if((ax=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *ax='\0';
		strcpy(ax,ex);
		strcat(ax,px);
		eval ->i = strint(ax);
		eval ->f = strfloat(ax);
		eval ->d = strdouble(ax);
		free(ax);
		break;
		default:	/* unknown */
		strcpy(err,se);
		free(ex);
		free(ps);
		free(px);
		return 1; }
		free(ex);
		free(ps);
		free(px);
		return 0;
	}
	else
	{
		if(!bcount(str))
		{
			printf("\ncomplex");
			return 0;
		}
		else
		{
			strcpy(err,"Illegal Equation, conflicting number of brackets.");
			return 1;
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

char getvar(const char *str, char *err, register struct data *eval)
{
	return 0;
}

char setvar(const char *str, char *err, register struct data *eval)
{
	return 0;
}

char callfunc(const char *name, const int argc, const char *argv[])
{
	return 0;
}

void allocerr(void)
{
	exit(0);
}
