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

imap CreateMap(queue<string> lines);
queue<string> LinearizeFile(const string &fname);
void display_imap(const imap &rmap);
