#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STR_MAX 1024

int main()
{
	char filename[STR_MAX];
	char err[STR_MAX+256];
	void c2html(const char *filename, char *err);
	err[0] = filename[0] = '\0';

	puts("===========================================");
	puts(" << SOURCE CODE TEXT TO HTML TRANSLATOR >> ");
	puts("===========================================");
	puts("  This program converts a source code text "); 
	puts("  file to an HTML page with line numbers.  ");
	puts("  This program was written by Arjun Menon. ");
	puts("-------------------------------------------");
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

inline void numstr(char *str, const unsigned int num, const unsigned int len)
{
	/*  this function generates line
	 *  numbers for the text file.
	 */

	unsigned int digits = 0;
	unsigned int _num = num;
	for(;_num;digits++) _num /= 10;
	unsigned int zeros = len-digits;
	if(zeros==len)
	zeros -= 1;
	for(;zeros;zeros--)
	*str++ = '0';
	itoa(num,str,10);
}
inline void numstr(char *str,
	   const unsigned int num,
       const unsigned int len);

void c2html(const char *filename, char *err)
{
	/* In this function the source code
	 * is tranlate to an HTML web page.
	 */

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

	printf("\nAnalyzing file: %s ...\n\n",filename);

	register char move = 0;
	unsigned int strmax, strnow, lines, pages;
	unsigned long int charcount = 0;
	strmax = strnow = lines = pages = 0;

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

	pages = lines/50;
	pages += lines%50 < 10 ? 0 : 1;

	printf("No. of pages = %d\n",pages);
	printf("No. of lines = %d\n",lines);
	printf("No. of chars = %ld\n",charcount);

	if(charcount<=1)
	{
		strcpy(err,"Too few characters.");
		fclose(hfile);
		fclose(cfile);
		return;
	}

	fprintf(hfile,"<HTML><HEAD><TITLE>%s</TITLE></HEAD>\n",filename);
	fprintf(hfile,"<BODY><CENTER><H1>%s</H1></CENTER><HR><CODE>\n",filename);

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

	unsigned int digits = 0;
	unsigned int _lines = lines;
	for(;_lines;digits++) _lines /= 10;

	char *nstr = NULL;
	if((nstr = (char *) malloc
	(sizeof(char)*(digits+1)))==NULL)
	{
		strcpy(err,"Out of memory.");
		fclose(hfile);
		fclose(cfile);
		return;
	}
	else *nstr = '\0';
	fclose(cfile);

	printf("\nTranslating file: %s ...\n",filename);

	unsigned int lnum = 0, cnum = 0;
	cfile = fopen(filename,"r");

	for(;fgets(wstr,strmax,cfile)!=NULL;lnum++)
	{
		numstr(nstr,lnum,digits);
		fprintf(hfile,"<B> %s </B> &nbsp;",nstr);
		for(cnum=0;cnum<strlen(wstr);cnum++)
		{
			if(wstr[cnum]=='\n')
			fprintf(hfile,"<BR>\n");
			else if(wstr[cnum]=='<')
			fprintf(hfile,"&lt;");
			else if(wstr[cnum]=='>')
			fprintf(hfile,"&gt;");
			else
			fprintf(hfile,"%c",wstr[cnum]);
		}
	}

	fputs("</CODE><HR NOSHADE><FONT FACE=Arial>\n",hfile);
	fprintf(hfile,"No. of pages = %d<BR>\n",pages);
	fprintf(hfile,"No. of lines = %d<BR>\n",lines);
	fprintf(hfile,"No. of chars = %ld<BR>\n",charcount);
	fputs("</FONT></BODY></HTML>",hfile);
	puts("\nTranslation Complete.");
	printf("HTML File: %s stored.\n",htmlfile);
	free(wstr);
	fclose(hfile);
	fclose(cfile);
}
