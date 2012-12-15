/*
 *     CSE 230 HW 3
 *
 *    date_mmddyyyy.h
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

struct mmddyyyy
{
	UI mm:4;
	UI dd:5;
	UI yyyy:23; // 32 - (4+5) = 32 - 9 = 23
	bool operator<(const mmddyyyy c) const;
};

struct date_range
{
	mmddyyyy start;
	mmddyyyy end;
};

mmddyyyy extract_date(char *date);
bool construct_mmddyyyy(mmddyyyy &x, int dd, int mm, int yyyy);
bool split_date(const char *str, int &dd, int &mm, int &yyyy);
bool is_date_in_range(const mmddyyyy &key, const date_range &r);
bool is_leap_year(int year);
bool valid_date(int d, int m, int y);
string date_to_string(mmddyyyy x);
