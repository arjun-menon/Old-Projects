/*
 *     CSE 230 HW 4
 *
 *      infomap.h
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

typedef vector<mmddyyyy> dvec;
typedef map<UI, dvec> vmap;
typedef map<string, vmap> imap;

imap CreateMap(const vector<string> &lines);
vector<string> LinearizeFile(const string &fname);
void display_map(const imap &rmap);
