#ifndef __DNS_HEADER_H__
#define __DNS_HEADER_H__

struct DNSHeader
{
	unsigned short int ID;
	bool QR;
	int OPCODE;
	bool AA;
	bool TC;
	bool RD;
	bool RA;
	char Z;
	char RCODE;
	unsigned short int QDCOUNT;
	unsigned short int ANCOUNT;
	unsigned short int NSCOUNT;
	unsigned short int ARCOUNT;
};

#endif