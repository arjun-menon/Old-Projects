#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STR_MAX 1024

/* colors for C elements */
char c_text[] = "black";
char c_char[] = "red";
char c_string[] = "red";
char c_comment[] = "green";
char c_keyword[] = "blue";
char c_numbers[] = "magenta";

int main()
{
	char filename[STR_MAX];
	char err[STR_MAX+256];
	void c2html(const char *filename, char *err);
	err[0] = filename[0] = '\0';

	puts("===========================================");
	puts(" << SOURCE CODE TEXT TO HTML TRANSLATOR >> ");
	puts("===========================================");
	puts("  This program converts C source code text "); 
	puts("  file to an HTML page with line numbers.  ");
	puts("  This program was written by Arjun Menon. ");
	puts("-------------------------------------------");
	puts("Please enter the name of the file to translate...");
	printf("\nFilename: ");
	gets(filename);

	/* now translate the file */
	c2html(filename,err);
	if(strlen(err))
	printf("\nError: %s\n",err);
	puts("\nProgram Terminated.");
	getchar();
	return 0;
}

inline void numstr(char *str, const unsigned int num, const unsigned int len)
{
	/* this function generates line
	   numbers for the text file. */

	unsigned int digits = 0;
	unsigned int _num = num;
	for(;_num;digits++) _num /= 10;
	unsigned int zeros = len-digits;
	if(zeros==len) zeros -= 1;
	for(;zeros;zeros--) *str++ = '0';
	itoa(num,str,10); /* stdlib.h */
}
inline void numstr(char *str,
	   const unsigned int num,
       const unsigned int len);

void c2html(const char *filename, char *err)
{
	/* <- core function -> here the C source 
	   code is tranlated to an HTML web page. */

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
	
	printf("\nTranslating file: %s ...\n",filename);

	if(charcount<=1)
	{
		strcpy(err,"Too few characters.");
		fclose(cfile);
		return;
	}

	if((hfile = fopen(htmlfile,"w"))==NULL)
	{
		sprintf(err,"Could not create file: %s",htmlfile);
		fclose(cfile);
		return;
	}

	fprintf(hfile,"<HTML><HEAD><TITLE>%s</TITLE></HEAD>\n",filename);
	fprintf(hfile,"<BODY TEXT=%s BGCOLOR=white><CENTER><H1>%s</H1></CENTER><HR><CODE>\n",c_text,filename);

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
		free(wstr);
		fclose(hfile);
		fclose(cfile);
		return;
	}
	else *nstr = '\0';
	fclose(cfile);

	unsigned int lnum = 0, cnum = 0;
	cfile = fopen(filename,"r");

	struct /* common bits */
	{
		unsigned char color : 1;  /* color the code */
		unsigned char qt1 : 1;  /* double quotes */
		unsigned char qt2 : 1;  /* single quotes */
		unsigned char cm1 : 1;  /* multi-line comments */
		unsigned char cm2 : 1;  /* single-line comments */
		unsigned char num : 1;  /* numerical constants */
	}
	bits = { 1, 0, 0, 0, 0, 0 };
	
	/* C keywords */
	char keys[][8] = {"if","else","for"};

	for(;fgets(wstr,strmax,cfile)!=NULL;lnum++)
	{
		/* set some bits to zero */
		bits.qt1 = 0;
		bits.qt2 = 0;
		bits.cm2 = 0;
		bits.num = 0;

		/* insert line numbers */
		numstr(nstr,lnum,digits);
		fprintf(hfile,"<B>%s</B> &nbsp;",nstr);

		/* check if the line has been commented off */
		if(bits.cm1&&lnum)
		fprintf(hfile,"<FONT COLOR=%s>",c_comment);

		for(cnum=0;cnum<strlen(wstr);cnum++)
		{
			/* color up the keywords */
			int keyw;
			for(keyw=0;keyw<3;keyw++)
			if(!strncmp(&wstr[cnum],keys[keyw],strlen(keys[keyw]))
               &&(!bits.cm1)
               &&(!bits.cm2)
               &&(!bits.qt1)
               &&(!bits.qt2)
               &&bits.color)
			{
				fprintf(hfile,"<FONT COLOR=%s>%s</FONT>",c_keyword,keys[keyw]);
				cnum += strlen(keys[keyw]);
			}

			/* color up the comments */
			if(!strncmp(&wstr[cnum],"/*",2)
				&&!bits.cm2&&bits.color)
			{
				fprintf(hfile,"<FONT COLOR=%s>/",c_comment);
				bits.cm1 = 1;
			}
			else if(!strncmp(&wstr[cnum],"*/",2)&&!bits.cm2)
			{
				fprintf(hfile,"*/</FONT>");
				cnum += 2;
				bits.cm1 = 0;
			}
			else if((!strncmp(&wstr[cnum],"//",2))
				     &&(!bits.cm2)
					 &&(!bits.cm1)
					 &&bits.color)
			{
				fprintf(hfile,"<FONT COLOR=%s>/",c_comment);
				bits.cm2 = 1;
			}
			/* color up the strings */
			else if(wstr[cnum]==34
				    &&(!bits.cm2)
					&&(!bits.cm1)
					&&bits.color)
			{
				if(!bits.qt1)
				{
					fprintf(hfile,"<FONT COLOR=%s>%c",c_string,34);
					bits.qt1 = 1;
				}
				else
				{
					fprintf(hfile,"%c</FONT>",34);
					bits.qt1 = 0;
				}				
			}
			/* color up the chars */
			else if(wstr[cnum]==39
				    &&(!bits.cm2)
					&&(!bits.cm1)
					&&bits.color)
			{
				if(!bits.qt2)
				{
					fprintf(hfile,"<FONT COLOR=%s>%c",c_char,39);
					bits.qt2 = 1;
				}
				else
				{
					fprintf(hfile,"%c</FONT>",39);
					bits.qt2 = 0;
				}				
			}
			/* color up the numbers */
			else if(wstr[cnum]>='0'&&
				    wstr[cnum]<='9'
				    &&(!bits.cm2)
					&&(!bits.cm1)
					&&(!bits.qt1)
					&&(!bits.qt2)
					&&!bits.num
					&&bits.color)
			{
				fprintf(hfile,"<FONT COLOR=%s>%c",c_numbers,wstr[cnum]);
				bits.num = 1;
			}
			else if(bits.num /* to end number colouring */
				    &&((wstr[cnum]<'0')
				    ||(wstr[cnum]>'9')))
			{
				fprintf(hfile,"</FONT>%c",wstr[cnum]);
				bits.num = 0;
			}

			/* replace the C symbols
			 with their HTML equivalent */
			else if(wstr[cnum]=='\t')
			fprintf(hfile," &nbsp; ");
			else if(wstr[cnum]=='<')
			fprintf(hfile,"&lt;");
			else if(wstr[cnum]=='>')
			fprintf(hfile,"&gt;");
			else if(wstr[cnum]=='\n')
			break;
			else
			fprintf(hfile,"%c",wstr[cnum]);
		}
		/* end font tag */
		if(bits.cm1||
		   bits.cm2||
		   bits.qt1||
		   bits.qt2||
		   bits.num)
		fprintf(hfile,"</FONT>");
		/* end the line */
		fprintf(hfile,"<BR>\n");
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
