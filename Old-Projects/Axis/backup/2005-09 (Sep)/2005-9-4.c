/*******************************************
  Name: Mathematical Expression Evaluator
  Author: Arjun Menon  |  Date: 4/09/05
*******************************************/

/* include headers*/
#include "stdio.h"
#include "string.h"

/* definitions */
#define KB 1024
#define IDSZ 32

/* global functions	*/
char prechar(char *mov,const char *str, const char *key);
char postchar(char *mov,const char *str, const char *key);
int strrht(char *rht, const char *str, const int n);
int strlft(char *lft, const char *str, const int n);
int strmid(char *mid, const char *str, const int m, const int n);
short int strtriml(char *str, const char trim);
short int strtrimr(char *str, const char trim);
short int strtrims(char *str, const char trim);
short int strtrim(char *str, const char trim);
int strint(const char *str);
float strfloat(const char *str);
double strdouble(const char *str);
void stradd(char *str, const char *add);
void straddc(char *str, const char add);
void tostr(char *str, const char ch);

/* global typedefs */
typedef struct { 
	int i;
	float f;
	double d;
	char s[KB];
} data;
typedef struct {
	unsigned int d : 5;  // max 32
	unsigned int m : 4;  // max 16
	unsigned int y : 7;  // max 128
} date;

/* global variables*/

/* global constants */
const char _eval_version[]="EVAL 0.0.1";
const char _str_null[]="";

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

	/* shell system */
	char cmd[KB]="";
	char err[KB]="";
	char runline(const char *cmd, char *err);
	while(strcmp(cmd,_shell_exit))
	{
		printf("\n%s",_shell_arrow);
		gets(cmd);
		strtrims(cmd,' ');
     	if(!strcmp(cmd,_str_null))
		continue;
		/* cmd should not be exit */
		if(strcmp(cmd,_shell_exit))
		/* check if eval return is 0 */
		if(runline(cmd,err))
		printf("\nError: %s\n",err);
	}
}

///////////////////////////////////////////
// Extended String Manupilation Functions
///////////////////////////////////////////

char prechar(char *mov,const char *str, const char *key)
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

short int  strtriml(char *str, const char trim)
{
	// function strtriml removes char trim selectively from str
	// starting from left, it removes trim until a non-trim char.
	// if modified str is '\0', then return is 1, otherwise is 0

	if(trim=='\0')  /* if trim is null then return */
	return 1;
	int len = strlen(str);
	int w = 0;  /* to store no.of trim chars */
	int l = 0;  /* to store real length */
	short int wfound=0;  /* boolean to verify if w is found */
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

short int strtrimr(char *str, const char trim)
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
	short int wfound=0;
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

short int strtrims(char *str, const char trim)
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

short int strtrim(char *str, const char trim)
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
	// note: 48 is '1' and '57' is 9 in ASCII.

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
	// note: 48 is '1', '57' is 9 and 46 is '.' in ASCII.

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
	// note: 48 is '1', '57' is 9 and 46 is '.' in ASCII.

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

void stradd(char *str, const char *add)
{
	for(;*str!='\0';str++);
	strcpy(str,add);
}

void straddc(char *str, const char add)
{
	for(;*str!='\0';str++);
	*str=add;
	str++;
	*str='\0';
}

void tostr(char *str, const char ch)
{
	*str++ = ch;
	*str = '\0';
}

/////////////////////////////////////////
// Core Expression Evaluation Functions
/////////////////////////////////////////

char runline(const char *cmd, char *err)
{
	// this function takes an instruction for execution.
	// it removes all comments and passes the statement to exec

	char exec(const char *str, char *err);
	char _err[KB]="";  /* error information string */
	char purity_check(const char *str);
	char impurity = purity_check(cmd);
	if(*cmd==';') /* if statemnt is a comment. */
	return 0;
	if(!impurity)  /* if cmd is pure */
	{
		char statement[KB]="";
		// remove comments from the statement
		int l = 0;  /* to store length of spointer */
		l = strlen(cmd);
		int i = 0;  /* loop counter */
		for(i=0;i<l;i++)
		{
			if(*cmd==';')
			break;
			statement[i] = *cmd;
			cmd++;
		}
		statement[i]='\0';
		strtrims(statement,' ');
		// now run the statement
		if(!exec(statement,_err))  /* if eval was succesfull. */
		return 0;
		else  /* if eval was un-succesfull. */
		strcpy(err,_err);  /* transfer eval's err to err */
		return 1;
	}
	else  /* if cmd is impure */
	{
		stradd(err,"Character [");
		straddc(err,impurity);
		stradd(err,"] is not allowed.");
		return 1;
	}
	return 1;
}

char exec(const char *str, char *err)
{
	// this function executes a given instruction.
	// all important functions are called from here.

	char eval(const char *str, char *err, register data *eval);
	char id_check(const char *str);
	char asgn[IDSZ]="";  /* assignment variable name container*/
	char _err[KB]="";  /* error information string */

	// data structure to store evaluation results
	data dat;
	dat.d=0;
	dat.f=0;
	dat.i=0;
	dat.s[0]='\0';

	if(prechar(asgn,str,"=")=='=') /* check for = operator */
	{
		if(strtrims(asgn,' '))
		{  // if assignment string is empty
			strcpy(err,"Assignment variable is not specified.");
			return 1;
		}
		char isid = id_check(asgn);
		if(!isid)
		{
			// assign ***************************************************
			printf("\nassgin: %s\n",asgn);
		}
		else  /* is not an identifier */
		{
			if(isid==1)  /* var name starts with a number */
			{
				strcpy(err,"Variable name cannot start with a number.");
				return 1;
			}
			else  /* var name has illegal chars*/
			{
				stradd(err,"Variable cannot contain [");
				straddc(err,isid);
				stradd(err,"] character");
			}
			return 1;
		}
	}
	else  /* there is no assigment involved */
	{
		// if there is no assignment then string is considered
		// to be the default assignment data type for evaluation.
		char tmp[KB]="";
		eval(str,_err,&dat);		
		strcpy(tmp,dat.s);
		printf("\n%s\n",tmp);
	}
	return 0;
}

char eval(const char *str, char *err, register data *eval)
{
	// Mathematical Expression Evaluation Function
	//---------------------------------------------

	strcpy(eval ->s, str);
	return 0;
}

char purity_check(const char *str)
{
	// checks the purity of a statement.
	// allowed characters in a statement are from 32 to 162

	for(;*str!='\0';str++)
	if(!(*str>=32&&*str<=126))
	return *str;
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

int callfunc(const char *name, const int argc, const char *argv[])
{
	return 0;
}
