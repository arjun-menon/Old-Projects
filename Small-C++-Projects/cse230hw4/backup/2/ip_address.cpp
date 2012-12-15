/*
 *     CSE 230 HW 3
 *
 *    ip_address.cpp
 *
 *  Written by Arjun G. Menon
 *
 *             arjungmenon@gmail.com
 *         www.arjungmenon.com
 *
 */

#include "common.h"

UI extract_ip(char *ips)
{
	struct IP_adr ip;
	if(!construct_IP_adr(ips, &ip))
		throw (string)("invalid IP address format ...");
	return transform_IP(ip);
}

bool construct_IP_adr(char *str, struct IP_adr *x)
{
	char *tok = NULL;
	bool success = true;

	#define TOKENIZE_IP_ADR(STR,SEGX) \
	if((tok = strtok (STR,"."))!=NULL) \
		success = get_ip_adr_num(tok,&(x->SEGX)); \
	else return false;

	TOKENIZE_IP_ADR(str,seg1);
	TOKENIZE_IP_ADR(NULL,seg2);
	TOKENIZE_IP_ADR(NULL,seg3);
	TOKENIZE_IP_ADR(NULL,seg4);

	#undef TOKENIZE_IP_ADR

	if(!success)
		throw (string)("invalid IP address ...");

	return success;
}

UI transform_IP(struct IP_adr x)
{
	return ((UI)(x.seg4)
	| (UI)(x.seg3) << 8
	| (UI)(x.seg2) << 16
	| (UI)(x.seg1) << 24);

}

string convert_IP_UI_to_string(UI n)
{
	char s[16]; // { ***.***.***.***\0 } 3+1+3+1+3+1+3+1 = 16 chars
	sprintf(s,"%u.%u.%u.%u", n>>24,(n<<8)>>24,(n<<16)>>24,(n<<24)>>24);
	return (string)(s);
}

bool get_ip_adr_num(const char *str, unsigned char *c)
{
	if( !(*str>='0' && *str<='9') )
		return false;

	int x = atoi(str);

	if(x>=0 && x<256)
		*c = (unsigned char)(x);
	else
		return false;
	
	return true;
}
