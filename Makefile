all: nameserver miProxy

nameserver: nameserver.h DNSHeader.h DNSQuestion.h DNSRecord.h dns_lib.h nameserver.cpp
	g++ -o nameserver -std=c++11 nameserver.cpp

miProxy: miProxy.h dns_lib.h miProxy.cpp
	g++ -o miProxy -std=c++11 miProxy.cpp

clean: miProxy nameserver
	rm -fr miProxy nameserver *.o
