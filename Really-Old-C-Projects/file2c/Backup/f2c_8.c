/**********************
  Name: File to C/C++
  Author: Arjun Menon      
***********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#define STR_MAX 1024

int main()
{
	char filename[STR_MAX], cfilename[STR_MAX], err[STR_MAX+256];
	void f2c(const char *filename, const char *cfilename, char *err);
	err[0] = filename[0] = cfilename[0] = '\0';

	puts("===========================================");
	puts(" <<  FILE  TO  C  LANGAUGE  TRANSLATOR  >> ");
	puts("===========================================");
	puts("  This program converts any ordinary file  "); 
	puts("  into a C program that generates the file ");
	puts("  from an array made of the file's bytes.  ");
	puts("  This program was written by Arjun Menon. ");
	puts("-------------------------------------------");
	puts("   U S A G E    I N S T R U C T I O N S :  ");
	puts(" Please enter the name of the file to translate");
    puts(" and target filename. To use same target filename, ");
    puts(" leave blank; otherwise enter the target name...");
    puts(" By default, the array is written in hexadecimal ");
    puts(" with 16 numbers on one line of code, to modify ");
    puts(" these settings precede the source filename with ");
    puts(" a single hyphen '-' character.");
	puts("-------------------------------------------");
    
	printf("\nSource Filename: ");
    fgets(filename,STR_MAX,stdin);
    filename[strlen(filename)-1]='\0';
	printf("\nNew Filename: "); 
    fgets(cfilename,STR_MAX,stdin);
    cfilename[strlen(cfilename)-1]='\0';
    if(!strlen(cfilename)) strcpy(cfilename, filename);
    	
	f2c(filename,cfilename,err);
	if(strlen(err))
	printf("\nError: %s\n",err);
	puts("\nProgram Terminated.");
	fflush(stdin);
	getchar();
	return 0;
}

void f2c(const char *filename, const char *cfilename, char *err)
{
	if(!strlen(filename))
	{
		puts("\nPlease enter a filename.");
		return;
	}

	unsigned short int cells = 16;
	char format = 'x';
	
	if(*filename=='-')
	{
        filename++;
        printf("\nHow many number per line (1 - 100): ");
        scanf("%u",&cells);
        if(cells<1||cells>100)
          cells = 16;
        fflush(stdin);
        
        printf("\nFormatting Hex(x), Dec(d), Octal(o); press one key: ");
        format = getchar();
        format = tolower(format);
        if(format!='x'&&format!='d'&&format!='o')
        format = 'x';
    }
    if(*cfilename=='-')
        cfilename++;
	
	unsigned char buffer[512];
	int infile = 0, bytes = 0;
	
	if((infile = open(filename,O_RDONLY|O_BINARY)) == -1)
	{
		sprintf(err,"Could not open file '%s'",filename);
		close(infile);
		return;
    }
    
    long fsize = 0;
    while(1)
    {
        bytes = read(infile, buffer, 512);
        
        if(bytes>0)
          fsize += bytes;
        else
          break;
    }
    close(infile);
    infile = open(filename,O_RDONLY|O_BINARY);
    
    char newfile[STR_MAX+5];
    sprintf(newfile,"%s.c",cfilename);
    FILE *cfile = NULL;
    
    cfile = fopen(newfile,"w");   
    fprintf(cfile,"/* %s */\n\n\
#include <stdio.h>\n\
#include <fcntl.h>\n\n\
unsigned char data[%u]=\n{\n"
    ,cfilename,fsize);
    
    long count = 0;
    short i = 0, n = 0;

	while(1)
	{
        bytes = read(infile, buffer, 512);

        if(bytes>0)
        {
            for(i=0; i<bytes; i++)
            {
                count++;
                if(count<fsize)
                {
                  switch(format) {
                  case 'd':
                       fprintf(cfile,"%u,",(unsigned char)(buffer[i]));
                       break;
                  case 'o':
                       fprintf(cfile,"0%o,",(unsigned char)(buffer[i]));
                       break;
                  default:
                       fprintf(cfile,"0x%x,",(unsigned char)(buffer[i]));
                       break;
                  } n++;
                  if(n>=cells) {
                  fprintf(cfile,"\n");
                  n = 0; }
                }
                else if(count==fsize)
                  switch(format) {
                  case 'd':
                       fprintf(cfile,"%u\n};\n",(unsigned char)(buffer[i]));
                       break;
                  case 'o':
                       fprintf(cfile,"0%o\n};\n",(unsigned char)(buffer[i]));
                       break;
                  default:
                       fprintf(cfile,"0x%x\n};\n",(unsigned char)(buffer[i]));
                       break;
                  }
            }
        }
        else break;
    }
    
    fprintf(cfile,"\n\
int main()\n{\
\n\tint file = open(\"%s\", O_CREAT|O_BINARY|O_WRONLY);\
\n\twrite(file, data, %u);\n\tclose(file);\n\n\treturn 0;\n}\n"
    ,cfilename,fsize);
    
	fclose(cfile);
	close(infile);

	printf("\nFile '%s' successfully translated to '%s'\n",filename,newfile);
}
