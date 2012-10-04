/*******************************************
  Name: Mathematical Expression Evaluator
  Author: Arjun Menon  |  Date: 29/08/05
*******************************************/

/* include headers*/
#include "stdio.h"
#include "string.h"

/* definitions */
#define NULL 0
#define _NULL ""
#define KB 1024
#define repeat(_n)	for(i=1;i<=_n;i++)
#define sigma(_i,_n)	for(i=_i;i<=_n;i++)

/* global functions	*/
int strrht(char *rht, const char *str, const int n);
int strlft(char *lft, const char *str, const int n);
int strmid(char *mid, const char *str, const int m, const int n);

/* global variables */

/* global constants */
extern const char _eval_version[]="EVAL 0.0.1";
extern const char _shell_arrow[] =">> ";
extern const char _shell_null[]="";
extern const char _shell_exit[]="exit";
extern const char _shell_error[]="Evaluation Error";

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
	auto char cmd[KB]=_NULL;
	int eval(const char *cmd);
	while(strcmp(cmd,_shell_exit))
	{
		printf("\n%s",_shell_arrow);
        gets(cmd);
     	if(!strcmp(cmd,_shell_null))
		continue;
		/* cmd should not be exit */
		if(strcmp(cmd,_shell_exit))
		/* check if eval return is 0 */
		if(eval(cmd)) 
		printf("\n%s\n",_shell_error);
	}
}

int strrht(char *rht, const char *str, const int n)
{
	int len = strlen(str);
	int i;
	repeat((len-n))
	str++;
	repeat(n)
	{
		if(*str==NULL)
		{
			*rht=NULL;
			return ((n-i)+1);
		}
		*rht=*str;
		rht++;
		str++;
	}
	*rht=NULL;
	return 0;
}

int strlft(char *lft, const char *str, const int n)
{
	int i;
	repeat(n)
	{
		if(*str==NULL)
		{
			*lft=NULL;
			return ((n-i)+1);
		}
		*lft=*str;
		lft++;
		str++;
	}
	*lft=NULL;
	return 0;
}

int strmid(char *mid, const char *str, const int m, const int n)
{
	int i;
	repeat(m)
	{
		if(*str==NULL)
		{
			*mid=NULL;
			return (((n+m)-i)+1);
		}
		str++;
	}
	repeat(n)
	{
		if(*str==NULL)
		{
			*mid=NULL;
			return ((n-i)+1);
		}
		*mid=*str;
		mid++;
		str++;
	}
	*mid=NULL;
	return 0;
}

int eval(const char *cmd)
{
	char s[KB];
	char a[KB];
	int n,m,i;
	if(!strcmp(cmd,"left"))
	{
		printf("\n%s:\n",cmd);
		printf("s? "); gets(a);
		printf("n? "); scanf("%d",&n);
		i=strlft(s,a,n);
		printf("\neval= %s",a);
		printf("\nleft= %s",s);
		printf("\nerror= %d\n",i);
		return 0;
	}
	if(!strcmp(cmd,"right"))
	{
		printf("\n%s:\n",cmd);
		printf("s? "); gets(a);
		printf("n? "); scanf("%d",&n);
		i=strrht(s,a,n);
		printf("\neval= %s",a);
		printf("\nright= %s",s);
		printf("\nerror= %d\n",i);
		return 0;
	}
	if(!strcmp(cmd,"middle"))
	{
		printf("\n%s:\n",cmd);
		printf("a? "); gets(a);
		printf("m? "); scanf("%d",&m);
		printf("n? "); scanf("%d",&n);
		i=strmid(s,a,m,n);
		printf("\neval= %s",a);
		printf("\nmiddle= %s",s);
		printf("\nerror= %d\n",i);
		return 0;
	}
	return 1;
}
