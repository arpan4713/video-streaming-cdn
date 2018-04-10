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
    Nameserver(string logfile, string listen_port, int geography_based, string servers):
            logfile(logfile), listen_port(listen_port), geography_based(geography_based){
        if (geography_based == 0) {
            rr_index = 0;
            parse_rr_addrs(servers);
        } else if (geography_based == 1) {
            // geography based
            proxy_ip = "10.0.0.2"; // hardcoded, remove for later
            parse_network_topo(servers);
            cout << get_next_addr() << endl;
        }
    }

    void dns_listen();
    int min_dist(bool[]);
    string get_next_addr();

protected:
    string logfile;
    string listen_port;
    int geography_based;

    int rr_index;
    vector<string> ip_addrs;

    string proxy_ip; // src IP for dijkstras
    vector<vector<int>> network_topo; // adjacency matrix
    vector<string> node_types; // node id (index) to node type
    vector<string> node_ips; // node id to node IP
    vector<int> dist; // distances src to each dst
    int V; // num nodes

    void parse_rr_addrs(string);
    void parse_network_topo(string);

};

#endif //NAMESERVER_H
