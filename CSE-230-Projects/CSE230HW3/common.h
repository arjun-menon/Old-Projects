/*
 *     CSE 230 HW 3
 *
 *  Common Include File
 *
 *   --- common.h ---
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

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cctype>
#include <vector>

using namespace std;

class LinearFileReader
{

private:
	vector <string> lines;
	unsigned int linecount;

public:
	LinearFileReader();
	LinearFileReader(const string &fname);
	LinearFileReader(const string &fname, unsigned int lcount);
	void LoadFile(const string &fname);
	void LoadMultipleFiles(char *fnames[], int fcount);
	unsigned int NumberOfLines();
	string GetLine(const unsigned int i);
	string GetFormattedLine(const unsigned int i);
	string GetFormattedLines(unsigned int i, unsigned int n);
	string DisplayFormattedLines(char mv);
	void SetLineCount(unsigned int n);
	bool IsEmpty();
	void ClearLines();
	~LinearFileReader();

	static void GeneratePaddedNumber(char *str, const unsigned int num, 
		const unsigned int len, const char padding_symbol);
};
