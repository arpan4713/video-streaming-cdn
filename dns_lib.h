#ifndef _DNS_LIB_H
#define _DNS_LIB_H

#include "DNSHeader.h"
#include "DNSQuestion.h"
#include "DNSRecord.h"

struct DNSMessage
{
    DNSHeader header;
    DNSQuestion question;
    DNSRecord answer;
    DNSRecord authority;
    DNSRecord additional;
};


#endif //_DNS_LIB_H
