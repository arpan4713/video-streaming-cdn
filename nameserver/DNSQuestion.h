#ifndef __DNS_Question_H__
#define __DNS_Question_H__

#include <string.h>

struct DNSQuestion
{
	char QNAME[100];
	unsigned short int QTYPE;
	unsigned short int QCLASS;

	DNSQuestion()
	{
		memset(QNAME, '\0', 100);
	}
};

#endif