/*
 *     CSE 230 HW 4
 *
 *      barchart.h
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

struct mmyyyy
{
	UI mm:4;
	UI yyyy:28;
	const mmyyyy operator++(int);
	bool operator<(const mmyyyy c) const;
};

void display_barchart(const vmap &r);
map<mmddyyyy, UI> construct_hits_map(const vmap &r);
map<mmyyyy, UI> barchart_hits_map(const map<mmddyyyy, UI> &k);
void star_adjust(const map<mmyyyy, UI> &k);
string multichar(char x, UI n);
