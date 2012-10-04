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

int eval(const char *cmd)
{
    printf("\n%s\n",cmd);
    return 0;
}
