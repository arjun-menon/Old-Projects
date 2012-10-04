/*
 *     CSE 230 HW 3
 *
 *       visit.h
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

struct visit
{
	UI ip;
	mmddyyyy date;
};

void extract_info(char *str, string &site_name, visit &visit);
