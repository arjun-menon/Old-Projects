/*
 *     CSE 230 HW 3
 *
 *    ip_address.h
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

struct IP_adr
{
	unsigned char seg1;
	unsigned char seg2;
	unsigned char seg3;
	unsigned char seg4;
};

UI extract_ip(char *ips);
bool construct_IP_adr(char *str, struct IP_adr *x);
UI transform_IP(struct IP_adr x);
string convert_IP_UI_to_string(UI n);
bool get_ip_adr_num(const char *str, unsigned char *c);
