/*
 *     CSE 230 HW 3
 *
 *    date_mmddyyyy.cpp
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

#include "common.h"

bool mmddyyyy::operator<(const mmddyyyy c) const
{
	if(yyyy < c.yyyy)
		return true;
	else if(yyyy == c.yyyy)
	{
		if(mm < c.mm)
			return true;
		else if(mm == c.mm)
		{
			if(dd < c.dd)
				return true;
			else // dd >= c.dd
				return false;
		}
		else // mm > c.mm
			return false;
	}
	else // yyyy > c.yyyy
		return false;
}

mmddyyyy extract_date(char *date)
{
	int d=0, m=0, y=0;
	if(!split_date(date, d, m, y))
		throw (string)("invalid date format (not mm/dd/yyyy) ...");

	mmddyyyy sd;
	if(!construct_mmddyyyy(sd, d, m, y))
		throw (string)("invalid date ...");
	return sd;
}

bool construct_mmddyyyy(mmddyyyy &x, int dd, int mm, int yyyy)
{
	if(!valid_date(dd,mm,yyyy))
		return false;

	x.dd = dd;
	x.mm = mm;
	x.yyyy = yyyy;

	return true;
}

bool split_date(const char *str, int &dd, int &mm, int &yyyy)
{
	/*
	mm/dd/yyyy
	  |  |    
    0123456789
	length=10.
	*/

	#define ASSERT(X) if(!(X)) goto error;

	ASSERT(strlen(str)==10)

	#define ISNUM(X)  (X>='0' && X<='9')
	#define AISNUM(X)  ASSERT(ISNUM(X))

	AISNUM(str[0])
	AISNUM(str[1])
	ASSERT(str[2]=='/')
	AISNUM(str[3])
	AISNUM(str[4])
	ASSERT(str[5]=='/')
	AISNUM(str[6])
	AISNUM(str[7])
	AISNUM(str[8])
	AISNUM(str[9])

	#undef ISNUM
	#undef AISNUM
	#undef ASSERT

	// success

	char ts[5];

	strncpy(ts, &str[0], 2);
	mm = atoi(ts);
	
	strncpy(ts, &str[3], 2);
	dd = atoi(ts);

	strncpy(ts, &str[6], 4);
	yyyy = atoi(ts);

	return true;

error:
	return false;
}

bool is_date_in_range(const mmddyyyy &key, const date_range &r)
{
	if((( key.dd >= r.start.dd ) && ( key.dd <= r.end.dd ))
	&& (( key.mm >= r.start.mm ) && ( key.mm <= r.end.mm ))
	&& (( key.yyyy >= r.start.yyyy ) && ( key.yyyy <= r.end.yyyy )))
		return true;
	return false;
}

bool valid_date(int d, int m, int y)
{
    if(m==1 || m==3 || m==5 || m==7 || m==8 || m==10 || m==12)
        if(d>=1 && d<= 31)
            return true;

    else if(m==4 || m==6 || m==9 || m==11)
        if(d>=1 && d<= 30)
            return true;

	else if(m==2)
	{
		if(is_leap_year(y)) {
            if(d>=1 && d<=29)
                return true;
		} else {
            if(d>=1 && d<=28)
                return true;
		}
	}

	return false;		
}

bool is_leap_year(int year)
{
	if(year%4==0) {
		if(year%100==0) {
			if(year%400==0)
				return true;
            else
				return false;
		} else
			return true;
	} else
        return false;
}

string date_to_string(mmddyyyy x)
{
	char str[11];
	sprintf(str, "%u\\%u\\%u", x.mm, x.dd, x.yyyy);
	return (string)(str);
}
