/**********************
  Name: File to C/C++
  Author: Arjun Menon      
***********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#define STR_MAX 1024

int main()
{
	char filename[STR_MAX];
	char err[STR_MAX+256];
	void f2hex(const char *filename, char *err);
	err[0] = filename[0] = '\0';

	puts("===========================================");
	puts(" <<  FILE  TO  C  LANGAUGE  TRANSLATOR  >> ");
	puts("===========================================");
	puts("  This program converts any ordinary file  "); 
	puts("  into a C program that generates the file ");
	puts("  from an array made of the file's bytes.  ");
	puts("  This program was written by Arjun Menon. ");
	puts("-------------------------------------------");
	puts("Please enter the name of the file to translate...");
	printf("\nFilename: ");

	gets(filename);
	f2hex(filename,err);
	if(strlen(err))
	printf("\nError: %s\n",err);
	puts("\nProgram Terminated.");
	getchar();
	return 0;
}

void f2hex(const char *filename, char *err)
{
	if(!strlen(filename))
	{
		puts("\nPlease enter a filename.");
		return;
	}
	
	char buffer[512];
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
    sprintf(newfile,"%s.c",filename);
    FILE *cfile = NULL;
    
    cfile = fopen(newfile,"w");   
    fprintf(cfile,"/* %s */\n\n\
#include <stdio.h>\n\
#include <fcntl.h>\n\n\
unsigned char data[%u]=\n{\n"
    ,filename,fsize);
    
    long count = 0;
    short i = 0;

	while(1)
	{
        bytes = read(infile, buffer, 512);

        if(bytes>0)
        {
            for(i=0; i<bytes; i++)
            {
                count++;
                if(count<fsize)
                fprintf(cfile,"%u,\n",buffer[i]);
                else if(count==fsize)
                fprintf(cfile,"%u\n};\n",buffer[i]);
            }
        }
        else break;
    }
    fprintf(cfile,"\n\
int main()\n{\
\n\tint file = open(\"%s\", O_CREAT|O_BINARY|O_WRONLY);\
\n\twrite(file, data, %u);\n\tclose(file);\n\n\treturn 0;\n}\n"
    ,filename,fsize);
    
	fclose(cfile);
	close(infile);

	printf("\nFile '%s' successfully translated to '%s'\n",filename,newfile);
}
