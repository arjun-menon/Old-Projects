#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STR_MAX 1024

int main(int argc, char *argv[])
{
	char filename[STR_MAX];
	char err[STR_MAX+256];
	void c2html(char *filename, char *err);
	err[0] = filename[0] = '\0';

	if(argc<=1)
	{
		puts("========================================");
		puts(" << C SOURCE CODE TO HTML TRANSLATOR >> ");
		puts("========================================");
		puts(" This program converts a C source code  "); 
		puts(" file to an HTML page with line numbers.");
		puts(" This program was written by Arjun Menon");
		puts("----------------------------------------");
		puts("Please enter the name of the file to translate...");
		printf("\nFilename: ");
		gets(filename);
		c2html(filename,err);
		if(strlen(err))
		printf("\nError: %s\n",err);
		puts("\nProgram Terminated.");
		getchar();
		return 0;
	}
	else
	{
		return 0;
	}
}

void c2html(char *filename, char *err)
{
	if(!strlen(filename))
	{
		puts("\nPlease enter a filename.");
		return;
	}

	FILE *cfile,*hfile;
	cfile = hfile = NULL;

	char htmlfile[STR_MAX];
	strcpy(htmlfile,filename);
	strcat(htmlfile,".html");

	if((cfile = fopen(filename,"r"))==NULL)
	{
		sprintf(err,"Could not open file: %s",filename);
		return;
	}

	if((hfile = fopen(htmlfile,"w"))==NULL)
	{
		sprintf(err,"Could not create file: %s",htmlfile);
		return;
	}

	printf("\nTranslating %s ...\n\n",filename);

	register char move = 0;
	unsigned int strmax, strnow, lines;
	unsigned long int charcount = 0;
	strmax = strnow = lines = 0;

	for(;move!=EOF;strnow++, charcount++)
	{
		move = fgetc(cfile);
		if(move=='\n')
		{
			strmax = strnow>strmax ? strnow:strmax;
			strnow = 0;
			lines++;
		}
	}

	printf("No. of lines = %d\n",lines);
	printf("Longest line = %d\n",strmax);
	printf("No. of chars = %ld\n",charcount);

	if(charcount<=1)
	{
		strcpy(err,"Too few characters.");
		fclose(hfile);
		fclose(cfile);
		return;
	}

	fprintf(hfile,"<HTML><HEAD><TITLE>%s</TITLE></HEAD>\n",filename);
	fprintf(hfile,"<BODY><CENTER><H1>%s</H1></CENTER><HR>\n",filename);

	char *wstr = NULL;
	if((wstr = (char *) malloc
	(sizeof(char)*(strmax+1)))==NULL)
	{
		strcpy(err,"Out of memory.");
		fclose(hfile);
		fclose(cfile);
		return;
	}
	else *wstr = '\0';

	unsigned int lnum = 0, cnum = 0;
	cfile = fopen(filename,"r");

	fputs("<TABLE BORDER=0>\n",hfile);
	for(;fgets(wstr,strmax,cfile)!=NULL;lnum++)
	{
		fprintf(hfile,"<TR><TD VALIGN=TOP><CODE><B>%d</B> &nbsp; </CODE></TD><TD><CODE>",lnum);
		for(cnum=0;cnum<strlen(wstr);cnum++)
		{
		//	printf("%c",wstr[cnum]);
			if(wstr[cnum]=='\n')
			fprintf(hfile,"</CODE></TD></TR>\n");
			else if(wstr[cnum]=='<')
			fprintf(hfile,"&lt;");
			else if(wstr[cnum]=='>')
			fprintf(hfile,"&gt;");
			else
			fprintf(hfile,"%c",wstr[cnum]);
		}
	}

	fputs("</TABLE><HR NOSHADE><CODE>\n",hfile);
	fprintf(hfile,"No. of lines = %d<BR>\n",lines);
	fprintf(hfile,"Longest line = %d<BR>\n",strmax);
	fprintf(hfile,"No. of chars = %ld<BR>\n",charcount);
	fputs("</CODE></BODY></HTML>",hfile);
	puts("\nTranslation Complete.");
	printf("HTML File: %s stored.\n\n",htmlfile);
	free(wstr);
	fclose(hfile);
	fclose(cfile);
}
