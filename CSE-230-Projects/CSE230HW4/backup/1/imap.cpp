/*
 *     CSE 230 HW 4
 *
 *     infomap.cpp
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

#include "common.h"

void MapFeed(char *str, imap &m)
{
	visit v;
	string site_name="";

	extract_info(str, site_name, v);

	// look for existing entry with same site_name
	if(m.find(site_name)==m.end())
	{
		// no existing entry with same site_name, so create one.
		vmap tm;
		dvec tv;

		tv.push_back(v.date);
		tm[v.ip] = tv;
		m[site_name] = tm;
	}
	else
	{
		// found existing entry with same site_name, append to it.
		vmap &tm = m[site_name];

		// look for existing entry with same IP address
		if(tm.find(v.ip)==tm.end())
		{
			// no existing entry with same IP address, so create one.
			dvec tv;
			tv.push_back(v.date);
			tm[v.ip] = tv;
		}
		else
		{
			// found existing entry with same IP address, append to it.
			dvec &tv = tm[v.ip];
			tv.push_back(v.date);
		}
	}
}

imap CreateMap(const vector<string> &lines)
{
	if(lines.size()<1)
		throw (string)("Empty file.");

	imap rmap;

	for(UI i=0; i<lines.size(); i++)
	{
		if(lines[i].size()>0)
		{
			// create cstring
			char *str = new char[lines[i].size()+1];
			if(str==NULL) throw (string)("Fatal Error");

			strcpy(str, lines[i].c_str());

			try
			{
				MapFeed(str, rmap);
			}
			catch(string err)
			{
				cout<<"Line "<<i+1<<": "<<err<<endl;
			}
			delete str;
		}
	}

	return rmap;
}

vector<string> LinearizeFile(const string &fname)
{
	vector<string> lines;
	ifstream f(fname.c_str());

	if(!f.is_open())
		throw (string)("Could not open the file \"" + fname + "\".\n");
	else
	{
		for(string s = ""; !f.eof(); lines.push_back((string)(s)))
			getline(f, s);
		f.close();
	}

	return lines;
}

void display_map(const imap &rmap)
{
	for(imap::const_iterator i=rmap.begin(); i!=rmap.end(); i++)
	{
		cout<<i->first<<"\n"<<multichar('-', i->first.length())<<"\n";

		for(vmap::const_iterator j = i->second.begin(); j!=i->second.end(); j++)
		{
			cout<<convert_IP_UI_to_string(j->first)<<" => ";
			
			for(dvec::const_iterator k = j->second.begin(); k!=j->second.end(); k++)
				cout<<k->mm<<"/"<<k->dd<<"/"<<k->yyyy<<"; ";
			
			cout<<endl;
		}

		cout<<endl;
	}
}
