/*
 *     CSE 230 HW 4
 *
 *     barchart.cpp
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

#include "common.h"

void display_barchart(const vmap &r)
{
	map<mmyyyy, UI> hits = barchart_hits_map(construct_hits_map(r));
	star_adjust(hits);

	for(map<mmyyyy, UI>::iterator i = hits.begin(); i!=hits.end(); i++)
		cout<<i->first.mm<<"/"<<i->first.yyyy<<": "<<multichar('*',i->second)<<endl;
}

map<mmddyyyy, UI> construct_hits_map(const vmap &r)
{
	map<mmddyyyy, UI> k;

	// construct map of dates and number of visits in each date
	for(vmap::const_iterator i = r.begin(); i!=r.end(); i++)
	{
		for(dvec::const_iterator j = i->second.begin(); j!=i->second.end(); j++)
		{
			if(k.find(*j)==k.end())
				k[*j] = 1;
			else
				k[*j]++;
		}
	}

	// erase unwanted elements
	mmddyyyy td = k.rbegin()->first;
	td.yyyy--; // this serves as the upper bound for erasure
	k.erase(k.begin(), k.upper_bound(td));

	return k;
}

map<mmyyyy, UI> barchart_hits_map(const map<mmddyyyy, UI> &k)
{
	map<mmyyyy, UI> hits;
	{
		// built hits maps with zeroes
		mmyyyy first, last;
		first.mm = k.begin()->first.mm;
		first.yyyy = k.begin()->first.yyyy;
		last.mm = k.rbegin()->first.mm;
		last.yyyy = k.rbegin()->first.yyyy;

		while(!(last<first)) // last>=first
			hits[first++] = 0;
	}

	// fill up hits with numbers
	mmyyyy t;
	for(map<mmddyyyy, UI>::const_iterator i=k.begin(); i!=k.end(); i++)
	{
		t.mm = i->first.mm;
		t.yyyy = i->first.yyyy;

		if(hits.find(t)!=hits.end())
			hits[t] += i->second;
	}

	return hits;
}

void star_adjust(const map<mmyyyy, UI> &k)
{
	/*
	 * Adjusts the mapped value to represent the correct number of stars as per the requirement:
	 *
	 * "Each month must fit on one page, so if there are too many hits for a given month 
	 *  you must change the number of hits each star represents."
	 *
	 */

	// not implemented
}

const mmyyyy mmyyyy::operator++(int)
{
	mmyyyy temp = *this;

	if(mm==12)
	{
		mm = 1;
		yyyy++;
	}
	else
		mm++;

	return temp;
}

bool mmyyyy::operator<(const mmyyyy c) const
{
	if(yyyy < c.yyyy)
		return true;
	else if(yyyy == c.yyyy)
	{
		if(mm < c.mm)
			return true;
		else // mm >= c.mm
			return false;
	}
	else // yyyy > c.yyyy
		return false;
}

string multichar(char x, UI n)
{
	string k = "";

	while(n--)
		k += x;

	return k;
}
