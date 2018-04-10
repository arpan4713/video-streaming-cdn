#ifndef __DNS_Record_H__
#define __DNS_Record_H__

#include <string.h>

struct DNSRecord
{
	char NAME[100];
	unsigned short int TYPE;
	unsigned short int CLASS;
	unsigned short int TTL;
	unsigned short int RDLENGTH;
	char RDATA[100];

	DNSRecord()
	{
		memset(NAME, '\0', 100);
		memset(RDATA, '\0', 100);
	}
};

#endif