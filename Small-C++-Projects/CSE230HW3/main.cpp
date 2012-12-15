/*
 *     CSE 230 HW 3
 *
 *  Main Program (LFRO)
 *
 *   --- main.cpp ---
 *
 *  Provides line number based file 
 *  reading services.
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

#include "common.h"

int main(int argc, char *argv[])
{
	LinearFileReader k;

	// function prototype
	void ErrorHandle(string err);

	if(argc <= 1)  // no commandline arguments
	{
		// prompt for file
		string input = "";
		cout<<"Enter a filename: ";
		cin>>input;

		// now open the file
		try  {
			k.LoadFile(input);
		}
		catch(string e) { ErrorHandle(e); }
	}
	else if(argc == 2) // with single file only
	{
		try {
			k.LoadFile(argv[1]);
		}
		catch(string e) { ErrorHandle(e); }
	}
	else if(argc==3) // with 2 files
	{
		try {
			k.LoadMultipleFiles(argv+1, 2);
		}
		catch(string e) { ErrorHandle(e); }
	}
	else // if(argc>3)
	{
		// Check for -n option
		bool option = false;
		if(!strcmp(argv[1], "-n")) {
			k.SetLineCount(atoi(argv[2]));
			option = true;
		}
		// Now load the files...
		try {
			if(option)
				k.LoadMultipleFiles(argv+3, argc-3);
			else
				k.LoadMultipleFiles(argv+1, argc-1);
		}
		catch(string e) { ErrorHandle(e); }
	}

	// Start UI...
	if(!k.IsEmpty())
	{
		// function prototype
		void StartUI(LinearFileReader &);
		StartUI(k);
		cout<<endl<<endl;
	}
	return 0;
}

void StartUI(LinearFileReader &obj)
{
	char input = 'N';
	do
	{
		switch(input)
		{
		case 'N':
		case 'P':
			system("clear");
			cout<<obj.DisplayFormattedLines(input);
			cout<<"\nPlease enter N to view next page, P for previous page and Q to quit: ";
			break;
		}
	}
	while((input = toupper(getchar()))!='Q');
}
void ErrorHandle(string err)
{
	cout<<err<<endl<<"Press Enter to continue...";
	fflush(stdin);
	getc(stdin);
}
