/*
 *     CSE 230 HW 4
 *
 *       main.cpp
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

#include "common.h"

int main()
{
	cout<<"Enter the name of a file to load: ";
	string fname = "";
	cin>>fname;
	
	cout<<"Loading "<<fname<<"...\n\n";

	imap rmap;

	try
	{
		rmap = CreateMap(LinearizeFile(fname));
	}
	catch(string error)
	{
		cout<<error<<endl;
		return 1;
	}
	cout<<endl;

	int i=0;
	string s="";

	do
	{
		cout<<"Please select a menu option: "<<endl;
		cout<<"1)Link Information in date range"<<endl;
		cout<<"2)Information about all links"<<endl;
		cout<<"3)Link barchart for last 12 months"<<endl;
		cout<<"4)Display contents of imap"<<endl;
		cout<<"5)Quit the program"<<endl;
		cout<<"Option (1,2,3,4,5): ";

		cin>>s;
		i = atoi(s.c_str());
		cout<<endl;

		switch(i)
		{
		case 1:
			link_info_in_date_range(rmap);
			cout<<endl;
			break;
		case 2:
			info_about_all_links(rmap);
			cout<<endl;
			break;
		case 3:
			link_barchart(rmap);
			cout<<endl;
			break;
		case 4:
			display_map(rmap);
			cout<<endl;
			break;
		}
	}
	while(i!=5);

	return 0;
}
