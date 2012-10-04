/*******************************************
  Name: Mathematical Expression Evaluator
  Author: Arjun Menon  |  Date: 29/10/05
*******************************************/

/* include files */
#include "arjun.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* global function prototypes */
PROTOTYPE(char prechar, (char *str, const char *src, const char *key));
PROTOTYPE(char postchar, (char *str, const char *src, const char *key));
PROTOTYPE(int strrht, (char *str, const char *src, const unsigned int n));
PROTOTYPE(int strlft, (char *str, const char *src, const unsigned int n));
PROTOTYPE(int strmid, (char *str, const char *src, const unsigned int m, const unsigned int n));
PROTOTYPE(void strtrm, (char *str, const char trim, const char mode));
PROTOTYPE(inline void strcatc,(char *str, const char chr));
PROTOTYPE(inline int strisc,(const char *str, const char chr));
PROTOTYPE(inline char strisn,(const char *str));
PROTOTYPE(char purity_check, (const char *str));
PROTOTYPE(char id_check, (const char *str));
PROTOTYPE(char numm_check, (const char *str));
PROTOTYPE(void allocerr, (void));

/* global constants */
unsigned long max_str_length = 1048576;
const char eval_version[] = "EVAL 0.0.2";

/* type definitions */
typedef struct { char c; int i; } char_int;

/* global variables */
int *ip = NULL;
float *fp = NULL;
double *dp = NULL;
char *cp = NULL;

/* gloabl structures */
struct  /* common bits */
{
	unsigned char qt : 1;
} bits;

////////////////////////
// Program Starts Here
////////////////////////

int main(int argc, char *argv[])
{
	puts("------------------------------------------");
	puts(" EVAL - Mathematical Expression Evaluator ");
	puts("==========================================");
	void shell(void); shell();
	puts("\nGoodbye!\n");
	return EXIT_SUCCESS;
}

void shell(void)
{
	printf("\n%s\n",eval_version);

	/* shell constants */
	const char _shell_arrow[] =">>";
	const char _shell_exit[]="exit";

	/* shell variables */
	char *cmd = NULL;
	char *str = NULL;
	char *err = NULL;
	PROTOTYPE(void runline, (const char *str, char *err));

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
	// function strrht takes chosen chars from *src and store it in *str
	// pointer starts from the right and stores n letters in reverse order.
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
	// function strlft takes chosen chars from *src and stores it in *str.
	// the pointer starts from the left and stores n letters into *str
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
	//  STRING TRIMMING FUNCTION:
	// ---------------------------
	// This function removes a certain character from a string,
	// then truncuates the string. <const char trim> is removed.
	// The <const char mode> defines the mode of operation:
	// if mode is 'r' or 'R' then right side of str is trimmed.
	// if mode is 'l' or 'L' then left side of str is trimmed.
	// if mode is 's' or 'S' then both sides of str is trimmed.
	// if mode is 'i' or 'I' then the entire string str is trimmed.
	// if mode is anything else, then str is not modified
	// Finally, if capital letter is used for mode, then the
	// string str is re-allocated to the new string size, 
	// in order to reduce memory usage. Warning: Use capital
	// char mode *only* for dynamically allocated strings.

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

inline void strcatc(char *str, const char chr)
{
	// add a single character chr to the string *str.
	while(*str!='\0') str++;
	*str++=chr; *str='\0';
}

inline int strisc(const char *str, const char chr)
{
	// searches a string for char chr & returns it, else 0.
	while(*str!='\0')
	if(*str++==chr)
	return *--str;
	return 0;
}

inline char strisn(const char *str)
{
	// checks wether a string consists of real/integer numbers.
	for(;*str!='\0';str++)
	if((*str<'0'||*str>'9')&&*str!='.')
	return *str; return 0;
}

/////////////////////////////////////////
// Core Expression Evaluation Functions
/////////////////////////////////////////

void runline(const char *str, char *err)
{
	// this function takes an instruction for execution.
	// it removes all comments and passes the statement to exec

	PROTOTYPE(void exec, (const char *str, char *err));
	char impurity[2] = {'\0','\0'};
	impurity[0] = purity_check(str);
	if(*str==';') /* if statement is a comment. */
	return;
	if(!impurity[0])  /* if cmd is pure */
	{
		strcpy(err,"");
		char *statement = NULL;
		if((statement=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *statement='\0';
		char *spointer = statement;
		int l = 0;  /* to store length of spointer */
		l = strlen(str);
		int i = 0;  /* loop counter */
		bits.qt=0;
		for(i=0;i<l;i++,str++,spointer++)
		{
			if(*str==34||*str==39)  /* char["] and char['] */
			{
				if(bits.qt==0)
				bits.qt = 1;  /* special symbol starts */
				else
				bits.qt = 0;  /* special symbol ends */
			}
			if(bits.qt==0)
			if(*str=='#')  /* remove comments from the statement */
			break;
			*spointer = *str;  /* transfer cmd to str */
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
	// all important functions are done from here.

	strcpy(err,"");  /* empty error container string */
	char *exp = NULL; /* string containing evaluation expression */
	char *asgn = NULL;  /* string containing assignment variable name */
	char data_type = '\0'; /* data type of variable to evaluate */

	/* assignment analysis & error handling */
	if((asgn=(char *)(malloc(sizeof(char)*
	(strlen(str)+1))))==NULL) allocerr(); *asgn='\0';

	/* check for = operator */
	if(prechar(asgn,str,"=")=='=')
	{
		/* ok, it is a variable assignment */
		strtrm(asgn,' ','S');

		if((*asgn==0)||(strlen(asgn)==0))
		{
			/* error: assignment string is empty */
			strcpy(err,"Assignment variable is not specified.");
			free(asgn);
			return;
		}
		else  /* assignment string is NOT empty */
		{
			char isid[2] = {'\0','\0'};
			/* check identifier validity*/
			isid[0] = id_check(asgn);
			if(isid[0]!=0)  /* error: is not an identifier */
			{
				if(isid[0]==1)  /* error: var name starts with a number */
				{
					strcpy(err,"Variable name cannot start with a number.");
				}
				else if(isid[0]==32)  /* error: var name contains spaces */
				{
					strcpy(err,"Variable name cannot contain spaces.");
				}
				else  /* error: var name has illegal chars*/
				{
					strcpy(err,"Variable name cannot contain [");
					strcat(err,isid);
					strcat(err,"] character.");
				}
				free(asgn);
				return;
			}
			else  /* success: assignment string is valid */
			{
				/* allocate string exp */
				if((exp=(char *)(malloc(sizeof(char)*
				(strlen(str)+1))))==NULL) allocerr(); *exp='\0';

				/* now split the equation to get the exp. */
				postchar(exp,str,"=");
				strtrm(exp,' ','S');

				// find out the data type of the assignment
				// variable & store it in char data_type
				data_type = ' ';

				/* NOT YET COMPLETED */
			} 

		}
	}
	else	/* there is no assigment involved */
	{
		/* allocate string exp */
		if((exp=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *exp='\0';

		/* store & trim exp */
		strcpy(exp,str);
		strtrm(exp,' ','S');

		/* de-allocate asgn */
		free(asgn);
		asgn = NULL;
	}

	/* variable assignment */
	if(asgn!=NULL)
	{
		/* NOT YET COMPLETED */

		/* possible variable data types */
		long *i = NULL;            // <- (int)
		short *n = NULL;           // <- (int-)
		char *c = NULL;            // <- (chr)
		double *d = NULL;          // <- (real)
		float *f = NULL;           // <- (real-)
		unsigned long *j = NULL;   // <- (int+)
		unsigned short *m = NULL;  // <- (int-+)
		unsigned char *a = NULL;   // <- (chr+)

		/* allocate memory of data type */
		switch(data_type)
		{
		case 'i':
			if((i=(long *)(malloc(sizeof(long
			))))==NULL) allocerr(); *i = 0;
			break;
		case 'n':
			if((n=(short *)(malloc(sizeof(short
			))))==NULL) allocerr(); *n = 0;
			break;
		case 'c':
			if((c=(char *)(malloc(sizeof(char
			))))==NULL) allocerr(); *c = 0;
			break;
		case 'd':
			if((d=(double *)(malloc(sizeof(double
			))))==NULL) allocerr(); *d = 0;
			break;
		case 'f':
			if((f=(float *)(malloc(sizeof(float
			))))==NULL) allocerr(); *f = 0;
			break;
		case 'j':
			if((j=(unsigned long *)
			(malloc(sizeof(unsigned long
			))))==NULL) allocerr(); *j = 0;
			break;
		case 'm':
			if((m=(unsigned short *)
			(malloc(sizeof(unsigned short
			))))==NULL) allocerr(); *m = 0;
			break;
		case 'a':
			if((a=(unsigned char *)
			(malloc(sizeof(unsigned char
			))))==NULL) allocerr(); *a = 0;
			break;
		default:
			strcpy(err,"Fatal Data Transfer Error on [");
			strcat(err,asgn);
			strcat(err,"] variable.");
		}

		/* NOT YET COMPLETED */

		/* de-allocate memory of data type */
		switch(data_type)
		{
		case 'i':
			free(i);
			i = NULL;
			break;
		case 'n':
			free(n);
			n = NULL;
			break;
		case 'c':
			free(c);
			c = NULL;
			break;
		case 'd':
			free(d);
			d = NULL;
			break;
		case 'f':
			free(f);
			f = NULL;
			break;
		case 'j':
			free(j);
			j = NULL;
			break;
		case 'm':
			free(m);
			m = NULL;
			break;
		case 'a':
			free(a);
			a = NULL;
			break;
		}
		/* de-allocate asgn */
		free(asgn);
		asgn = NULL;
	}
	else	/* no assigment */
	{
   		// if there is no assignment then integer is considered
		// to be the default assignment data type for evaluation.
		PROTOTYPE(void eval, (const char *str, char *err, long *val));
		PROTOTYPE(void evali,(const char *str, char *err, int *eval));

		int i = 1;
		evali(exp,err,&i);
		if(!strlen(err))
		printf("\neval[i]: %d\n",i);

		long int l = 1;
		eval(exp,err,&l);
		if(!strlen(err))
		printf("\neval[=]: %ld\n",l);
	}
	free(exp);
	exp = NULL;
}

void eval(const char *str, char *err, long *val)
{
	// this function evaluates mathematical expression
	// as a <long> and stores the resultant value in *val.

	PROTOTYPE(void vals, (const char *str, char *err, long *val));

	strcpy(err,"");  /* empty error string */
	unsigned int len; /* length of str */
	unsigned char nature = 0;
	unsigned int brackets[2];
	unsigned int joiners = 0;

	/* find out nature of str */
	for(len=0;str[len]!='\0';len++)
	{
		if((str[len]<='0'||
			str[len]>='9')&&
			str[len]!='.'&&
			str[len]!='\0')
			nature = 1;
		/* the nature of str has been found as:
		 *	0 - if numerical.
		 *	1 - if alphanumeric
		 */
		if(str[len]==34||str[len]==39)
		joiners += 1; /* quotes */
		if(str[len]=='('&&joiners%2==0)
		brackets[0] += 1;
		if(str[len]==')'&&joiners%2==0)
		brackets[1] += 1;
	}

	// check integrity
	if((joiners%2!=0)||brackets[0]!=brackets[1])
	{
		strcpy(err,"Syntax Error in expression. Recount the brakcets & quotation marks.");
		return;
	}

	// a pure number
	if(!nature)
	{
		/* to check if the exp. has been 
		un-neccessarily accelerated */
		if(*str=='.')
		str++;
		/* now return integer translation */
		*val = atol(str);
		return; // <--------------- FUNCTION RETURN
	}

	// mathematical expression
	else
	{
		char *trans =  NULL;
		/* allocate a string trans which 
		 * will be used as a temporary
		 * string by various algorithms.
		 */
		if((trans=(char *)(malloc(sizeof(char)*
		((strlen(str)*2)+1))))==NULL)
		allocerr(); *trans='\0';
		 /* loop counter */
		unsigned int lc = 0;
		/* postfix stack size descriptor */
		unsigned int stack_size = 0;

		/**************************************.
		| Accelerated (.) Expression Evaluator |
		`**************************************/
		if(*str=='.')
		{
			/* it's an accelerated expression */
			str++;

			bits.qt = 0;
			/* retreive the intial value */
			for(lc=0;str[lc]!='\0';lc++)
			{
				/* set quotation bit */
				if(str[lc]==34||str[lc]==39) {
				/* if char["] or char['] */
				if(bits.qt) bits.qt = 0;
				else bits.qt = 1; }

				/* loop till quote ends */
				if(bits.qt)
				continue;

				/* here trans is made */
				if(str[lc]!='^'&&
				   str[lc]!='*'&&
				   str[lc]!='/'&&
				   str[lc]!='+'&&
				   str[lc]!='-'&&
				   str[lc]!='&'&&
				   str[lc]!='\0')
				trans[lc] = str[lc];
				else
				break;
			}
			trans[lc] = '\0';

			/* if there are no math symbols, 
			 * then lenght of str will be 
			 * equal to lc+1 (add 1 for '.')
			 */
			if(len==lc+1)
			{
				strtrm(trans,' ','S');
				/* it's a pure variable */
				vals(trans,err,val);
				free(trans);
				return; // <--------------- FUNCTION RETURN
			}

			/* set initial value */
			long value = 0;
			strtrm(trans,' ','s');
			if(*trans=='\0')
			value = *val;
			else
			{
				if(strisn(trans))
				vals(trans,err,&value);
				else
				value = atol(trans);
			}
			printf("\ninit[#]: %ld",value);

			/* [prepare now] */
			strcpy(trans,"");
			int tranLC = 0;  /* a char. counter for trans */
			long opval = 0;
			char mathSymbol = str[lc++];
			bits.qt=0;

			/* here the math is done */
			for(;;lc++,tranLC++)
			{
				/* set quotation bit */
				if(str[lc]==34||str[lc]==39) {
				/* if char["] or char['] */
				if(bits.qt) bits.qt = 0;
				else bits.qt = 1; }

				if((str[lc]!='^'&&
 	 			    str[lc]!='*'&&
				    str[lc]!='/'&&
				    str[lc]!='+'&&
				    str[lc]!='-'&&
				    str[lc]!='&'&&
				    str[lc]!=' '&&
				    str[lc]!='\0')
				    ||bits.qt)
				{
					/* here chars from str are
					 * dropped into str trans
					 * until a mathematical
					 * operator is encountered.
					 */
					trans[tranLC]=str[lc];
					/* when a math. operator
					 * is encountered the long
					 * value is operated upon
					 * the long translation
					 * of the trans string.
					 */
				}
				else
				{
					/* trans is finalised */
					trans[tranLC] = '\0';
					printf("\ntran[%c]: %s",mathSymbol,trans);
					/* required operations are performed */
					switch(mathSymbol)
					{
					case '^':
						if(strisn(trans)) {
						vals(trans,err,&opval);
						if(strlen(err)) {
						free(trans);
						return; } } // <---------- FUNCTION RETURN
						else  /* it is a number */
						opval = atol(trans);
						value = (long)(pow(value,opval));
						break;
					case '*':
						if(strisn(trans)) {
						vals(trans,err,&opval);
						if(strlen(err)) {
						free(trans);
						return; } } // <---------- FUNCTION RETURN
						else  /* it is a number */
						opval = atol(trans);
						value *= opval;
						break;
					case '/':
						if(strisn(trans)) {
						vals(trans,err,&opval);
						if(strlen(err)) {
						free(trans);
						return; } } // <---------- FUNCTION RETURN
						else  /* it is a number */
						opval = atol(trans);
						value /= opval;
						break;
					case '+':
						if(strisn(trans)) {
						vals(trans,err,&opval);
						if(strlen(err)) {
						free(trans);
						return; } } // <---------- FUNCTION RETURN
						else  /* it is a number */
						opval = atol(trans);
						value += opval;
						break;
					case '-':
						if(strisn(trans)) {
						vals(trans,err,&opval);
						if(strlen(err)) {
						free(trans);
						return; } } // <---------- FUNCTION RETURN
						else  /* it is a number */
						opval = atol(trans);
						value -= opval;
						break;
					case '&':
						if(strisn(trans)) {
						vals(trans,err,&opval);
						if(strlen(err)) {
						free(trans);
						return; } } // <---------- FUNCTION RETURN
						else  /* it is a number */
						opval = atol(trans);
						long _opval = opval;
						do { 
						_opval/= 10;
						value *= 10; }
						while(_opval!=0);
						value += opval;
						break;
					}
					if(str[lc]!=' ')
					mathSymbol = str[lc];
					strcpy(trans,"");
					tranLC = -1;
					/* end loop at end of str */
					if(str[lc]=='\0')
					break;
				}
			}
			*val = value;
			free(trans);
			return;  // <--------------- FUNCTION RETURN
		}

		/**********************************.
		| Postfix (\) Expression Extractor |
		`**********************************/
		else if(*str=='\\')
		{
		/*
		 *  In order to allow user defined postfix expressions and
		 *  differentiate them from ordinary infix expresions, every
		 *  postfix expression *must* begin with a '\' character.
		 */
			/* the expression is postfix */
			str++;
			for(lc=0;;lc++)
			{
				if(str[lc]>='0'&&str[lc]<='9')
				stack_size = stack_size*10 + (str[lc]-'0');
				else if(str[lc]==':') { 
				strcpy(trans,&str[lc+1]); break; }
				else if(str[lc]=='\0') {  /* unspecified stack size */
				strcpy(err,"Invalid Postfix Expression, unknown stack size. Did you forget : ?");
				free(trans); return; }  // <--------------- FUNCTION RETURN
				else {  /* syntax error */
				strcpy(err,"Syntax Error in your postfix expression, [");
				strcatc(err,str[lc]);
				strcat(err,"] charcater is not allowed.");
				free(trans); return; }  // <--------------- FUNCTION RETURN
			}   
			if(!stack_size) stack_size = 32;
			/* okay, now we know the stack size */
		}
		/****************************************************.
		| Infix Expression => Postfix Expression Translator  |
		`****************************************************/
		else
		{
			/* it's an ordinary infix expression */
			PROTOTYPE(int iftopf, (const char *str, char *err, char *pfe));
			stack_size = iftopf(str,err,trans); /* covert to postfix */
			if(strlen(err)) {
			free(trans);
			return;  } // <--------------- FUNCTION RETURN
		}

	    /*****************************************.
		| << CORE >> Postfix Expression Evaluator |
		`*****************************************/
		/*
		 *  At this stage, trans *must* contain a
		 *  postfix expression becuase if the equation 
		 *  was an ordinary infix expression, it has
		 *  been translated to postfix and stored
		 *  in the string trans. Or else, if it
		 *  was an accelerated expression, then it
		 *  has been solved to *val and returned to
		 *  exec, otherwise if the function iftopf 
		 *  has met with an error, then too the
		 *  function has returned to exec. In all
		 *  other cases, the program reaches here.
		 */
		if(!stack_size) stack_size = 32;
		printf("\nstack = %d",stack_size);
		printf("\ntran: %s",trans);

		/* stack integer array of size stack_size */
		long *stack = NULL;
		if((stack=(long *)(malloc(sizeof(int)*
		(stack_size))))==NULL) allocerr();
		
		/* this is string is used to temporarily 
		 store operators of the postfix expression */
		char *ops = NULL;
		if((ops=(char *)(malloc(sizeof(char)*
		(strlen(trans)+1))))==NULL) allocerr(); *ops='\0';

		unsigned int stlc = 0; /* stack counter */
		signed int oplc = 0;  /* char ops[] counter */

		/* infinite loop */
		for(joiners=0,bits.qt=0;;lc++,oplc++)
		{
			if((trans[lc]==','||
			   trans[lc]=='\0')
			   /* skip off commas if inside quotes or brackets */
			   &&(!bits.qt)&&(!joiners))
			{
				/* finalise ops */
				ops[oplc] = '\0';
				strtrm(ops,' ','s');
				printf("\nops: %s",ops);
				/* now ops is ready for processing */
				if(strisn(ops)==0) /* it's just a number */
				{ printf(" {num}");
				stack[stlc] = atol(ops);
				goto procend; }
				/* processing starts */
				switch(*ops)
				{
				/* NOT YET COMPLETED */
				case ' ':
					break;
				default:
					printf(" {val}");
					vals(ops,err,&stack[stlc]);
					if(strlen(err)) {
					/* in case of error */
					free(ops);
					free(stack);
					free(trans);
					return; } // <---------- FUNCTION RETURN
				}
				/* end of processing */
				procend:
				printf(" = %ld <- %d",stack[stlc],stlc);
				/* empty ops */
				strcpy(ops,"");
				oplc = -1;
				stlc++;
				/* loop breaker */
				if(trans[lc]=='\0')
				break;
			}
			else  /* trans[lc] != ',' && '\0' */
			{
				/* set bracket bit */
				if(str[lc]=='('&&!bits.qt)
				joiners++;
				else if(str[lc]==')'&&!bits.qt)
				joiners--;
				/* set quotation bit */
				else if(str[lc]==34||str[lc]==39) {
				/* if char["] or char['] or brackets */
				if(bits.qt) bits.qt = 0;
				else bits.qt = 1; }
				/* here we build ops */
				ops[oplc] = trans[lc];
				/* to avoid infinite loops */
				if(trans[lc]=='\0')
				break; /* though this will never happen */
			}
			/* NOT YET COMPLETED */
		}

		/* now val has been found */
		*val = -1;
		free(ops);
		free(stack);
		free(trans);
		return;  // <--------------- FUNCTION RETURN
	}
}

int iftopf(const char *str, char *err, char *pfe)
{
	// this function translates an infix expression
	// into a postfix expression and stores the
	// resultant translated string in char *pfe.
	// -[the format of the infix expression]:
	// all varibales & numbers are seperated by
	// the comma(',') character.

	strcpy(err,"");  /* empty error string */

	/* since iftopf is not fully functional */
	strcat(pfe,str);  /* NOT YET COMPLETED */
	return 0;
}

void evali(const char *str, char *err, int *eval)
{
	//  Mathematical Expression Evaluation Function (long)
	// ---------------------------------------------------
	//  This functions solves infix expressions as long
	//	integers. When an infix expression is passed via
	//  *str, the expression is broken into parts, and the
	//	part with the highest prority is passed on recursively 
	//	onto the function itself. This recurvise process is 
	//  repeated until the whole expression has been solved.
	//  Results of the evaluation are stored in int *eval.

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
		if((tmp=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *tmp='\0';
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
			else if(!id_check(str))
			{
				free(tmp);
				tmp = NULL;
				/*
				char *vals = NULL;
				vals = valstr(str);
				*eval = atoi(vals);
				free(vals);
				vals = NULL;
				*/
				*eval = 0;
				/* NOT YET COMPLETED */
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

		/* now the real maths */
		char *pre = NULL; /* string preceding the operator */
		char *pos = NULL; /* string succeding the operator */

		/* now allocate the variables pre & pos */
		if((pre=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *pre='\0';

		if((pos=(char *)(malloc(sizeof(char)*
		(strlen(str)+1))))==NULL) allocerr(); *pos='\0';

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
//		printf("\nax=%s",ax);
//		printf("\nbx=%s",bx);
		int evala=0;
		int evalb=0;
		int evalc=0;

		if(strisn(ax))
		{
			/*
			tmp = valstr(ax);
			if((cp=(char *)(realloc(ax,sizeof(char)*
			(strlen(tmp)+1))))!=NULL) { ax = cp; }
			else { allocerr(); }
			strcpy(ax,tmp);
			free(tmp);
			tmp = NULL;
			*/
			strcpy(ax,"");
			/* NOT YET COMPLETED */
		}
		if(strisn(bx))
		{
			/*
			tmp = valstr(bx);
			if((cp=(char *)(realloc(bx,sizeof(char)*
			(strlen(tmp)+1))))!=NULL) { bx = cp; }
			else { allocerr(); }
			strcpy(bx,tmp);
			free(tmp);
			tmp = NULL;
			*/
			strcpy(bx,"");
			/* NOT YET COMPLETED */
		}

		evala = atoi(ax);
		evalb = atoi(bx);
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
		if((cx=(char *)(malloc(sizeof(char)*
		((cxlen)+1))))==NULL) allocerr(); *cx='\0';

		itoa(evalc,cx,10);
//		printf("\ncx=%s\n",cx);
		printf("\nstep: %s",cx);

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

			/* now allocate the above string */
			if((pres=(char *)(malloc(sizeof(char)*
			(strlen(str)+1))))==NULL) allocerr(); *pres='\0';

			if((poss=(char *)(malloc(sizeof(char)*
			(strlen(str)+1))))==NULL) allocerr(); *poss='\0';

			if((mids=(char *)(malloc(sizeof(char)*
			(strlen(str)+1))))==NULL) allocerr(); *mids='\0';

			if((midx=(char *)(malloc(sizeof(char)*
			(strlen(str)+1))))==NULL) allocerr(); *midx='\0';

			if((newx=(char *)(malloc(sizeof(char)*
			(strlen(str)+1))))==NULL) allocerr(); *newx='\0';

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

void vals(const char *str, char *err, long *val)
{
	strcpy(err,"");  /* empty error string */
	printf(" {var}");
	*val = 999;
}

///////////////////////////////////
// Secondary Evaluation Functions
///////////////////////////////////

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
		else if(*str==34||*str==39)
		qt=0;
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
	/* invoked when memory allocation fails */
	printf("\nNot enough memory.\a\n");
	exit(1);
}
