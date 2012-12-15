/*
 *     CSE 230 HW 4
 *
 *     link_info.h
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

class link_info
{
private:
	UI x[3]; // unique, returning, total.
	string sn;
	void fail();
public:
	link_info(const imap &r, string site_name);
	link_info(const imap &r, string site_name, const date_range &dr);
	static UI unique_visitors(const vmap &visits);
	static UI unique_visitors(const vmap &visits, const date_range &dr);
	static UI count_visitors(const vmap &visits, const bool returning);
	static UI count_visitors(const vmap &visits, const bool returning, const date_range &dr);
	bool operator<(link_info c);
	UI unique();
	UI returning();
	UI total();
	string name();
};
