/*******************************************
  Name: Mathematical Expression Evaluator
  Author: Arjun Menon  |  Date: 1/10/05
*******************************************/

/* include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* global functions */

void strtrm(char *str, const char trim, const char mode);

char purity_check(const char *str);
char id_check(const char *str);
char num_check(const char *str);
void allocerr(void);

char prechar(char *str, const char *src, const char *key);
char postchar(char *str, const char *src, const char *key);
int strrht(char *str, const char *src, const unsigned int n);
int strlft(char *str, const char *src, const unsigned int n);
int strmid(char *str, const char *src, const unsigned int m, const unsigned int n);
int strint(const char *str);
int struint(const char *str);
float strfloat(const char *str);
float strufloat(const char *str);
double strdouble(const char *str);
double strudouble(const char *str);
inline void straddc(char *str, const char ch);
inline void strchar(char *str, const char ch);


/* global constants */
unsigned long max_str_length = 1048576;
const char eval_version[] = "EVAL 0.0.2";

/* global variables */
char *cp = NULL;
int *ip = NULL;
float *fp = NULL;
double *dp = NULL;

////////////////////////
// Program Starts Here
////////////////////////

int main()
{
	printf("------------------------------------------\n");
	printf(" EVAL - Mathematical Expression Evaluator \n");
	printf("==========================================\n");
	printf("\n%s\n",eval_version);
	void shell(void);
	shell();
	printf("\nGoodbye!\n");
	return EXIT_SUCCESS;
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
		stralloc: if((cp=(char *)(realloc(str,sizeof(char)*
		(max_str_length))))!=NULL) { str = cp; } else {
		/* if the system is very very low on memory */
		if(max_str_length>=255) { max_str_length -= 1;
		goto stralloc; } else { allocerr(); } }
		printf("\n%s",_shell_arrow);
		gets(str);
		/* re-allocate cmd for reuse */
		if((cp=(char *)(realloc(cmd,sizeof(char)*(strlen(str)+1))))!=NULL)
		{ cmd = cp; } else { allocerr(); }
		strcpy(cmd,str);
		/* destroy str to free (max_str_length) bytes of memory space */
		free(str); str = NULL;
		/* truncuate the string cmd */
		strtrm(cmd,' ','s');
		if(*cmd==0) continue;
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

char prechar(char *str, const char *src, const char *key)
{
	// function prechar takes string preceeding key and stores it in *mov
	// key can be a string if multiple comparisns are to be made.
	// the first key to satisfy comparisn is used and returned as char.

	if(*src=='\0')  /* if str is empty return 0 */
	return 0;
	if(*key=='\0')  /* if key is empty return 0 */
	return 0;
	unsigned int klen = strlen(key);
	unsigned int slen = strlen(src);
	unsigned int u=0, i=0, j=0, k=0;  /* loop counters */
	for(u=0;u<klen;u++)  /* to take each char from key for comparisn */
	{
		if(*key=='\0')  /* exit loop if all keys are over */
		break;
		for(i=0;i<slen;i++)  /* to take each char from str for comparisn */
		{
			if(*src=='\0')  /* check if it is the end of str */
			break;
			if(*src==*key)  /* compare *key and *str */
			{
				for(k=0;k<i;k++)  /* reverse str pointer to start  */
				src--;
				for(k=0;k<i;k++)  /* to take char from str for storage */
				{
					*str=*src;  /* here the new string is made */
					str++;
					src++;
				}
				*str='\0';  /* set termination point for string */
				return *key;
			}
			src++;  /* move pointer to next char in str */
		}
		for(j=0;j<i;j++)  /* reverse str pointer to start  */
		src--;
		key++;  /* move pointer to next char in key */
	}
	return 0;
}

char postchar(char *str,const char *src, const char *key)
{
	// function postchar takes string after key and stores it in *mov
	// key can be a string if multiple comparisns are to be made.
	// the first key to satisfy comparisn is used and returned as char.

	if(*src=='\0')  /* if str is empty return 0 */
	return 0;
	if(*key=='\0')  /* if key is empty return 0 */
	return 0;
	unsigned int klen = strlen(key);
	unsigned int slen = strlen(src);
	unsigned int u=0, i=0, j=0, k=0;  /* loop counters */
	for(u=0;u<klen;u++)  /* to take each char from key for comparisn */
	{
		if(*key=='\0')  /* exit loop if all keys are over */
		break;
		for(i=0;i<slen;i++)  /* to take each char from str for comparisn */
		{
			if(*src=='\0')  /* check if it is the end of str */
			break;
			if(*src==*key)  /* compare *key and *str */
			{
				src++;  /* increment pointer by one block to avoid *key */
				for(k=0;k<(slen-i);k++)  /* to take char from str for storage */
				{
					*str=*src;  /* here the new string is made */
					str++;
					src++;
				}
				*str='\0';  /* set termination point for string */
				return *key;
			}
			src++;  /* move pointer to next char in str */
		}
		for(j=0;j<i;j++)  /* reverse str pointer to start  */
		src--;
		key++;  /* move pointer to next char in key */
	}
	return 0;
}

int strrht(char *str, const char *src, const unsigned int n)
{
	// function strrht takes chosen chars from *str and store it in *rht
	// it start from the right and stores n letters in reverse order.
	// if n exceeds the string length, return will be no.of exceed chars.

	unsigned int len = strlen(src);
	unsigned int i;  /* loop counter */
	for(i=1;i<=(len-n);i++)
	src++;  /* to move pointer to n blocks before null */
	for(i=1;i<=n;i++)
	{
		if(*src=='\0')  /* in case of premature string termination */
		{ 
			*str='\0';
			return ((n-i)+1);  /* calculates how bad the error was */
		}
		*str=*src;  /* here the new string is made */
		str++;
		src++;
	}
	*str='\0';  /* set termination point for string */
	return 0;
}

int strlft(char *str, const char *src, const unsigned int n)
{
	// function strlft takes chosen chars from *str and store it in *lft
	// pointer starts from the left and stores n letters into *str
	// if n exceeds the string length, return will be no.of exceed chars.

	unsigned int i;  /* loop counter */
	for(i=1;i<=n;i++)
	{
		if(*src=='\0')  /* in case of premature string termination */
		{
			*str='\0';
			return ((n-i)+1);  /* calculates how bad the error was */
		}
		*str=*src;  /* here the new string is made */
		str++;
		src++;
	}
	*str='\0';  /* set termination point for string */
	return 0;
}

int strmid(char *str, const char *src, const unsigned int m, const unsigned int n)
{
	// function strmid takes chosen chars from *str and store it in *rht
	// first, the pointer moves forward by m no. of chars.
	// then, it start from the left and stores n letters into *str
	// if n or m exceeds the string length, return will be no.of exceed chars.

	unsigned int i;  /* loop counter */
	for(i=1;i<=m;i++)
	{
		if(*src=='\0')  /* in case of premature string termination */
		{
			*str='\0';
			return (((n+m)-i)+1);  /* calculates how bad the error was */
		}
		src++;  /* to move pointer to m blocks forward */
	}
	for(i=1;i<=n;i++)
	{
		if(*src=='\0')  /* in case of premature string termination */
		{
			*str='\0';
			return ((n-i)+1);  /* calculates how bad the error was */
		}
		*str=*src;  /* here the new string is made */
		str++;
		src++;
	}
	*str='\0';  /* set termination point for string */
	return 0;
}

void strtrm(char *str, const char trim, const char mode)
{
	// this function removes a certain character from a string,
	// then truncuates the string. <const char trim> is removed.
	// The <const char mode> defines the mode of operation:
	// if mode is 'r' or 'R' then right side of str is trimmed.
	// if mode is 'l' or 'L' then left side of str is trimmed.
	// if mode is 's' or 'S' then both sides of str is trimmed.
	// if mode is 'i' or 'I' then the entire string str is trimmed.
	// if mode is anything else, then str is not modified
	// finally, if capital letter is used for mode, then the
	// string str is re-allocated to the new string size, 
	// in order to reduce memory usage. Use capitation only for
	// dynamically allocated strings to prevent memory errors.

	if(trim=='\0')
	return;  /* if trim is null then return */
	if((strlen(str)==0)||(*str=='\0'))
	return;  /* str is empty */

	if(mode=='i'||mode=='I') // inside
	{
		// an extensive trimming operation. this method omits all
		// the trim chars from str and squeezes str to close holes.

		char ch; /* temporary character storage variable */
		unsigned int i, j;  /* loop counters */
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
		if(!strlen(str)) *str = '\0';
	}
	else if(mode=='r'||mode=='R') // right
	{
		// this method removes all char trim selectively from str
		// starting from right, it removes trim until a non-trim char.
		// if modified str is '\0', then return is 1, otherwise is 0

		const unsigned int len = strlen(str);
		unsigned int i = 0; /* loop counter */
		for(i=0;i<len;i++)
		str++; /* push str poiner to the right end */
		unsigned char wfound=0;
		for(i=0;i<len;i++)
		{
			str--;  /* move pointer backward by one block */
			if(*str!=trim)
			wfound=1;  /* reached end of trim chars */
			if(wfound)  /* now reached the end of actual string*/
			{
				str++;  /* move pointer forward by one block */
				*str='\0';    /* set termination point for string */
				goto end;
			}
		}
		if(i==len) /* check is str is full of trims */
		*str = '\0';
	}
	else if(mode=='l'||mode=='L') // left
	{
		// this method removes all char trim selectively from str
		// starting from left, it removes trim until a non-trim char.
		// if modified str is '\0', then return is 1, otherwise is 0

		const unsigned int len = strlen(str);
		unsigned int w = 0;  /* to store no.of trim chars */
		unsigned int l = 0;  /* to store real length */
		unsigned int i = 0;  /* loop counter */
		unsigned char wfound=0;  /* boolean to verify if w is found */
		for(;*str!='\0';str++)
		{
			if(*str!=trim)
			wfound=1;  /* now w is found */
			if(!wfound)
			if(*str==trim)
			w++;  /* find out the no. of trim chars. */
		}
		if(w==len) /* check is str is full of trims */
		{
			for(i=0;i<len;i++)
			str--;  /* move str pointer to the starting*/
			*str = '\0';
			goto end;
		}
		l=(len - w);  /* find out real length */
		for(i=0;i<len;i++)
		str--;  /* move str pointer to the starting*/
		char ch; /* temporary character storage variable */
		unsigned int u = 0;  /* loop counter */
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
		/* set termination point for string */
		*str='\0';
	}
	else if(mode=='s'||mode=='S') // sides
	{
		// the function is recursed to remove
		// char trim from left and right sides.
		strtrm(str,trim,'r');
		strtrm(str,trim,'l');
	}
end:
	if(mode=='I'||mode=='L'||mode=='R'||mode=='S')
	{
		// re-alocate str to new size.
		int newsize = strlen(str)+1;
		if((cp=(char *)(realloc(str,sizeof(char)*newsize)))!=NULL)
		{ str = cp; } else { allocerr(); }
	}
	return;
}


int strint(const char *str)
{
	// this function extracts an integer from a string.
	// note: 48 is '0' and '57' is 9 in ASCII.

	signed int n = 0;  /* to carry return value */
	unsigned char i = 0;  /* to contain a digit */
	signed char negative = 1; /* sign flag */
	for(;*str!='\0';str++)
	{
		if((*str>=48)&&(*str<=57))
		{
			i = *str - 48;  /* translate char to number */
			n = n * 10 + i;  /* mathematical formula */
		}
		else if(*str=='-')
		negative = -1;
	}
	return n*negative;
}

int struint(const char *str)
{
	// this function extracts an integer from a string.
	// note: 48 is '0' and '57' is 9 in ASCII.

	signed int n = 0;  /* to carry return value */
	unsigned char i = 0;  /* to contain a digit */
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
	// note: 48 is '0', 57 is '9' and 46 is '.' in ASCII.

	unsigned char i = 0;  /* to contain a digit */
	unsigned int c = 0; /* loop counter */
    char negative = 1; /* sign flag */
	float n = 0, ix = 0;  /* to carry return value & subzero value */
	unsigned char z=0, zp=0;  /* flag & count for decimal point */
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
		else if(*str=='-')
		negative = -1;
	}
	return n*negative;
}

float strufloat(const char *str)
{
	// this function extracts a float from a string.
	// note: 48 is '0', 57 is '9' and 46 is '.' in ASCII.

	unsigned char i = 0;  /* to contain a digit */
	unsigned int c = 0; /* loop counter */
	float n = 0, ix = 0;  /* to carry return value & subzero value */
	unsigned char z=0, zp=0;  /* flag & count for decimal point */
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

	unsigned char i = 0;  /* to contain a digit */
	unsigned int c = 0; /* loop counter */
	char negative = 1; /* sign flag */
	double n = 0, ix = 0;  /* to carry return value & subzero value */
	unsigned char z=0, zp=0;  /* flag & count for decimal point */
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
		else if(*str=='-')
		negative = -1;
	}
	return n*negative;
}

double strudouble(const char *str)
{
	// this function extracts a double from a string.
	// note: 48 is '0', '57' is 9 and 46 is '.' in ASCII.

	unsigned char i = 0;  /* to contain a digit */
	unsigned int c = 0; /* loop counter */
	double n = 0, ix = 0;  /* to carry return value & subzero value */
	unsigned char z=0, zp=0;  /* flag & count for decimal point */
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
{ str+=strlen(str); *str++=ch;*str='\0'; }

inline void strchar(char *str, const char ch)
{ *str++ = ch; *str = '\0'; }

/////////////////////////////////////////
// Core Expression Evaluation Functions
/////////////////////////////////////////

void runline(const char *cmd, char *err)
{
	// this function takes an instruction for execution.
	// it removes all comments and passes the statement to exec

	void exec(const char *str, char *err);
	char impurity[2] = {'\0','\0'};
	impurity[0] = purity_check(cmd);
	if(*cmd==';') /* if statement is a comment. */
	return;
	if(!impurity[0])  /* if cmd is pure */
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
			if(*cmd=='#')  /* remove comments from the statement */
			break;
			*spointer = *cmd;  /* transfer cmd to str */
		}
		*spointer='\0';  /* set termination point for string */
		strtrm(statement,' ','s');
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
		strcat(err,impurity);
		strcat(err,"] is not allowed.");
		return;
	}
}

void exec(const char *str, char *err)
{
	// this function executes a given instruction.
	// all important functions are called from here.

	char * iftopf(const char *str);
	strcpy(err,"");  /* empty error container string */
//	int integer=0;  /* integer to store integer evaluation results */
//	double real=0;  /* double to store double evaluation results */
	char *asgn = NULL;  /* string containing assignment variable name */
	if((asgn=(char *)(malloc(sizeof(char)*
	(strlen(str)+1))))==NULL) allocerr(); *asgn='\0';
	char *exp = NULL; /* string containing postfix expression */

	if(prechar(asgn,str,"=")=='=') /* check for = operator */
	{
		/* is a variable assignment */
		strtrm(asgn,' ','s');
		if(*asgn==0)
		{  // if assignment string is empty
			strcpy(err,"Assignment variable is not specified.");
			return;
		}
		char isid[2] = {'\0','\0'};
		isid[0] = id_check(asgn);
		if(!isid[0])
		{
			if(!strlen(err))
			{
				printf("\n[%s]\n",asgn);
				return;
			}
			else  /* there was an error */
			{
				return;
			}
		}
		else  /* is not an identifier */
		{
			if(isid[0]==1)  /* var name starts with a number */
			{
				strcpy(err,"Variable name cannot start with a number.");
				return;
			}
			else  /* var name has illegal chars*/
			{
				strcpy(err,"Variable cannot contain [");
				strcat(err,isid);
				strcat(err,"] character");
			}
			return;
		}
	}
	else  /* there is no assigment involved */
	{
		// if there is no assignment then integer is considered
		// to be the default assignment data type for evaluation.
		iftopf(str);
		if(!strlen(err))
		{
			printf("\n%s\n",exp);
			return;
		}
		else  /* there was an error */
		{
			return;
		}
	}
}

char * iftopf(const char *str)
{
	//  INFIX EXP. TO POSTFIX EXP. TRANSLATOR
	// ---------------------------------------
	// This function converts a postfix expression
	// to an infix expression if the conversion was 
	// successful, the evaluated expression is returned 
	// as a string, wheras if converson fails then a 
	// string containing a '#' character followed by 
	// an error statement is returned.

	return 0;
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
	signed int i=0;
	i = atoi(str);
	char *vals = NULL;
	if((vals=(char *)(malloc(sizeof(char)*
	(strlen(str)+1))))==NULL) allocerr(); *vals='\0';
	itoa(i,vals,10);
	return vals;
}

void allocerr(void)
{
	/* invoked when memory allocation fails */
	printf("\nNot enough memory.\a\n");
	exit(1);
}
