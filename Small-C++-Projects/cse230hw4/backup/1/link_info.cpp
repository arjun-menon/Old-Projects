/*
 *     CSE 230 HW 4
 *
 *     link_info.cpp
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

#include "common.h"

link_info::link_info(const imap &r, string site_name)
{
	if(r.find(site_name)==r.end()) {
		fail();
		return;
	}
	const vmap visits = r.find(site_name)->second;
	x[0] = unique_visitors(visits);
	x[1] = count_visitors(visits, true);
	x[2] = count_visitors(visits, false);
	sn = site_name;
}

link_info::link_info(const imap &r, string site_name, const date_range &dr)
{
	if(r.find(site_name)==r.end()) {
		fail();
		return;
	}
	vmap visits = r.find(site_name)->second;
	x[0] = unique_visitors(visits, dr);
	x[1] = count_visitors(visits, true, dr);
	x[2] = count_visitors(visits, false, dr);
	sn = site_name;
}

void link_info::fail()
{
	sn = "";
	x[0] = x[1] = x[2] = 0;
}

UI link_info::unique_visitors(const vmap &visits)
{
	return visits.size();
}

UI link_info::unique_visitors(const vmap &visits, const date_range &dr)
{
	UI count = 0;

	for(vmap::const_iterator i = visits.begin(); i!=visits.end(); i++)
	{
		for(dvec::const_iterator j = i->second.begin(); j!=i->second.end(); j++)
		{
			if(is_date_in_range(*j, dr))
			{
				count++;
				break;
			}
		}
	}

	return count;
}

UI link_info::count_visitors(const vmap &visits, const bool returning)
{
	UI count = 0;
	for(vmap::const_iterator i = visits.begin(); i!=visits.end(); i++)
	{
		count += i->second.size();
		count -= (count>0) ? (returning ? 1:0) : 0;
	}
	return count;
}

UI link_info::count_visitors(const vmap &visits, const bool returning, const date_range &dr)
{
	UI count = 0;

	for(vmap::const_iterator i = visits.begin(); i!=visits.end(); i++)
	{
		for(dvec::const_iterator j = i->second.begin(); j!=i->second.end(); j++)
			if(is_date_in_range(*j, dr))
				count++;
		count -= (count>0) ? (returning ? 1:0) : 0;
	}

	return count;
}

bool link_info::operator <(link_info c)
{
	return (x[0] < c.x[0]);
}

UI link_info::unique()
{
	return x[0];
}

UI link_info::returning()
{
	return x[1];
}

UI link_info::total()
{
	return x[2];
}

string link_info::name()
{
	return sn;
}
