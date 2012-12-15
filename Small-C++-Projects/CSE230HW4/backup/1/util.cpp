/*
 *     CSE 230 HW 4
 *
 *      util.cpp
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

#include "common.h"

void link_barchart(const imap &r)
{
	cout<<"Enter a link name: ";

	string link_name = "";
	cin>>link_name;

	if(r.find(link_name)==r.end()) {
		cout<<"Could not find link \'"<<link_name<<"\' in the record.\n";
		return;
	}

	cout<<endl<<"Bar chart for "<<link_name<<":"<<endl;
	display_barchart(r.find(link_name)->second);
}

void link_info_in_date_range(const imap &r)
{
	cout<<"Enter a link name: ";

	string link_name = "";
	cin>>link_name;

	if(r.find(link_name)==r.end()) {
		cout<<"Could not find link \'"<<link_name<<"\' in the record.\n";
		return;
	}
	
	date_range dr = input_a_date_range();
	link_info k(r, link_name, dr);

	cout.setf(ios::left);
	cout<<endl<<"Link: "<<link_name<<endl;
	cout<<setw(16)<<"Total hits: "<<k.total()<<endl;
	cout<<setw(16)<<"Unique: "<<k.unique()<<endl;
	cout<<setw(16)<<"Returning: "<<k.returning()<<endl;
}

void info_about_all_links(const imap &r)
{
	list<link_info> k;
	for(imap::const_iterator i = r.begin(); i!=r.end(); i++)
		k.push_back(link_info(r, i->first));
	k.sort();

	cout<<"     Link Name          Unique Visits    Return Visits   Total Visits"<<endl;
	cout<<"--------------------    -------------    -------------   ------------"<<endl;
	for(list<link_info>::iterator i = k.begin(); i!=k.end(); i++)
	{
		cout.setf(ios::left);
		cout<<setw(20)<<i->name()<<"          ";
		cout<<setw(17)<<i->unique();
		cout<<setw(15)<<i->returning();
		cout<<i->total();
		cout<<endl;
	}
}

date_range input_a_date_range()
{
	date_range dr;
	char str[11];

both_inputs:

start_input:
	cout<<"Enter a start date: ";
	fflush(stdin);
	fgets(str,11,stdin);
	if(*str=='\0'||*str=='\n')
		goto start_input;
	try { dr.start = extract_date(str); }
	catch(string e)
	{
		cout<<((char)(toupper(*(e.c_str()))) + e.substr(1, e.length()-1) + " please re-enter information\n");
		goto start_input;
	}

end_input:
	cout<<"Enter a finish date: ";
	fflush(stdin);
	fgets(str,11,stdin);
	if(*str=='\0'||*str=='\n')
		goto end_input;
	try { dr.end = extract_date(str); }
	catch(string e)
	{
		cout<<((char)(toupper(*(e.c_str()))) + e.substr(1, e.length()-1) + " please re-enter information\n");
		goto end_input;
	}

	if(dr.end < dr.start) {
		cout<<"Dates out of order... please re-enter information\n";
		goto both_inputs;
	}

	return dr;
}
