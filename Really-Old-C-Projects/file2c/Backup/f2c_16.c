/**********************
  Name: File to C/C++
  Author: Arjun Menon      
***********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#define ERR_SZ 256
#define STR_SZ 1024
#define BUF_SZ 2048

int main()
{
    void f2c(const char *filename, const char *cfilename, char *err, char platform);
	char filename[STR_SZ], cfilename[STR_SZ], err[STR_SZ+ERR_SZ], platform;
	err[0] = filename[0] = cfilename[0] = platform = '\0';

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

    char file_exists(char *fname);    
    source_filename_entry:
	printf("\nSource Filename: ");
    fgets(filename,STR_SZ,stdin);
    filename[strlen(filename)-1]='\0';
    if(!file_exists(filename)) {
      puts("No such file exists");
      goto source_filename_entry;
    }
    
    new_filename_entry:
	printf("\nNew Filename: "); 
    fgets(cfilename,STR_SZ,stdin);
    cfilename[strlen(cfilename)-1]='\0';
    if(file_exists(cfilename)) {
      puts("Warning file already exists.");
      printf("Continue operation (y/n) ? ");
      char option_key = '\0';
      fflush(stdin);
      option_key = getchar();
      option_key = tolower(option_key);
      fflush(stdin);
      if(option_key!='y' || option_key!='Y')
        goto new_filename_entry;
    }    
    
    if(!strlen(cfilename)) strcpy(cfilename, filename);
    printf("\
\nAvailable Platforms...\
\nDOS / Linux / UNIX (d)\
\nWindows Executable (w)\
\nPlatform Option: ");
    fflush(stdin);
    platform = getchar();
    platform = tolower(platform);

    f2c(filename,cfilename,err,platform);
	if(strlen(err))
	printf("\nError: %s\n",err);
	puts("\nProgram Terminated.");
	fflush(stdin);
	getchar();
	return 0;
}

void f2c(const char *filename, const char *cfilename, char *err, char platform)
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
        
        printf("\nFormatting Hex(x), Dec(d), Octal(o), Character(c); press one key: ");
        format = getchar();
        format = tolower(format);
        if(format!='x'&&format!='d'&&format!='o'&&format!='c')
        format = 'x';
    }
    if(*cfilename=='-')
        cfilename++;
	
	FILE *infile = NULL;
	int bytes = 0;
	unsigned char buffer[BUF_SZ];
	
	if((infile = fopen(filename,"rb")) == NULL)
	{
		sprintf(err,"Could not open file '%s'",filename);
		fclose(infile);
		return;
    }
    
    long fsize = 0;
    while(1)
    {
        bytes = fread(buffer, sizeof(char), BUF_SZ, infile);
        
        if(bytes>0)
          fsize += bytes;
        else
          break;
    }
    fclose(infile);
    infile = fopen(filename,"rb");
    
    char newfile[STR_SZ+3];
    sprintf(newfile,"%s.c",cfilename);
    FILE *cfile = NULL;
    cfile = fopen(newfile,"w");
    
    if(platform == 'w')
    {
      fprintf(cfile,"\
/* File Generator for: %s\n\
   Platform: Microsoft Windows  */\n\n\
/* Code automatically generated by\n\
   Arjun's Programming Toolkit */\n\n\
#include <windows.h>\n\
#include <fcntl.h>\n\n\
// WARNING: DO NOT MODIFY THIS ARRAY !\n\
unsigned char data[%u]=\n{\n"
      ,cfilename,fsize);
    }
    else
    {
      fprintf(cfile,"\
/* File Generator for: %s\n\
   Platform: DOS/Linux/UNIX  */\n\n\
/* Code automatically generated by\n\
   Arjun's Programming Toolkit */\n\n\
#include <stdio.h>\n\
#include <fcntl.h>\n\n\
// WARNING: DO NOT MODIFY THIS ARRAY !\n\
unsigned char data[%u]=\n{\n"
      ,cfilename,fsize);
    }
    
    long count = 0;
    short i = 0, n = 0;

	while(1)
	{
        bytes = fread(buffer, sizeof(char), BUF_SZ, infile);

        if(bytes>0)
        {
            for(i=0; i<bytes; i++)
            {
                count++;
                if(count<fsize)
                {
                  switch(format) {
                  case 'c':
                       if(buffer[i]!='\n'&&buffer[i]!='\r'&&buffer[i]!='\0'
                       &&buffer[i]!='\''&&buffer[i]!='\"'&&buffer[i]!='\\')
                         fprintf(cfile,"'%c',",(unsigned char)(buffer[i]));
                       else
                         fprintf(cfile,"%u,",(unsigned char)(buffer[i]));
                       break;
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
    
    if(platform == 'w')
    {
      fprintf(cfile,"\n\
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)\n{\
\n\tchar str[255];\n\tif(!GetTempPath(255,str));\n\tstrcpy(str,\"C:\\\\\");\n\tstrcat(str,\"%s\");\
\n\tint file = open(str, O_CREAT|O_BINARY|O_WRONLY);\
\n\twrite(file, data, %u);\n\tclose(file);\n\
\n\tSTARTUPINFO si;\n\tPROCESS_INFORMATION pi;\n\
\n\tZeroMemory( &si, sizeof(si) );\n\tsi.cb = sizeof(si);\
\n\tZeroMemory( &pi, sizeof(pi) );\n\
\n\tCreateProcess( NULL, str, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);\n\
\n\tCloseHandle( pi.hProcess );\n\tCloseHandle( pi.hThread );\n\n\treturn 0;\n}\n"
      ,cfilename ,fsize, cfilename);
    }
    else
    {
      fprintf(cfile,"\n\
int main()\n{\
\n\tint file = open(\"%s\", O_CREAT|O_BINARY|O_WRONLY);\
\n\twrite(file, data, %u);\n\tclose(file);\n\n\treturn 0;\n}\n"
      ,cfilename,fsize);
    }
    
	fclose(cfile);
	fclose(infile);

	printf("\nFile '%s' successfully translated to '%s'\n",filename,newfile);
}

char file_exists(char *fname)
{
     if(*fname == '-')
       fname++;
     
     FILE *tmp;
     tmp = fopen(fname,"r");

     if(!tmp)
       return 0;
     else
       fclose(tmp);
     
     return 1;
}
