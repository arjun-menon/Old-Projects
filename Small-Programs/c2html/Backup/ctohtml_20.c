/***************************************
  C SOURCE CODE TO HTML PAGE TRANSLATOR
========================================
  <----- Written By Arjun Menon ----->
****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STR_MAX 1024
#define COL_MAX 32

struct colors
{
	char chr[COL_MAX];
	char text[COL_MAX];
	char string[COL_MAX];
	char comment[COL_MAX];
	char keyword[COL_MAX];
	char numbers[COL_MAX];
};
void string(char *str);

int main(int argc, char *argv[])
{
	/* function declarations */
	void default_colors(struct colors *c);
	void c2html(const char *filename, char *err, struct colors *c);

	/* varaible declarations */
	char filename[STR_MAX];
	char err[STR_MAX+256];
	err[0] = filename[0] = '\0';
	struct colors c;
	default_colors(&c);

	/* user interface */
	puts("===========================================");
	puts(" << SOURCE CODE TEXT TO HTML TRANSLATOR >> ");
	puts("===========================================");
	puts("  This program converts C source code text "); 
	puts("  file to an HTML page with line numbers.  ");
	puts("  This program was written by Arjun Menon. ");
	puts("-------------------------------------------");
	puts("  To modify the default color settings,    ");
	puts("  precede the filename with : colon symbol ");
	puts("  for monochrome, or ? for user-defined.   ");
	puts("===========================================");

	if(argc<=1)  /* the are no arguments */
	{
		puts("Please enter the name of the file to translate...");
		printf("\nFilename: ");
		fgets(filename,STR_MAX,stdin);
		string(filename);
	}
	else  /* there are arguments */
	{ /* only the first argument is read */
		strcpy(filename,argv[1]);
		printf("\nFilenaeme: %s\n",filename);
	}

	/* now translate the file */
	c2html(filename,err,&c);
	/* check for errors */
	if(strlen(err))
	printf("\nError: %s\n",err);
	/* bye bye */
	printf("\nProgram Terminated.");
    getc(stdin);
	return 0;
}

void default_colors(struct colors *c)
{
	/* set default colors for C elements */
	strcpy(c->chr, "magenta");
	strcpy(c->text, "black");
	strcpy(c->string, "red");
	strcpy(c->comment, "green");
	strcpy(c->keyword, "blue");
	strcpy(c->numbers, "purple");
}

void userdef_colors(struct colors *c)
{
	/* input colors from the user */
	printf("\n");
	printf("Color of text: ");
	fgets(c->text,COL_MAX,stdin);
	fflush(stdin);
	printf("Color of char: ");
	fgets(c->chr,COL_MAX,stdin);
	fflush(stdin);
	printf("Color of string: ");
	fgets(c->string,COL_MAX,stdin);
	fflush(stdin);
	printf("Color of comment: ");
	fgets(c->comment,COL_MAX,stdin);
	fflush(stdin);
	printf("Color of keywords: ");
	fgets(c->keyword,COL_MAX,stdin);
	fflush(stdin);
	printf("Color of numbers: ");
	fgets(c->numbers,COL_MAX,stdin);
	fflush(stdin);
	/* convert to string */
	void string(char *str);
	string(c->chr);
	string(c->text);
	string(c->string);
	string(c->comment);
	string(c->keyword);
	string(c->numbers);
}

inline void numstr(char *str, const unsigned int num, const unsigned int len)
{
	/* this function generates line
	 numbers for the text file. */

	unsigned int digits = 0;
	unsigned int _num = num;
	for(;_num;digits++)
		_num /= 10;

	unsigned int zeros = len-digits;
	if(zeros==len)
		zeros -= 1;
	for(;zeros;zeros--)
		*str++='0';

	if(!digits)
		str[++digits-1]='0';
	str[digits]='\0'; /* end string */

	for(_num=num;_num;_num/=10)
	str[digits---1] = (_num%10)+'0';
}
inline void numstr(char *str,
	   const unsigned int num,
       const unsigned int len);

inline char * clones(const char *str)
{
	/* allocates a copy of string *str & returns it. */

	const int len = strlen(str) + 1;
	char *strc = (char *) malloc(len);

	if(strc==NULL)
		return 0;
	strcpy(strc,str);
	return strc;
}

inline char * allocs(int len)
{
	/* allocates an empty string 
	   of length (int32 len). */

	char *str = (char *) malloc(len+1);
	if(str==NULL) return 0;
	*str = '\0';
	return str;
}

int strmid(char *str, const char *src, const unsigned int m, const unsigned int n)
{
	// strrht can be used to extract a part of a string.
	// it takes upto 'n' characters that are encountered
	// after the first 'm' set of characters and
	// stores them in *str. if successfull, returns 0.
	// Note: if 'n' or  'm' exceeds the string length of
	// *str, then the return will be no.of exceed chars.

	unsigned int i;
	/* main loop counter */
	for(i=1;i<=m;i++)
	{
		if(*src=='\0')
			/* in case of premature string termination */
		{
			*str='\0';
			  /* calculates how bad the error was */
			return (((n+m)-i)+1);
		}
		  /* to move pointer to m blocks forward */
		src++;
	}

	for(i=1;i<=n;i++)
	{
		if(*src=='\0')  
			/* in case of premature string termination */
		{
			*str='\0';
			  /* calculates how bad the error was */
			return ((n-i)+1);
		}
		  /* here the new string is made */
		*str=*src;
		str++;
		src++;
	}

	/* set termination point for string */
	*str='\0';
	return 0;
}
int strmid(char *str, const char *src, const unsigned int m, const unsigned int n);

void strbluff(char *str, const char *bluff, const unsigned int n);
void strnull(char *str);

void c2html(const char *filename, char *err, struct colors *c)
{
	/* <- core function -> here the C source 
	   code is tranlated to an HTML web page. */

	struct /* common bits */
	{
		unsigned char color : 1;  /* color the code */
		unsigned char qt1 : 1;  /* double quotes */
		unsigned char qt2 : 1;  /* single quotes */
		unsigned char cm1 : 1;  /* multi-line comments */
		unsigned char cm2 : 1;  /* single-line comments */
		unsigned char num : 1;  /* numerical constants */
		unsigned char pre : 1;  /* preprocessors */
		unsigned char cont : 1;  /* preprocessors */
	}
	bits = { 1, 0, 0, 0, 0, 0, 0, 0 };

	if(!strlen(filename))
	{
		puts("\nPlease enter a filename.");
		return;
	}

	if(*filename==':')
	{
		bits.color = 0;
		filename++;
	}
	else if(*filename=='?')
	{
		userdef_colors(c);
		filename++;
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

	unsigned short int line_length = 0;
	char input_str[STR_MAX];
	printf("\nHow many letters per line? ");
	fgets(input_str,STR_MAX,stdin);
	string(input_str);
	line_length = atoi(input_str);
	if(line_length <= 1)
		line_length = 80;

	printf("\nAnalyzing file: %s ...\n\n",filename);

	register char move = 0;
	unsigned int strmax, strnow, lines, pages;
	unsigned long int charcount = 0;
	unsigned long int letters = 0;
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
		if(move<128 && move!='\n' && move!= '\r')
			letters++;
	}

	pages = lines/50;
	pages += lines%50 < 10 ? 0 : 1;

	printf("No. of pages = %d\n",pages);
	printf("No. of lines = %d\n",lines);
	printf("No. of chars = %ld\n",charcount);
	printf("No. of letters = %ld\n",letters);
	
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

	printf("\nTranslating file: %s ...    ",filename);

	fprintf(hfile,"<HTML><HEAD><TITLE>%s</TITLE></HEAD>\n",filename);
	fprintf(hfile,"<BODY TEXT=%s BGCOLOR=white><CENTER><H1>%s</H1></CENTER><HR><CODE>\n",c->text,filename);

	char *wstr = NULL, *wwstr = NULL;
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

	unsigned int lnum = 0; 
	unsigned register int cnum = 0;
	cfile = fopen(filename,"r");

	/* C keywords */
	char keys[33][9] =
	{ 
		"auto","break","case","char",
		"const","continue","default",
		"double","do","else","enum",
		"extern","float","for","goto",
		"if","int","long","register",
		"return","short","signed",
		"sizeof","static","struct",
		"switch","typedef","union",
		"unsigned","void","volatile",
		"while", "inline"
	};

	for(bits.cont=0;fgets(wstr,strmax,cfile)!=NULL;lnum++)
	{
		unsigned int remaining = 0;
		unsigned int split_offset = 0;

		remainder:

		/* show translation progress */
		char prog[4] = "00%";
		double progn = (lnum*100)/lines;

		if(progn<100)
		numstr(prog,(int)progn,progn<10?1:2);
		strcat(prog,"%");

		if(strlen(prog)==2)
		printf("\b\b%s",prog);


		if(strlen(prog)==3)
		printf("\b\b\b%s",prog);
		
		/* insert line numbers */
		short int numlength = 0;
		if(remaining)
			goto so_go_on;

		numstr(nstr,lnum,digits);
		numlength = strlen(nstr);
		fprintf(hfile,"<B>%s</B> &nbsp;",nstr);

		so_go_on:

		/* check if the line has been commented off */
		if(bits.cm1&&lnum)
		fprintf(hfile,"<FONT COLOR=%s>",c->comment);

		/* reset the booleans in bits */
		if(bits.cont && bits.pre)
			fprintf(hfile,"<B>");
		else
			bits.pre = 0;
		if(bits.cont && bits.qt1)
			fprintf(hfile,"<FONT COLOR=%s>",c->string);
		else
			bits.qt1 = 0;
		bits.qt2 = 0;
		bits.cm2 = 0;
		bits.num = 0;

		unsigned int line_len = 0;
		line_len = line_length - numlength;

		if(remaining)
		{
			numstr(nstr,lnum,digits);
			numlength = strlen(nstr);
			strnull(nstr);
			strbluff(nstr,"&nbsp;" ,numlength);
			fprintf(hfile,"%s &nbsp;",nstr);

			char *t = allocs(line_len+1);
			strmid(t,wwstr,split_offset,line_len);
			strcpy(wstr,t);
			split_offset += strlen(wstr);
			remaining -= strlen(wstr);
		}

		if(strlen(wstr)>line_len && remaining <= 0)
		{
			split_offset = 0;
			char *t = allocs(line_len+1);
			strmid(t,wstr,split_offset,line_len);
			wwstr = clones(wstr);
			strcpy(wstr,t);

			split_offset += strlen(wstr);
			remaining = strlen(wwstr)-line_len;
		}

		for(cnum=0;cnum<strlen(wstr);cnum++)
		{
			/* color up the keywords */
			int keyw;
			for(keyw=0;keyw<33;keyw++)
			if(!strncmp(&wstr[cnum],keys[keyw],
				strlen(keys[keyw]))&&!(
			   wstr[cnum-1]>='A'&&
			   wstr[cnum-1]<='Z')&&!(
			   wstr[cnum-1]>='a'&&
			   wstr[cnum-1]<='z')&&!(
			   wstr[cnum-1]>='0'&&
			   wstr[cnum-1]<='9')&&
			   wstr[cnum-1]!='_'
               &&(!bits.cm1)
               &&(!bits.cm2)
               &&(!bits.qt1)
               &&(!bits.qt2)
               &&bits.color)
			{
				if(!(wstr[cnum+strlen(keys[keyw])]>='A'&&
			         wstr[cnum+strlen(keys[keyw])]<='Z')&&!(
			         wstr[cnum+strlen(keys[keyw])]>='a'&&
			         wstr[cnum+strlen(keys[keyw])]<='z')&&!(
					 wstr[cnum+strlen(keys[keyw])]>='0'&&
			         wstr[cnum+strlen(keys[keyw])]<='9')&&
			         wstr[cnum+strlen(keys[keyw])]!='_') {
				fprintf(hfile,"<FONT COLOR=%s>%s</FONT>",c->keyword,keys[keyw]);
				cnum += strlen(keys[keyw]);
				}
			}

			/* color up the comments */
			if(!strncmp(&wstr[cnum],"/*",2)
			   &&!bits.qt1
			   &&!bits.qt2
			   &&!bits.cm2
			   &&bits.color)
			{
				fprintf(hfile,"<FONT COLOR=%s>/",c->comment);
				bits.cm1 = 1;
			}

			else if(!strncmp(&wstr[cnum],"*/",2)
				    &&!bits.qt1
					&&!bits.qt2
				    &&!bits.cm2
					&&(bits.cm1)
					&&bits.color)
			{
				fprintf(hfile,"*/</FONT>");
				cnum++;
				bits.cm1 = 0;
			}

			else if((!strncmp(&wstr[cnum],"//",2))
				     &&!bits.qt1
					 &&!bits.qt2
				     &&!bits.cm2
					 &&!bits.cm1
					 &&bits.color)
			{
				fprintf(hfile,"<FONT COLOR=%s>/",c->comment);
				bits.cm2 = 1;
			}

			/* color up the strings */
			else if(wstr[cnum]==34
				    &&(!bits.cm2)
					&&(!bits.cm1)
					&&(!bits.pre)
					&&bits.color)
			{
				if(wstr[cnum-1]!='\\') {
				if(!bits.qt1)
				{
					fprintf(hfile,"<FONT COLOR=%s>%c",c->string,34);
					bits.qt1 = 1;
				}
				else
				{
					fprintf(hfile,"%c</FONT>",34);
					bits.qt1 = 0;
				} }
			}

			/* color up the chars */
			else if(wstr[cnum]==39
					&&(!bits.qt1)
				    &&(!bits.cm2)
					&&(!bits.cm1)
					&&(!bits.pre)
					&&bits.color)
			{
				if(wstr[cnum-1]!='\\') {
				if(!bits.qt2)
				{
					fprintf(hfile,"<FONT COLOR=%s>%c",c->chr,39);
					bits.qt2 = 1;
				}
				else
				{
					fprintf(hfile,"%c</FONT>",39);
					bits.qt2 = 0;
				} }
			}

			/* color up the numbers */
			else if(wstr[cnum]>='0'&&
				    wstr[cnum]<='9'&&!(
					wstr[cnum-1]>'A'&&
					wstr[cnum-1]<'Z')&&!(
					wstr[cnum-1]>'a'&&
					wstr[cnum-1]<'z')&&
					wstr[cnum-1]!='_'
				    &&(!bits.cm2)
					&&(!bits.cm1)
					&&(!bits.qt1)
					&&(!bits.qt2)
					&&(!bits.num)
					&&(!bits.pre)
					&&bits.color)
			{
				fprintf(hfile,"<FONT COLOR=%s>%c",c->numbers,wstr[cnum]);
				bits.num = 1;
			}

			else if(bits.num /* to end number colouring */
				    &&((wstr[cnum]<'0')
				    ||(wstr[cnum]>'9')))
			{
			    fprintf(hfile,"</FONT>%c",wstr[cnum]);
				bits.num = 0;
			}

			/* color up the preprocessors */
			else if(wstr[cnum]=='#'
				    &&(!bits.cm2)
					&&(!bits.cm1)
					&&(!bits.qt1)
					&&(!bits.qt2)
					&&(!bits.pre)
					&&(bits.color))
			{
				fprintf(hfile,"<B>#");
				bits.pre = 1;
			}

			/* replace the C symbols
			 with their HTML equivalent */
			else if(wstr[cnum-1]==' '&&wstr[cnum]==' ')
			fprintf(hfile,"&nbsp;");
			else if(wstr[cnum]=='\t')
			fprintf(hfile,"&nbsp; ");
			else if(wstr[cnum]=='<')
			fprintf(hfile,"&lt;");
			else if(wstr[cnum]=='>')
			fprintf(hfile,"&gt;");
			else if(wstr[cnum]=='\n') {
				fprintf(hfile,"</FONT>");
				if(wstr[cnum-1]=='\\')
					bits.cont = 1;
				else
					bits.cont = 0;
			break;
			} else
			fprintf(hfile,"%c",wstr[cnum]);
		}

		if(bits.pre)
		fprintf(hfile,"</B>");

		/* end font tag */
		if(bits.cm1||
		   bits.cm2||
		   bits.qt1||
		   bits.qt2||
		   bits.num)
		fprintf(hfile,"</FONT>");

		/* end the line */
		fprintf(hfile,"<BR>\n");

		if(remaining)
			goto remainder;
	}

	fputs("</CODE><HR NOSHADE><FONT FACE=Arial>\n",hfile);
	fprintf(hfile,"No. of pages = %d<BR>\n",pages);
	fprintf(hfile,"No. of lines = %d<BR>\n",lines);
	fprintf(hfile,"No. of chars = %ld<BR>\n",charcount);
	fprintf(hfile,"No. of letters = %ld\n",letters);

	fputs("</FONT></BODY></HTML>",hfile);
	puts("\b\b\b100%\n\nTranslation Complete.");
	printf("HTML File: %s stored.\n",htmlfile);

	free(wstr);
	fclose(hfile);
	fclose(cfile);
}

void string(char *str)
{
	/* remove a new-line char. 
	from strings if present. */
	short int i;
	for(i=0;str[i]!='\0';i++)
	{
		if(str[i]=='\n')
		{ 
			str[i]='\0';
			break;
		}
	}
}

inline void strnull(char *str)
{
	*str = '\0';
}

void strbluff(char *str, const char *bluff, const unsigned int n)
{
	// fill up a string with a another string 'n' times.

	for(register unsigned int i=0; i<n; i++)
		strcat(str, bluff);
}