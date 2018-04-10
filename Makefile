all: nameserver miProxy dns_lib

dns_lib: dns_lib.cpp dns_lib.h
    g++ -o dns_lib -std=c++11 dns_lib.cpp

nameserver: nameserver.h DNSHeader.h DNSQuestion.h DNSRecord.h dns_lib.h nameserver.cpp
	g++ -o nameserver -std=c++11 nameserver.cpp

miProxy: miProxy.h miProxy.cpp
	g++ -o miProxy -std=c++11 miProxy.cpp

clean: miProxy
	rm -fr miProxy nameserver dns_lib *.o
