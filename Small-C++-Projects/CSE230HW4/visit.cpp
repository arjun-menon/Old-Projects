/*
 *     CSE 230 HW 3
 *
 *      visit.cpp
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

#include "common.h"

void extract_info(char *str, string &site_name, visit &visit)
{
	char *ips=NULL, *site=NULL, *date=NULL;

	#define TOK_ERR "Found a record with invalid formatting... ignoring entry"

	if((ips = strtok(str," "))==NULL)
		throw (string)(TOK_ERR);
	if((site = strtok(NULL," "))==NULL)
		throw (string)(TOK_ERR);
	if((date = strtok(NULL," "))==NULL)
		throw (string)(TOK_ERR);

	#undef TOK_ERR

	if(!strlen(site))
		throw("Found a record with site name missing... ignoring entry");

	UI ipn;
	mmddyyyy sd;

	try {
		ipn = extract_ip(ips);
		sd = extract_date(date);
	}
	catch(string e) {
		throw "Found a record with an " + e + " ignoring entry";
	}

	// success
	site_name = site;
	visit.ip = ipn;
	visit.date = sd;
}
