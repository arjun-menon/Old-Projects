/*
 *          CSE 230 HW 3
 *
 *   Functions for definitions for
 *   class Linear File Reader (LFR)
 *
 *        --- lfro.cpp ---
 *
 *  Provides a line number based 
 *  formatted textfile reader.
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

#include "common.h"

#define LFR LinearFileReader

LFR::LinearFileReader()
{
	linecount = 20;
}

LFR::LinearFileReader(const string &fname)
{
	LoadFile(fname);
	linecount = 20;
}

LFR::LinearFileReader(const string &fname, unsigned int lcount)
{
	LoadFile(fname);
	linecount = 20;
}

void LFR::LoadFile(const string &fname)
{
	ifstream f(fname.c_str());

	if(!f.is_open())
		throw (string)("Could not open the file \"" + fname + "\".\n");
	else
	{
		for(string s = ""; !f.eof(); lines.push_back((string)(s)))
			getline(f, s);
		f.close();
	}
}

void LFR::LoadMultipleFiles(char *fnames[], int fcount)
{
	bool erred = false;
	string errors = "";

	for(int i=0; i<fcount; i++)
	{
		try
		{
			LoadFile(fnames[i]);
		}
		catch(string err)
		{
			errors += err;
			erred = true;
		}
	}

	if(erred)
		throw errors;
}

unsigned int LFR::NumberOfLines()
{
	return (unsigned int)(lines.size());
}

string LFR::GetLine(const unsigned int i)
{
	return ( i < NumberOfLines() && i > 0 ) ? lines[i-1] : "";
}

string LFR::GetFormattedLine(const unsigned int i)
{
	char nol_str[12];
	sprintf(nol_str, "%u", NumberOfLines());
	
	unsigned int length = strlen(nol_str);
	nol_str[0] = '\0'; // empty the string
	GeneratePaddedNumber(nol_str, i, length, ' ');

	string s = nol_str;
	s += ": " + GetLine(i) + "\n";

	return s;
}

string LFR::GetFormattedLines(unsigned int i, unsigned int n)
{
	string s = "";

	while(n--)
		s += GetFormattedLine(i++);

	return s;
}

string LFR::DisplayFormattedLines(char mv)
{
	static unsigned int current_line = 1;

	string s = "";

	if(IsEmpty())
		return s;

	if(linecount > NumberOfLines())
		linecount = NumberOfLines();

	if(mv == 'N')
	{
		if(current_line > NumberOfLines())
			current_line = 1;

		if(current_line+linecount <= NumberOfLines())
		{
			s = GetFormattedLines(current_line, linecount);
			current_line += linecount;
		}
		else
		{
			s = GetFormattedLines(current_line, NumberOfLines()-current_line+1);
			current_line = NumberOfLines()+1;
		}
	}
	else if(mv == 'P')
	{
		if(current_line > linecount*2)
		{
			if(current_line <= ((NumberOfLines()/linecount)*linecount+1) )
				current_line -= linecount * 2;
			else
				current_line = (NumberOfLines()/linecount-1) * linecount + 1;
		}
		else // (current_line <= linecount*2)
		{
			if(current_line <= linecount+1)
				current_line = ((NumberOfLines()/linecount)*linecount)+1;
			else
				current_line = 1;
		}
		s = DisplayFormattedLines('N');
	}

	return s;
}

void LFR::SetLineCount(unsigned int n)
{
	linecount = n;
}

bool LFR::IsEmpty()
{
	return (NumberOfLines() == 0);
}

void LFR::ClearLines()
{
	lines.clear();
}

LFR::~LinearFileReader()
{
	ClearLines();
}

void LFR::GeneratePaddedNumber(char *str, const unsigned int num, const unsigned int len, const char padding_symbol)
{
	// this function generates a number string.
	// it adds extra spaces in front of the 
	// number and ensures thar the total 
	// number of characters = len.

	unsigned int digits = 0;
	unsigned int _num = num;
	for(;_num;digits++)
		_num /= 10;

	unsigned int zeros = len-digits;
	if(zeros==len)
		zeros -= 1;
	for(;zeros;zeros--)
		*str++=padding_symbol;

	if(!digits)
		str[++digits-1]='0';
	str[digits]='\0'; /* end string */

	for(_num=num;_num;_num/=10)
	str[digits---1] = (_num%10)+'0';
}
