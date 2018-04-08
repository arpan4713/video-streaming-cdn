#ifndef NAMESERVER_H
#define NAMESERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <csignal>
#include <sys/time.h>

#include <vector>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

#define MAXPACKETSIZE 65536

using std::string;
using std::vector;
using std::cout;
using std::endl;

class Nameserver {
public:
    Nameserver(string logfile, int listen_port, int geography_based, string servers):
            logfile(logfile), listen_port(listen_port), geography_based(geography_based){
        if (geography_based == 0) {
            rr_index = 0;
            parse_rr_addrs(servers);
        } else if (geography_based == 1) {
            // geography based
        }
    }
    void dns_listen();
    string get_next_addr();

protected:
    string logfile;
    int listen_port;
    int geography_based;

    int rr_index; // make sure to wrap around
    vector<string> ip_addrs;

    void parse_rr_addrs(string);

};

#endif //NAMESERVER_H
