#ifndef _MIPROXY_H
#define _MIPROXY_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <vector>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <csignal>
#include <sys/time.h>

#define BACKLOG 10
#define MAXPACKETSIZE 65536

using std::string;
using std::to_string;
using std::max;
using std::cout;
using std::endl;
using std::ofstream;
using std::vector;
using std::unordered_map;


bool check_video_data(string str);
string get_value(string str, string key);
string recv_response(int server_sd);
string get_chunkname(string request);

#endif
